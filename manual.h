#ifndef MANUAL_H
#define MANUAL_H

#include <QWidget>

namespace Ui {
class manual;
}

class manual : public QWidget
{
    Q_OBJECT
    
public:
    explicit manual(QWidget *parent = 0);
    ~manual();
    
private slots:
    void on_okButton_clicked();

private:
    Ui::manual *ui;
};

#endif // MANUAL_H
