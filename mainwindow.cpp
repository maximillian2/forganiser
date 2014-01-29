#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addfilm.h"

#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // setting up locale to show russian letters
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // connecting to database
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("Films");
    db.setUserName("root");
    db.setPassword("123");

    // open check
    if (!db.open())
    {
        QMessageBox::critical(0, QObject::tr("Database Error"),
        db.lastError().text());
    }

    model = new QSqlRelationalTableModel;
    model->setTable("Film_info");
    model->select();

    ui->films_tableview->setModel(model);
    ui->films_tableview->show();

    ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->films_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->films_tableview->setSelectionMode(QAbstractItemView::SingleSelection);

//    ui->films_tableview->verticalHeader()->setVisible(false);

    ui->films_tableview->resizeColumnsToContents();       //fit into column field

    ui->films_tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    // setting shortcuts to add/remove action
    add_shortcut = new QShortcut(Qt::Key_A, this);
    remove_shortcut = new QShortcut(Qt::Key_D, this);

    connect(add_shortcut, SIGNAL(activated()), this, SLOT(addEntry()));
    connect(remove_shortcut, SIGNAL(activated()), this, SLOT(deleteEntry()));

    // doing menubar
    fileMenu = menuBar()->addMenu(tr("&Film"));

    // doing menu actions
    add_film_action = new QAction(tr("&Add film"),  this);                  // creating add option
    connect(add_film_action, SIGNAL(activated()), this, SLOT(addEntry()));  // and connecting it

    fileMenu->addAction(add_film_action);

    // adding help/about
    fileMenu = menuBar()->addMenu(tr("&Help"));

    manual_option = new QAction(tr("&Manual"), this);
//    connect(help_option, SIGNAL(activated()), this, SLOT(manualEntry()));

    about_option = new QAction(tr("&About"), this);
//    connect(about_option, SIGNAL(activated()), this, SLOT(aboutEntry()));

    about_qt_option = new QAction(tr("&About Qt"), this);
    connect(about_qt_option, SIGNAL(activated()), this, SLOT(aboutQt()));

    fileMenu->addAction(about_qt_option);
}

MainWindow::~MainWindow()
{
    delete ui;
    db.close();
}

void MainWindow::on_films_tableview_customContextMenuRequested(const QPoint &pos)
{
    index = ui->films_tableview->indexAt(pos);
    QMenu *menu = new QMenu(this);

    qDebug() << index.row();
    if(index.isValid())
    {
        menu->addAction("Add", this, SLOT(addEntry()));
        menu->addAction("Remove", this, SLOT(deleteEntry()));
        menu->popup(ui->films_tableview->viewport()->mapToGlobal(pos));
    }
    else
    {
        menu->addAction("Add", this, SLOT(addEntry()));
        menu->popup(ui->films_tableview->viewport()->mapToGlobal(pos));
    }
}

// delete film slot
void MainWindow::deleteEntry()
{
    ui->films_tableview->model()->removeRow(row_to_delete);
    model->submitAll();
    model->select();
//    qDebug() << "activated delete entry. Row is " << row_to_delete << " :)";
}

// add film slot
void MainWindow::addEntry()
{
    AddFilm *dialog = new AddFilm(model);
    dialog->show();
}

// helps to find out what row to delete
void MainWindow::on_films_tableview_clicked(const QModelIndex &index)
{
    if(index.isValid())
        row_to_delete = index.row();
//    qDebug() << "Row to delete" << row_to_delete;
    // just for fun
}
