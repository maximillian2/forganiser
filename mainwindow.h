#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlRelationalTableModel>
#include <QShortcut>
#include <QMenu>
#include <QAction>
#include <QSqlQuery>

#include "manual.h"
#include "addfilm.h"

namespace Ui {
class MainWindow;
}

class AddFilm;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    AddFilm *dialog;

    void createMenus();
    void createActions();

    void setStatusMessage();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void deleteEntry();
    void addEntry();
    void editEntry();
    void safeExit();
    void aboutQtEntry();
    void aboutEntry();
    void manualEntry();
    void updateInfo();

private slots:
    void on_films_tableview_customContextMenuRequested(const QPoint &pos);
    void on_films_tableview_clicked(const QModelIndex &index);
    void on_unlockButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *model;

    manual *manual_page;
    QSqlDatabase db;
    QModelIndex index;

    // menus
    QMenu *menu;
    QMenu *fileMenu;
    QMenu *helpMenu;

    // menu actions
    QAction *add_film_action;
    QAction *manual_option;
    QAction *about_option;
    QAction *about_qt_option;
    QAction *exit;

    QSqlQuery *film_num_query;
    QLabel *film_number_label;

    int row_to_delete;
    int film_number;

};

#endif // MAINWINDOW_H
