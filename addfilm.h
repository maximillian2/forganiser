#ifndef ADDFILM_H
#define ADDFILM_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QMessageBox>
#include "mainwindow.h"

namespace Ui {
class AddFilm;
}

class AddFilm : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddFilm(QWidget *parent = 0);
    explicit AddFilm(QSqlRelationalTableModel* model);
    ~AddFilm();
    
private slots:
    void on_buttonBox_accepted();

    void on_ok_button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::AddFilm *ui;
    QSqlRelationalTableModel *pointer;
    QMessageBox empty_title_message;
};

#endif // ADDFILM_H
