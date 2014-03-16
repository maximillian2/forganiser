#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QTextCodec>
#include <QLabel>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // setting up locale to show russian letters
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // connecting to database
    db = QSqlDatabase::addDatabase("QSQLITE");

    //FIXME: locate file in more true way
    QString filePath = "/home/maxi/Qt_Projects/Forganiser/filmInfo.sqlite";
    QFileInfo fileInfoObject(filePath);

    if(fileInfoObject.exists())
    {
        db.setDatabaseName(filePath);
        qDebug() << "Database location:" << filePath;
    }
    else
    {
        //FIXME: remove another small message window w\ info
        QMessageBox::critical(0, tr("Error"), tr("Database not found"));
        qDebug() << "Error: Database not found";
    }
    // allocate memory for items
    model           =    new QSqlRelationalTableModel;
    manual_page     =    new manual;
    dialog          =    new AddFilm(model);
    plus            =    new QIcon(":/images/plus.png");
    remove          =    new QIcon(":/images/remove.png");
    unlock          =    new QIcon(":/images/unlock.png");
    lock            =    new QIcon(":/images/lock.png");

    checkConnection(db);
    model->setTable("information");
    model->setRelation(2, QSqlRelation("Rating", "id", "name"));
    model->setHeaderData(1,Qt::Horizontal, tr("Film title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Rating"));
    model->setHeaderData(3, Qt::Horizontal, tr("Place"));
    model->select();

    ui->films_tableview->setModel(model);
    ui->films_tableview->show();
    ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);        // editing rows off
    ui->films_tableview->setSelectionMode(QAbstractItemView::ContiguousSelection);
//    ui->films_tableview->setSelectionMode(QAbstractItemView::NoSelection);
    ui->films_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->films_tableview->hideColumn( /*id*/ 0);

    // statusbar consist label with No. of watched films
    setStatusMessage();

    connect(dialog, SIGNAL(accepted()), this, SLOT(updateInfo()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addFilmEntry()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(deleteFilmEntry()));

    createMenus();
    createActions();

    ui->unlockButton->setIcon(*lock);
    ui->addButton->setIcon(*plus);
    ui->removeButton->setIcon(*remove);

    ui->removeButton->setEnabled(false);
    ui->films_tableview->setSortingEnabled(true);
    ui->films_tableview->sortByColumn(0, Qt::AscendingOrder);

    connect(ui->delete_button, SIGNAL(clicked()), this, SLOT(deleteFilmEntry()));

    ui->films_tableview->resizeColumnsToContents(); // should be in the end of constructor

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
    delete film_number_label;

    delete dialog;

    db.close();
}

// first - private slots
// second - public slots
// third - functions

// row to delete
// TODO: disable remove button when not on the cell
void MainWindow::on_films_tableview_clicked(const QModelIndex &index)
{
    if(index.isValid())
    {
        ui->removeButton->setEnabled(true);
        selected_row = index.row();
    }

    // query to get selected row in table by select sql statement
    QSqlQuery query;
    query.prepare("SELECT information.title, information.place, rating.name FROM information, rating WHERE rating.id = information.rating LIMIT :row, 1;");
    query.bindValue(":row", selected_row);
    query.exec();

    // get to valid record through .next()
    query.next();

    ui->film_title_label->setText(query.value(0).toString());
    ui->place_Checkbox->setText(query.value(1).toString());
    ui->rating_label->setText(query.value(2).toString());
}

// edit field
void MainWindow::on_unlockButton_toggled(bool checked)
{
    if(checked)
    {
        ui->unlockButton->setIcon(*unlock);
        ui->films_tableview->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    else
    {
        ui->unlockButton->setIcon(*lock);
        ui->films_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->films_tableview->resizeColumnsToContents();
    }
}

void MainWindow::deleteFilmEntry()
{
    ui->films_tableview->model()->removeRow(selected_row);
    model->select();

    qDebug() << "**Deleted row #" << selected_row << "successfully." ;

    selected_row = -1;
    updateInfo();       // should this be in connect() or like function in this slot ???
//    ui->rating_label->setVisible(false);
//    ui->film_title_label->setVisible(false);
//    ui->place_Checkbox->setVisible(false);
//    ui->removeButton->setVisible(false);
}

void MainWindow::addFilmEntry()
{
    dialog->show();
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
    bool connected = db.open();
    if (!connected)
    {
        QMessageBox::critical(0, QObject::tr("Can't open database."),
        db.lastError().text());

        qDebug() << "Error: Can't open database.";
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

    delete_option = new QAction(tr("&Delete film"), this);
    delete_option->setShortcut(tr("Del"));
    connect(delete_option, SIGNAL(activated()), this, SLOT(deleteFilmEntry()));

    // adding actions to menu
    fileMenu->addAction(add_film_action);
    fileMenu->addAction(delete_option);
    fileMenu->addSeparator();
    fileMenu->addAction(exit);

    helpMenu->addAction(manual_option);
    helpMenu->addAction(about_option);
    helpMenu->addAction(about_qt_option);
}

void MainWindow::setStatusMessage()
{
    QSqlQuery *film_num_query = new QSqlQuery;

    ui->statusBar->removeWidget(film_number_label);

    // total films query
    film_num_query->exec("SELECT COUNT(*) FROM information");

    // get result as int
    film_num_query->next();
    film_number = film_num_query->value(0).toInt();

    // set statusbar message as label widget
    film_number_label = new QLabel("Total films watched: " + QString::number(film_number));
    ui->statusBar->addWidget(film_number_label);
}
