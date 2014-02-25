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

    film_num_query = new QSqlQuery();

    // open check
    if (!db.open())
    {
        QMessageBox::critical(0, QObject::tr("Database Error"),
        db.lastError().text());
    }

    // set up main table
    model = new QSqlRelationalTableModel;
    model->setTable("Film_info");

    // set relation between rating table and main table
    model->setRelation(2, QSqlRelation("Rating", "id", "name"));

    model->select();

    ui->films_tableview->setModel(model);
    ui->films_tableview->show();

    // turn off editing rows and ability to select one field at time
    ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->films_tableview->setSelectionMode(QAbstractItemView::SingleSelection);

    // hide id from table
    ui->films_tableview->hideColumn(0);

    // set custom names to columns
    model->setHeaderData(1,Qt::Horizontal, tr("Film title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Rating"));
    model->setHeaderData(3, Qt::Horizontal, tr("Place"));

    //fit into column field
    ui->films_tableview->resizeColumnsToContents();

    ui->films_tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    // set up manual object
    manual_page = new manual;

    // set up add window
    dialog = new AddFilm(model);

    // statusbar consist label with number of watched films
    setStatusMessage();

    connect(dialog, SIGNAL(accepted()), this, SLOT(updateInfo()));
    connect(dialog, SIGNAL(rejected()), this, SLOT(updateInfo()));

    createMenus();
    createActions();

    //TODO: move definition to .h (clean up this code)
    QIcon *lock_close = new QIcon(":/images/lock-close.png");
    ui->unlockButton->setIcon(*lock_close);

    QIcon *plus = new QIcon(":/images/plus.png");
    ui->addButton->setIcon(*plus);
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

void MainWindow::on_films_tableview_customContextMenuRequested(const QPoint &pos)
{
    menu = new QMenu(this);
    index = ui->films_tableview->indexAt(pos);

    menu->addAction("Add", this, SLOT(addEntry()));

    if(index.isValid()) menu->addAction("Remove", this, SLOT(deleteEntry()));

    menu->popup(ui->films_tableview->viewport()->mapToGlobal(pos));
}

// delete film slot
void MainWindow::deleteEntry()
{
    ui->films_tableview->model()->removeRow(row_to_delete);
    model->submitAll();
    model->select();
    emit updateInfo();
}

// add film slot
void MainWindow::addEntry()
{
    dialog->show();
    setStatusMessage();
    ui->films_tableview->resizeColumnsToContents();
}

// edit film slot
void MainWindow::editEntry()
{
    dialog->show();
}

// helps to find out what row to delete
void MainWindow::on_films_tableview_clicked(const QModelIndex &index)
{
    if(index.isValid()) row_to_delete = index.row();
}

// exit from application slot
void MainWindow::safeExit(){ QCoreApplication::exit(); }

// about qt version window
void MainWindow::aboutQtEntry()
{
    QString string = "About";
    QMessageBox::aboutQt(0, string);
}

// about program window
void MainWindow::aboutEntry()
{
    QString title = "About Forganiser", text = "Program to store watched film info.";
    QMessageBox::about(0, title, text);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Film"));
    helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createActions()
{
    // add film feature
    add_film_action = new QAction(tr("&Add film"),  this);
    add_film_action->setShortcut(tr("Ctrl+A"));
    connect(add_film_action, SIGNAL(activated()), this, SLOT(addEntry()));

    // safe exit feature
    exit = new QAction(tr("&Exit"), this);
    exit->setShortcut(tr("Ctrl+Q"));
    connect(exit, SIGNAL(activated()), this, SLOT(safeExit()));

    // manual feature
    manual_option = new QAction(tr("&Manual"), this);
    manual_option->setShortcut(tr("F1"));
    connect(manual_option, SIGNAL(activated()), this, SLOT(manualEntry()));

    // about program feature
    about_option = new QAction(tr("&About Forganiser"), this);
    connect(about_option, SIGNAL(activated()), this, SLOT(aboutEntry()));

    // about qt feature
    about_qt_option = new QAction(tr("&About Qt"), this);
    connect(about_qt_option, SIGNAL(activated()), this, SLOT(aboutQtEntry()));

    // adding actions to menu
    fileMenu->addAction(add_film_action);
    fileMenu->addAction(exit);

    helpMenu->addAction(manual_option);
    helpMenu->addAction(about_option);
    helpMenu->addAction(about_qt_option);
}

// manual window
void MainWindow::manualEntry(){ manual_page->show(); }

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

// updates statusbar and table view
void MainWindow::updateInfo()
{
    setStatusMessage();
    ui->films_tableview->resizeColumnsToContents();
}

// edit fields in table
void MainWindow::on_unlockButton_toggled(bool checked)
{
    QIcon *open = new QIcon(":/images/lock-open.png");
    QIcon *close = new QIcon(":/images/lock-close.png");

    if(checked)
    {
        ui->unlockButton->setIcon(*open);
        ui->unlockButton->setText("Lock");
        ui->films_tableview->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    else
    {
        ui->unlockButton->setIcon(*close);
        ui->unlockButton->setText("Unlock");
        ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->films_tableview->resizeColumnsToContents();
    }
}
