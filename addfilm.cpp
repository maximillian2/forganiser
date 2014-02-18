#include "addfilm.h"
#include "ui_addfilm.h"
#include <QDebug>

AddFilm::AddFilm(QSqlRelationalTableModel *model) : ui(new Ui::AddFilm)
{
    ui->setupUi(this);
    pointer = model;

    // default film place
    place = "Cinema";

    // set up modal window with warning
    empty_title_message.setModal(true);
    empty_title_message.setText("No film title specified. Please, try again.");
    empty_title_message.setStandardButtons(QMessageBox::Ok);
    empty_title_message.setIcon(QMessageBox::Information);

    ui->title_lineedit->setFocus();
    ui->ok_button->setDefault(true);
}

AddFilm::~AddFilm()
{
    delete ui;
    delete pointer;
}

void AddFilm::on_ok_button_clicked()
{
    ui->home_radioButton->isChecked() ? place = "Home" : place = "Cinema";

    if(ui->title_lineedit->text() != NULL)      // successful adding
    {
        add_field = "INSERT INTO Film_info (place, rating, title) VALUES ('" + place + "', " + ui->rating_spinBox->cleanText() + ", '" + ui->title_lineedit->text() + "');";
        qDebug() << add_field;

        QSqlQuery query;
        query.exec(add_field);
        pointer->select();
        pointer->submitAll();

        this->accept();
        //TODO: make total film number count a film when adding one
        ui->title_lineedit->clear();
        ui->title_lineedit->setFocus();
    }
    else
    {
        empty_title_message.show();
        ui->title_lineedit->setFocus();
        this->show();
    }


}

void AddFilm::on_cancel_button_clicked(){ this->hide(); }
