//#include "addfilm.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QTextCodec>
#include <QLabel>

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

    // allocate memory for items
    film_num_query  =    new QSqlQuery;
    model           =    new QSqlRelationalTableModel;
    manual_page     =    new manual;
    dialog          =    new AddFilm(model);
    plus            =    new QIcon(":/images/plus.png");
    remove          =    new QIcon(":/images/remove.png");
    lock_open       =    new QIcon(":/images/lock-open.png");
    lock_close      =    new QIcon(":/images/lock-close.png");

    checkConnection(db);

    model->setTable("Film_info");
    model->setRelation(2, QSqlRelation("Rating", "id", "name"));
    model->setHeaderData(1,Qt::Horizontal, tr("Film title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Rating"));
    model->setHeaderData(3, Qt::Horizontal, tr("Place"));
    model->select();

    ui->films_tableview->setModel(model);
    ui->films_tableview->show();
    ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);        // editing rows off
    ui->films_tableview->setSelectionMode(QAbstractItemView::ContiguousSelection);
    ui->films_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->films_tableview->hideColumn( /*id*/ 0);
    ui->films_tableview->resizeColumnsToContents();
    ui->films_tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    // statusbar consist label with No. of watched films
    setStatusMessage();

    connect(dialog, SIGNAL(accepted()), this, SLOT(updateInfo()));

    createMenus();
    createActions();

    ui->unlockButton->setIcon(*lock_close);
    ui->addButton->setIcon(*plus);
    ui->removeButton->setIcon(*remove);

    // nothing to remove
    ui->removeButton->setEnabled(false);

    ui->films_tableview->selectionModel()->clearSelection();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;

    // menu
    delete fileMenu;
    delete helpMenu;

    // menu actions
    delete add_film_action;
    delete manual_option;
    delete about_option;
    delete about_qt_option;
    delete exit;

    delete manual_page;
    delete film_num_query;
    delete film_number_label;

    delete dialog;
    // database
    db.close();
}

// first - private slots
// second - public slots
// third - functions

// context menu
void MainWindow::on_films_tableview_customContextMenuRequested(const QPoint &pos)
{
    menu = new QMenu(this);
    index = ui->films_tableview->indexAt(pos);

    menu->addAction("Add", this, SLOT(addFilmEntry()));

    if(index.isValid())
    {
        menu->addAction("Remove", this, SLOT(deleteFilmEntry()));
    }
    menu->popup(ui->films_tableview->viewport()->mapToGlobal(pos));
}

// row to delete
void MainWindow::on_films_tableview_clicked(const QModelIndex &index)
{

    qDebug() << index.row();
    if(index.isValid())
    {
        qDebug() << "index is valid";
        row_to_delete = index.row();
        ui->removeButton->setEnabled(true);
    }
    else
    {
        ui->removeButton->setEnabled(false);
        qDebug() << "row is invalid";
    }
}

// edit field
void MainWindow::on_unlockButton_toggled(bool checked)
{
    if(checked)
    {
        ui->unlockButton->setIcon(*lock_open);
        ui->unlockButton->setText("Lock");
        ui->films_tableview->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    else
    {
        ui->unlockButton->setIcon(*lock_close);
        ui->unlockButton->setText("Unlock");
        ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->films_tableview->resizeColumnsToContents();
    }
}

void MainWindow::on_addButton_clicked()
{
    emit addFilmEntry();
}

void MainWindow::deleteFilmEntry()
{
    ui->films_tableview->model()->removeRow(row_to_delete);
    model->submitAll();
    model->select();
    emit updateInfo();
}

void MainWindow::addFilmEntry()
{
    dialog->show();
    emit updateInfo();
}

void MainWindow::applicationExit()
{
    QCoreApplication::exit();
}

void MainWindow::aboutQtEntry()
{
    QMessageBox::aboutQt(0, QString("About"));
}

void MainWindow::aboutProgramEntry()
{
    QMessageBox::about(0, QString("About Forganiser"), QString("Program to store watched film info."));
}

void MainWindow::manualPageEntry()
{
    manual_page->show();
}

// table view/statusbar update
void MainWindow::updateInfo()
{
    setStatusMessage();
    ui->films_tableview->resizeColumnsToContents();
}

void MainWindow::checkConnection(QSqlDatabase db)
{
    if (!db.open())
    {
        QMessageBox::critical(0, QObject::tr("Database Error"),
        db.lastError().text());
    }

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Film"));
    helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createActions()
{
    add_film_action = new QAction(tr("&Add film"),  this);
    add_film_action->setShortcut(tr("Ctrl+A"));
    connect(add_film_action, SIGNAL(activated()), this, SLOT(addFilmEntry()));

    exit = new QAction(tr("&Exit"), this);
    exit->setShortcut(tr("Ctrl+Q"));
    connect(exit, SIGNAL(activated()), this, SLOT(applicationExit()));

    manual_option = new QAction(tr("&Manual"), this);
    manual_option->setShortcut(tr("F1"));
    connect(manual_option, SIGNAL(activated()), this, SLOT(manualPageEntry()));

    about_option = new QAction(tr("&About Forganiser"), this);
    connect(about_option, SIGNAL(activated()), this, SLOT(aboutProgramEntry()));

    about_qt_option = new QAction(tr("&About Qt"), this);
    connect(about_qt_option, SIGNAL(activated()), this, SLOT(aboutQtEntry()));

    // adding actions to menu
    fileMenu->addAction(add_film_action);
    fileMenu->addAction(exit);
    helpMenu->addAction(manual_option);
    helpMenu->addAction(about_option);
    helpMenu->addAction(about_qt_option);
}

void MainWindow::setStatusMessage()
{
    ui->statusBar->removeWidget(film_number_label);

    // total films query
    film_num_query->exec("SELECT COUNT(*) FROM Film_info");

    // get result as int
    film_num_query->next();
    film_number = film_num_query->value(0).toInt();

    // set statusbar message as label widget
    film_number_label = new QLabel("Total films watched: " + QString::number(film_number));
    ui->statusBar->addWidget(film_number_label);
}

void MainWindow::on_removeButton_clicked()
{
    emit deleteFilmEntry();
}
