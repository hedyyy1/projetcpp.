#include "mainwindow.h"
#include<QApplication>
#include<iostream>
#include "connection.h"
using namespace std;
#include "ui_mainwindow.h"
#include "livraisons.h"
#include<QMessageBox>
#include<QIntValidator>
#include <QtDebug>
#include <QString>

#include <QPieSeries>
#include <QPieSlice>
#include <QChart>
#include <QChartView>
#include <QStackedWidget>
#include <QtCharts>
#include <QSqlQuery>

#include<QPrinter>
#include<QPainter>
#include<QPdfWriter>
#include<QTextTable>
#include<QTextDocument>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->numliv->setValidator(new QIntValidator(0, 999999, this));
    ui->numliv_m_4->setValidator(new QIntValidator(0, 999999, this));
    ui->numero->setValidator(new QIntValidator(0, 999999, this));

    ui->tablivraisons->setModel(L.afficher());
    proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(L.afficher());
        ui->tablivraisons->setModel(proxyModel);
        ui->dateliv->setValidator(new QRegExpValidator(QRegExp("\\d{2}/\\d{2}/\\d{4}"),this));
        ui->dateliv_m->setValidator(new QRegExpValidator(QRegExp("\\d{2}/\\d{2}/\\d{4}"),this));




}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pb_ajouter_clicked()
{
  qDebug();
   int NUMLIV=ui->numliv->text().toInt();
   QString DATELIV=ui->dateliv->text();
   QString ADRESSELIV=ui->adresseliv->text();
   QString STATUTLIV=ui->statutliv->currentText();

   Livraisons L(NUMLIV,DATELIV,ADRESSELIV,STATUTLIV);
   bool test=L.ajouter();
   if (test){



       QMessageBox::information(nullptr, QObject::tr("ok"),
                   QObject::tr("ajout avec success.\n"
                               "Click Cancel to exit."), QMessageBox::Cancel);
       ui->tablivraisons->setModel(L.afficher());

   }
   else
       QMessageBox::critical(nullptr, QObject::tr("not ok"),
                   QObject::tr(" ajout non effectué.\n"
                               "Click Cancel to exit."), QMessageBox::Cancel);

}





void MainWindow::on_pb_supprimer_clicked()

    {
        int NUMLIV = ui->numero->text().toInt();

        if (NUMLIV==-1) {
            // Show an error message if the number input is empty.
            QMessageBox::critical(nullptr, QObject::tr("Error"),
                QObject::tr("Veuillez entrer un numero avant de tenter de supprimer."), QMessageBox::Ok);
            return;
        }

        // Check if the number exists in the database.
        Livraisons L;
        L.setNUMLIV(NUMLIV);

        if (L.numExists(NUMLIV)) {
            // The ID exists, so proceed with the deletion.
            bool test = L.supprimer(L.getNUMLIV());

            if (test) {
                QMessageBox::information(nullptr, QObject::tr("OK"),
                    QObject::tr("Suppression avec succès.\nClick Cancel to exit."), QMessageBox::Cancel);
                ui->tablivraisons->setModel(L.afficher()); // Mise à jour
            }
            else {
                QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                    QObject::tr("La suppression a échoué.\nClick Cancel to exit."), QMessageBox::Cancel);
            }
        }
        else {
            // Show an error message if the ID does not exist in the database and return from the function.
            QMessageBox::critical(nullptr, QObject::tr("Error"),
                QObject::tr("L'identifiant n'existe pas."), QMessageBox::Ok);
            return;
        }
    }





