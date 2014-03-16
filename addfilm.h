#ifndef ADDFILM_H
#define ADDFILM_H

#include "mainwindow.h"

#include <QDialog>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QMessageBox>

namespace Ui {
class AddFilm;
}

class AddFilm : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddFilm(QSqlRelationalTableModel *model);
    ~AddFilm();

private slots:
    void on_ok_button_clicked();
    void on_cancel_button_clicked();

private:
    QString add_field;
    QString place;
    Ui::AddFilm *ui;
    QSqlRelationalTableModel *model_pointer;
    QMessageBox empty_title_message;
};

#endif // ADDFILM_H
