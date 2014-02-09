#include "manual.h"
#include "ui_manual.h"

#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>

#include <QDebug>

manual::manual(QWidget *parent) : QWidget(parent), ui(new Ui::manual)
{
    ui->setupUi(this);

    // TODO: добавить текст из файла, который находится в той же директрии, где и проект
    QString filename = "/home/maxi/Qt_Projects/Forganiser/manual.txt";
    QFile file(filename);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream input(&file);
        QString line;
        while(!input.atEnd())
        {
            line = input.readAll();
            ui->textBrowser->setText(line);
        }
    }
    ui->okButton->setDefault(true);
    ui->okButton->setFocus();
}

manual::~manual()
{
    delete ui;
}

void manual::on_okButton_clicked()
{
    this->hide();
}
