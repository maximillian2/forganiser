#include "addfilm.h"
#include "ui_addfilm.h"
#include <QDebug>

//AddFilm::AddFilm(QWidget *parent) : QDialog(parent), ui(new Ui::AddFilm)
//{
//    ui->setupUi(this);
//}

AddFilm::AddFilm(QSqlRelationalTableModel* model) : ui(new Ui::AddFilm)
{
    ui->setupUi(this);
    pointer = model;

    empty_title_message.setModal(true);
    empty_title_message.setText("No film title specified. Please, try again.");
    empty_title_message.setStandardButtons(QMessageBox::Ok);
    empty_title_message.setIcon(QMessageBox::Information);

    ui->ok_button->setDefault(true);
}

AddFilm::~AddFilm()
{
    delete ui;
    delete pointer;
}

void AddFilm::on_buttonBox_accepted()
{
    QString add_field, place;
    bool success = true;

    ui->home_radioButton->isChecked() ? place = "Home" : place = "Cinema";

    if(ui->title_lineedit->text() == NULL) success = false;

    if(success)
    {
        add_field = "INSERT INTO Film_info (place, rating, title) VALUES ('" + place + "', " + ui->rating_spinBox->cleanText() + ", '" + ui->title_lineedit->text() + "');";
        qDebug() << add_field;

        QSqlQuery query;
        query.exec(add_field);

        pointer->select();
        pointer->submitAll();
//        this->hide();
    }
    else
    {
        empty_title_message.show();
//        this->show();
    }
}

void AddFilm::on_ok_button_clicked()
{
    QString add_field, place;
    bool success = true;


    ui->home_radioButton->isChecked() ? place = "Home" : place = "Cinema";

    if(ui->title_lineedit->text() == NULL) success = false;

    if(success)
    {
        add_field = "INSERT INTO Film_info (place, rating, title) VALUES ('" + place + "', " + ui->rating_spinBox->cleanText() + ", '" + ui->title_lineedit->text() + "');";
        qDebug() << add_field;

        QSqlQuery query;
        query.exec(add_field);

        pointer->select();
        pointer->submitAll();
        this->hide();
    }
    else
    {
        empty_title_message.show();
        this->show();
    }
}

void AddFilm::on_cancel_button_clicked()
{
    this->hide();
}
