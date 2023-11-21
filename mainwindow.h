#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "livraisons.h"
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pb_ajouter_clicked();

    void on_pb_supprimer_clicked();


    void on_pb_modifier_clicked();

    void on_pb_trier_clicked();

    void on_pb_rechercher_clicked();

    void on_pb_stat_clicked();

    void on_pb_pdf_clicked();
    void sendMail();
    void mailSent(QString);
    void browse();



private:
    Ui::MainWindow *ui;
    QStringList files;
    Livraisons L;
    QSortFilterProxyModel *proxyModel;

};

#endif // MAINWINDOW_H
