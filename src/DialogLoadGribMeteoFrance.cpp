#include "DialogLoadGribMeteoFrance.h"
#include "ui_dialogloadgribmeteofrance.h"

#include <QDebug>

DialogLoadGRIBMeteoFrance *globalDiag = NULL;

//-------------------------------------------------------------------------------
QString DialogLoadGRIBMeteoFrance::getFile(QNetworkAccessManager *networkManager,
                                           QWidget *parent, double x0, double y0,
                                           double x1, double y1)
{
    /* Static method that loads the Dialog Window
     * with downloading options for the Meteo France GRIB,
     * and return the filename of the GRIB file.
     */
    if (!globalDiag)
        globalDiag = new DialogLoadGRIBMeteoFrance(networkManager, parent);
    globalDiag->setZone(x0, y0, x1, y1);
    globalDiag->exec();
    return globalDiag->m_fullPathFileName;

    return QString("temp");
}


DialogLoadGRIBMeteoFrance::DialogLoadGRIBMeteoFrance(QNetworkAccessManager *networkManager,
                                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLoadGRIBMeteoFrance)
{
    ui->setupUi(this);
    m_networkManager = networkManager;

    // Init progress bar
    ui->progressBar->setValue(0);

    // Save old cursor before we change it
    m_oldCursor = cursor();
}

DialogLoadGRIBMeteoFrance::~DialogLoadGRIBMeteoFrance()
{
    delete ui;
    delete m_networkManager;
}

void DialogLoadGRIBMeteoFrance::setZone(double lon_min, double lat_min,
                                        double lon_max, double lat_max)
{
    double tmp;
    if (lon_min > lon_max)  { tmp=lon_min; lon_min=lon_max; lon_max=tmp; }
    if (lat_max > lat_min)  { tmp=lat_min; lat_min=lat_max; lat_max=tmp; }

    m_lat_min = (int)ceil(lat_min);
    m_lon_min = (int)floor(lon_min);
    m_lat_max = (int)floor(lat_max);
    m_lon_max = (int)ceil(lon_max);
}

// SLOTS

void DialogLoadGRIBMeteoFrance::slotBtOK()
{
    // Slot to be run when button 'OK'
    // is clicked by the user.

    // Start the loading bar
    // When setMinimum and setMaximum are set to 0 both, then QProgressBar only
    // displays a busy indicator as MeteoFrance GRIB server does not send the
    // content-length header...
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);

    // Display downloading status
    ui->downloadingInfo->setText("Meteo France's servers are very slow. Please be patient while downloading...");

    // Change cursor to waiting cursor
    setCursor(Qt::WaitCursor);

    // Start downloading.
    if (ui->radioArome->isChecked())
    {
        Arome *model = new Arome(m_networkManager,
                                 m_lat_min, m_lon_min, m_lat_max, m_lon_max);
        model->download();
        connect(this, SIGNAL(rejected()), model, SLOT(slotAbortDownload()));
        connect(model, SIGNAL(signalGribSaved(QString)), this, SLOT(slotGribSaved(QString)));
    }
    else
    {
        Arpege *model = new Arpege(m_networkManager,
                                   m_lat_min, m_lon_min, m_lat_max, m_lon_max);
        model->download();
        connect(this, SIGNAL(rejected()), model, SLOT(slotAbortDownload()));
        connect(model, SIGNAL(signalGribSaved(QString)), this, SLOT(slotGribSaved(QString)));
    }

}

void DialogLoadGRIBMeteoFrance::slotGribSaved(QString fullPathFileName)
{
    m_fullPathFileName = fullPathFileName;
    setCursor(m_oldCursor);
    accept();
}

