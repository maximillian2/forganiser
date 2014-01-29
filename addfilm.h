#ifndef ADDFILM_H
#define ADDFILM_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
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

private:
    Ui::AddFilm *ui;
    QSqlRelationalTableModel* pointer;
};

#endif // ADDFILM_H
