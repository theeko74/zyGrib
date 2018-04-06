#include <QDebug>
#include "MeteoFranceModel.h"
#include "Util.h"


//----------------------------------------------------------------------------
// METEO FRANCE ABSTRACT CLASS
//---------------------------------------------------------------------------

MeteoFranceModel::MeteoFranceModel(QNetworkAccessManager *networkManager, QWidget *parent,
                                   int lat_min, int lon_min, int lat_max, int lon_max)
{
    m_networkManager = networkManager;
    m_lat_min = lat_min;
    m_lat_max = lat_max;
    m_lon_min = lon_min;
    m_lon_max = lon_max;
    m_args = QString("wgtprn");
    m_parent = parent;
}

void MeteoFranceModel::download()
{
    QNetworkRequest request;
    request.setUrl(m_api);
    qDebug() << request.url();
    reply = m_networkManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
}

void MeteoFranceModel::slotFinished()
{
    qDebug() << reply->rawHeaderList();
    qDebug() << getFileName();
    bool fileSaved = saveToDisk(getFileName(), reply);
    qDebug() << "File saved?: " << fileSaved;
}

QString MeteoFranceModel::getPartialFileName()
{
    // Return a partial file name for the GRIB
    // file that will need to add first the name
    // of the model used (Arpege or Arome)
    QString fileName("_%1-%2-%3-%4_%5.grb");
    fileName = fileName.arg(QString::number(m_lat_min), QString::number(m_lat_max),
                            QString::number(m_lon_min), QString::number(m_lon_max),
                            QDate::currentDate().toString("dd-MM-yyyy"));
    return fileName;
}

bool MeteoFranceModel::saveToDisk(const QString &filename, QIODevice *data)
{
    // Get user preferences for the folder
    // used to save the GRIB files
    QString path = Util::getSetting("gribFilePath", "").toString();
    if (path == "")
        path = "./";
    else
        path += "/";

    // Open a dialog window to select
    // the folder to save the GRIB file
    QString fullPath = Util::getSaveFileName(NULL, tr("Save GRIB File"), path+filename);
    qDebug() << fullPath;

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        // Display an error message because the file can't be opened
        return false;
    }
    file.write(data->readAll());
    file.close();
    return true;
}



//----------------------------------------------------------------------------
// ARPEGE CLASS
//---------------------------------------------------------------------------

Arpege::Arpege(QNetworkAccessManager *networkManager, QWidget *parent,
               int lat_min, int lon_min, int lat_max, int lon_max)
: MeteoFranceModel(networkManager, parent, lat_min, lon_min, lat_max, lon_max)
{
    getEndpoint();
}

Arpege::~Arpege()
{

}

void Arpege::getEndpoint()
{
    // API endpoint to donwload GRIB file
    // from the correct Meteo France model
    // http://195.154.231.142/grib/arpege?
    //  x={long_min}&X={long_max}&y={lat_min}&Y={lat_max}&r={args}
    m_api = QUrl(QString("http://195.154.231.142/grib/arpege?x=%1&X=%2&y=%3&Y=%4&r=%5").arg(
                     QString::number(m_lon_min), QString::number(m_lon_max),
                     QString::number(m_lat_min), QString::number(m_lat_max),
                     m_args)
                 );

    qDebug() << "Arpege created";
}

QString Arpege::getFileName()
{
    QString fileName = getPartialFileName();
    return QString("Arpege") + fileName;
}
