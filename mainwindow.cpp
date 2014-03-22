#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addfilm.h"

#include <QSqlError>
#include <QDebug>
#include <QMenu>
#include <QTextCodec>
#include <QLabel>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // setting up locale to show russian letters
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // connecting to database
    qDebug() << QSqlDatabase::isDriverAvailable("QIBASE");
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
        qDebug() << "Error: Database not found at" << filePath;
    }
    // allocate memory for items
    model               =    new QSqlRelationalTableModel;
    plusIcon            =    new QIcon(":/images/plus.png");
    unlockIcon          =    new QIcon(":/images/unlock.png");
    lockIcon            =    new QIcon(":/images/lock.png");

    checkConnection(db);
    model->setTable("information");
    model->setRelation(2, QSqlRelation("Rating", "id", "name"));
    model->setHeaderData(1,Qt::Horizontal, tr("Film title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Rating"));
    model->setHeaderData(3, Qt::Horizontal, tr("Place"));
    model->select();

    ui->filmsTableView->setModel(model);
    ui->filmsTableView->show();
    ui->filmsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);        // editing rows off
    ui->filmsTableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    ui->filmsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->filmsTableView->hideColumn( /*id*/     0);
    ui->filmsTableView->hideColumn( /*rating*/ 2);
    ui->filmsTableView->hideColumn( /*place*/  3);
    ui->filmsTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // statusbar consist label with No. of watched films
    setStatusMessage();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->deleteButtonDetail, SIGNAL(clicked()), this, SLOT(deleteEntry()));

    createMenus();
    createActions();

    ui->unlockButton->setIcon(*lockIcon);
    ui->addButton->setIcon(*plusIcon);

    ui->filmsTableView->setSortingEnabled(true);
    ui->filmsTableView->sortByColumn(0, Qt::AscendingOrder);

    ui->placeCheckbox->setVisible(false);
    ui->watchedLabel->setVisible(false);
    ui->deleteButtonDetail->setVisible(false);

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    showPaneAction->setChecked(true);
    showStatusBarAction->setChecked(true);
    qDebug() << db.drivers();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;

    delete fileMenu;
    delete viewMenu;
    delete helpMenu;

    delete addAction;
    delete manualAction;
    delete aboutAction;
    delete aboutQtAction;
    delete exitAction;

    delete filmNumberLabel;
    db.close();
}

// first - private slots, second - public slots, third - functions

void MainWindow::on_filmsTableView_clicked(const QModelIndex &index)
{
    if(index.isValid())
    {
        selectedRow = index.row();
    }

    // query to get selected row in table by select sql statement
    QSqlQuery query;
    query.prepare("SELECT information.title, information.place, rating.name FROM information, rating WHERE rating.id = information.rating LIMIT :row, 1;");
    query.bindValue(":row", selectedRow);
    query.exec();

    // get to valid record through next()
    query.next();

    ui->filmTitleDetail->setText(query.value(0).toString());
    ui->placeCheckbox->setText(query.value(1).toString());
    ui->ratingLabelDetail->setText(query.value(2).toString());

    QWidgetList widgets = ui->scrollArea->findChildren<QWidget *>();
    foreach(QWidget *cur, widgets)
        cur->setVisible(true);
}

