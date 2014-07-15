#include "addfilm.h"
#include "ui_addfilm.h"

#include <QDebug>
#include <QSqlError>

AddFilm::AddFilm(QSqlRelationalTableModel *model) : ui(new Ui::AddFilm)
{
    ui->setupUi(this);
    model_pointer = model;

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
    delete model_pointer;
}

void AddFilm::on_ok_button_clicked()
{
    ui->home_radioButton->isChecked() ? place = "Home" : place = "Cinema";

    if(ui->title_lineedit->text() != NULL)      // successful adding
    {
        add_field = "INSERT INTO information (title, rating, place) VALUES ('" + ui->title_lineedit->text() + "', " + ui->rating_spinBox->cleanText() + ", '" + place + "');";

        QSqlQuery *query = new QSqlQuery;
        query->exec(add_field);

        model_pointer->submitAll();
        model_pointer->select();

        this->accept();

        ui->title_lineedit->clear();
        ui->title_lineedit->setFocus();

        qDebug() << "**Added entry successfully.";
    }
    else
    {
        empty_title_message.show();
        ui->title_lineedit->setFocus();
        this->exec();
    }
}

void AddFilm::on_cancel_button_clicked()
{
    this->hide();
}
