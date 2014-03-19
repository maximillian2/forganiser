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

    void checkConnection(QSqlDatabase);
    void createMenus();
    void createActions();
    void setStatusMessage();

public slots:
    void addEntry();
    void deleteEntry();
    void applicationExitEntry();
    void aboutQtEntry();
    void aboutEntry();
    void manualEntry();
    void updateInfo();
    void showPaneEntry(bool hide);

private slots:
    void on_unlockButton_toggled(bool checked);
    void on_filmsTableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *model;

    QSqlDatabase db;
    QModelIndex index;

    QMenu *menu;
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *addAction;
    QAction *deleteAction;
    QAction *exitAction;

    QAction *showPaneAction;

    QAction *manualAction;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QLabel *filmNumberLabel;

    QIcon *lockIcon;
    QIcon *unlockIcon;
    QIcon *plusIcon;

    int selectedRow;
    int filmNumber;

};

#endif // MAINWINDOW_H


