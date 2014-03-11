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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    AddFilm *dialog;

    void checkConnection(QSqlDatabase);
    void createMenus();
    void createActions();
    void setStatusMessage();

public slots:
    void deleteFilmEntry();
    void addFilmEntry();
    void applicationExit();
    void aboutQtEntry();
    void aboutProgramEntry();
    void manualPageEntry();
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

    QMenu *menu;
    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *add_film_action;
    QAction *manual_option;
    QAction *about_option;
    QAction *about_qt_option;
    QAction *delete_option;
    QAction *exit;

    QSqlQuery *film_num_query;
    QLabel *film_number_label;

    QIcon *lock;
    QIcon *unlock;
    QIcon *plus;
    QIcon *remove;

    int row_to_delete;
    int film_number;

};

#endif // MAINWINDOW_H


