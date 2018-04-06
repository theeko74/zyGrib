#include "dialogloadgribmeteofrance.h"
#include "ui_dialogloadgribmeteofrance.h"

DialogLoadGRIBMeteoFrance::DialogLoadGRIBMeteoFrance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLoadGRIBMeteoFrance)
{
    ui->setupUi(this);
}

DialogLoadGRIBMeteoFrance::~DialogLoadGRIBMeteoFrance()
{
    delete ui;
}