void MainWindow::on_pb_modifier_clicked()
{
    {
        int NUMLIV = ui->numliv_m_4->text().toInt();
        Livraisons L;
        L.setNUMLIV(NUMLIV);
        QString newDATELIV=ui->dateliv_m->text();
        QString newADRESSELIV=ui->adresseliv_m->text();
        QString newSTATUTLIV=ui->statutliv_m->currentText();

        if (!L.numExists(NUMLIV)) {
                QMessageBox::critical(nullptr, QObject::tr("Error"),
                    QObject::tr("Le numero n'existe pas dans la base de données. Veuillez choisir un identifiant unique."), QMessageBox::Ok);
                return; // number already exists, so don't proceed with modification.
            }

        // If the num is unique and changes are made, update the livraison's information.
        L.setNUMLIV(NUMLIV);
        L.setADRESSELIV(newADRESSELIV);
        L.setSTATUTLIV( newSTATUTLIV);
        L.setDATELIV(newDATELIV);

        bool test = L.modifier();

        if (test) {
            QMessageBox::information(nullptr, QObject::tr("OK"),
                QObject::tr("Modification avec succès.\nClick Cancel to exit."), QMessageBox::Cancel);
            ui->tablivraisons->setModel(L.afficher()); // Mise à jour
        } else {
            QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                QObject::tr("La Modification a échoué.\nClick Cancel to exit."), QMessageBox::Cancel);
        }
    }
}

void MainWindow::on_pb_trier_clicked()
{
    ui->tablivraisons->sortByColumn(0, Qt::DescendingOrder);

}

void MainWindow::on_pb_rechercher_clicked()
{
    int NUMLIV = ui->numero->text().toInt(); // Récupérez le numéro de livraison entré par l'utilisateur.

    // Créez un modèle de requête SQL pour afficher la livraison recherchée.
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT  * FROM livraisons WHERE NUMLIV = :NUMLIV");
    query.bindValue(":NUMLIV", NUMLIV);

    if (query.exec()) {
        model->setQuery(query);
        ui->tablivraisons->setModel(model);

    } else {
        // Gérez les erreurs de requête SQL si nécessaire.
        qDebug() << "Erreur de requête SQL : " << query.lastError().text();
    }
}



void MainWindow::on_pb_stat_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT STATUTLIV, COUNT(*) FROM LIVRAISONS GROUP BY STATUTLIV");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête.";
        //db.close();
        return;
    }

    // Création de la série de données pour le graphique en secteurs
    QPieSeries *series = new QPieSeries();

    while (query.next()) {
        QString statutliv = query.value(0).toString();
        int count = query.value(1).toInt();
        series->append(statutliv, count);
    }

    foreach(QPieSlice *slice, series->slices()) {
        QString label = QString("%1 (%2%)")
            .arg(slice->label())
            .arg(100 * slice->percentage(), 0, 'f', 1);
        slice->setLabel(label);
    }

    // Création du graphique et ajout de la série de données
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des livraisons selon le statut ");

    // Configuration du graphique
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->setAnimationOptions(QChart::AllAnimations);

    // Affichage du graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);
    chartView->show();
}


void MainWindow::on_pb_pdf_clicked()
{  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty()) {
        return;  // L'utilisateur a annulé la boîte de dialogue
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    QTextCursor cursor(&doc);

    // En-tête du document
    QTextCharFormat headerFormat;
    headerFormat.setFontPointSize(14);
    headerFormat.setFontWeight(QFont::Bold);
    cursor.setCharFormat(headerFormat);
    cursor.insertText("Liste des livraisons\n\n");

    // Crée un tableau avec des colonnes pour chaque champ de la table
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignLeft);
    QTextTable *table = cursor.insertTable(1, 7, tableFormat);
    QTextCursor cellCursor;

    // Remplir les en-têtes de colonne
    cellCursor = table->cellAt(0, 0).firstCursorPosition();
    cellCursor.insertText("Numéro du livraison");

    cellCursor = table->cellAt(0, 1).firstCursorPosition();
    cellCursor.insertText("Etat");

    cellCursor = table->cellAt(0, 2).firstCursorPosition();
    cellCursor.insertText("Date");

    cellCursor = table->cellAt(0, 3).firstCursorPosition();
    cellCursor.insertText("Adresse");



    // Obtain the client data from your table model
    QSqlQueryModel *model = static_cast<QSqlQueryModel*>(ui->tablivraisons->model());

    // Fill the table data
    for (int row = 0; row < model->rowCount(); ++row) {
        table->appendRows(1);

        for (int col = 0; col < 7; ++col) {
            cellCursor = table->cellAt(row + 1, col).firstCursorPosition();
            cellCursor.insertText(model->data(model->index(row, col)).toString());
        }
    }

    doc.print(&printer);

    // Display a success message
    QMessageBox::information(this, "Succès", "Liste des livraisons exportée sous forme de PDF.");
}
