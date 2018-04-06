#ifndef DIALOGLOADGRIBMETEOFRANCE_H
#define DIALOGLOADGRIBMETEOFRANCE_H

#include <QDialog>

namespace Ui {
class DialogLoadGRIBMeteoFrance;
}

class DialogLoadGRIBMeteoFrance : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLoadGRIBMeteoFrance(QWidget *parent = 0);
    ~DialogLoadGRIBMeteoFrance();

private:
    Ui::DialogLoadGRIBMeteoFrance *ui;
};

#endif // DIALOGLOADGRIBMETEOFRANCE_H
