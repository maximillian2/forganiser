#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlRelationalTableModel>
#include <QShortcut>
#include <QMenu>
#include <QAction>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    QSqlRelationalTableModel *model;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    

public slots:
    void deleteEntry();
    void addEntry();

private slots:
    void on_films_tableview_customContextMenuRequested(const QPoint &pos);

    void on_films_tableview_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;
    QModelIndex index;
    QShortcut *add_shortcut;
    QShortcut *remove_shortcut;

    // menu
    QMenu *fileMenu;

    // menu actions
    QAction *add_film_action;
    QAction *manual_option;
    QAction *about_option;
    QAction *about_qt_option;

    int row_to_delete;
};

#endif // MAINWINDOW_H