// editing feature
void MainWindow::on_unlockButton_toggled(bool checked)
{
    if(checked)
    {
        ui->unlockButton->setIcon(*unlockIcon);
        ui->filmsTableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    else
    {
        ui->unlockButton->setIcon(*lockIcon);
        ui->filmsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void MainWindow::deleteEntry()
{
    ui->filmsTableView->model()->removeRow(selectedRow);
    model->select();
    qDebug() << "**Deleted row #" << selectedRow << "successfully." ;

    selectedRow = -1;
    updateInfo();       // should this be in connect() or like function in this slot ???

    QWidgetList widgets = ui->scrollArea->findChildren<QWidget *>();
    foreach(QWidget *cur, widgets)
        cur->setVisible(false);

    ui->filmsTableView->clearSelection();
}

void MainWindow::addEntry()
{
    AddFilm *dialog = new AddFilm(model);
    connect(dialog, SIGNAL(accepted()), this, SLOT(updateInfo()));
    dialog->exec();
    updateInfo();
}

void MainWindow::applicationExitEntry()
{
    QCoreApplication::exit();
}

void MainWindow::aboutQtEntry()
{
    QMessageBox::aboutQt(0, QString("About"));
}

void MainWindow::aboutEntry()
{
    QMessageBox::about(this, tr("About Forganiser"), tr("<h2>Forganiser 0.77</h2>"
                                                        "<p>Copyright &copy; 2014 maxi"
                                                        "<p>Forganiser is a small apllication to store information about watched films."));
}

void MainWindow::manualEntry()
{
    QString documentationPage = "https://github.com/maximillian2/forganiser/wiki";
    QDesktopServices::openUrl(QUrl(documentationPage));
}

// table view/statusbar update
void MainWindow::updateInfo()
{
    setStatusMessage();
    ui->filmsTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

void MainWindow::showPaneEntry(bool visible)
{
    visible ? ui->scrollArea->setVisible(true) : ui->scrollArea->setVisible(false);
}

void MainWindow::showStatusBarEntry(bool visible)
{
    visible ? ui->statusBar->show() : ui->statusBar->hide();
}

void MainWindow::checkConnection(QSqlDatabase db)
{
    bool connected = db.open();
    if (!connected)
    {
        QMessageBox::critical(0, QObject::tr("Can't open database."),
        db.lastError().text());
        qDebug() << "Error: Can't open database.\n" << db.lastError().text();
    }
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Film"));
    viewMenu = menuBar()->addMenu(tr("&View"));
    helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createActions()
{
    addAction = new QAction(tr("&Add film"),  this);
    addAction->setShortcut(tr("Ctrl+A"));
    connect(addAction, SIGNAL(activated()), this, SLOT(addEntry()));

    deleteAction = new QAction(tr("&Delete film"), this);
    deleteAction->setShortcut(tr("Del"));
    connect(deleteAction, SIGNAL(activated()), this, SLOT(deleteEntry()));

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(activated()), this, SLOT(applicationExitEntry()));

    manualAction = new QAction(tr("&Online Manual"), this);
    manualAction->setShortcut(tr("F1"));
    connect(manualAction, SIGNAL(activated()), this, SLOT(manualEntry()));

    aboutAction = new QAction(tr("&About Forganiser"), this);
    connect(aboutAction, SIGNAL(activated()), this, SLOT(aboutEntry()));

    aboutQtAction = new QAction(tr("&About Qt"), this);
    connect(aboutQtAction, SIGNAL(activated()), this, SLOT(aboutQtEntry()));

    showPaneAction = new QAction(tr("&Show Pane"), this);
    showPaneAction->setCheckable(true);
    connect(showPaneAction, SIGNAL(toggled(bool)), this, SLOT(showPaneEntry(bool)));

    showStatusBarAction = new QAction(tr("&Show statusbar"), this);
    showStatusBarAction->setCheckable(true);
    connect(showStatusBarAction, SIGNAL(toggled(bool)), this, SLOT(showStatusBarEntry(bool)));

    // adding actions to menu
    fileMenu->addAction(addAction);
    fileMenu->addAction(deleteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu->addAction(showPaneAction);
    viewMenu->addAction(showStatusBarAction);

    helpMenu->addAction(manualAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::setStatusMessage()
{
    QSqlQuery *filmNumberQuery = new QSqlQuery("SELECT COUNT(*) FROM information");
    ui->statusBar->removeWidget(filmNumberLabel);

    // get result as int
    filmNumberQuery->next();
    filmNumber = filmNumberQuery->value(0).toInt();

    // set statusbar message as label widget
    filmNumberLabel = new QLabel("Total films: " + QString::number(filmNumber));
    ui->statusBar->addWidget(filmNumberLabel);

    delete filmNumberQuery;
}
