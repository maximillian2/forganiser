#include "addfilm.h"
#include "ui_addfilm.h"
#include <QDebug>

AddFilm::AddFilm(QWidget *parent) : QDialog(parent), ui(new Ui::AddFilm)
{
    ui->setupUi(this);
}

AddFilm::AddFilm(QSqlRelationalTableModel* model) : ui(new Ui::AddFilm)
{
    ui->setupUi(this);
    pointer = model;
}


AddFilm::~AddFilm()
{
    delete ui;
}

void AddFilm::on_buttonBox_accepted()
{
    QString add_field, place;

    if(ui->home_radioButton->isChecked())
        place = "Home";
    else
        place = "Cinema";

    if(ui->title_lineedit->text() == NULL)
    {
        QMessageBox::warning(0, QObject::tr("Add error!"), QObject::tr("Empty film title."));
        this->show();
    }
    else
    {
        add_field = "INSERT INTO Film_info (place, rating, title) VALUES ('" + place + "', " + ui->rating_spinBox->cleanText() + ", '" + ui->title_lineedit->text() + "');";
        qWarning(add_field.toLatin1());

        QSqlQuery query;
        query.exec(add_field);

        pointer->select();
        pointer->submitAll();

    }
}
