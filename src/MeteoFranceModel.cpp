#include "Util.h"
#include "MeteoFranceModel.h"


//----------------------------------------------------------------------------
// METEO FRANCE ABSTRACT CLASS
//---------------------------------------------------------------------------

MeteoFranceModel::MeteoFranceModel(QNetworkAccessManager *networkManager,
                                   int lat_min, int lon_min, int lat_max, int lon_max)
{
    m_networkManager = networkManager;
    m_lat_min = lat_min;
    m_lat_max = lat_max;
    m_lon_min = lon_min;
    m_lon_max = lon_max;
    m_args = QString("wgtprn");
    m_error = false;
}

void MeteoFranceModel::download()
{
    QNetworkRequest request;
    request.setUrl(m_api);
    m_reply = m_networkManager->get(request);

    connect(m_reply, SIGNAL(finished()), this, SLOT(slotFinished()));
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

QString MeteoFranceModel::getFullPathFileName(const QString &filename)
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
    return fullPath;
}

bool MeteoFranceModel::saveToDisk(const QString &fullPath, QIODevice *data)
{
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        // Display an error message because the file can't be opened
        return false;
    }
    file.write(data->readAll());
    file.close();
    return true;
}

// SLOTS

void MeteoFranceModel::slotFinished()
{
    if (!m_error)
    {
        QString fileName = getFileName();
        QString fullPathFileName = getFullPathFileName(fileName);
        bool fileSaved = saveToDisk(fullPathFileName, m_reply);
        if (fileSaved)
            emit signalGribSaved(fullPathFileName);
    }
}

void MeteoFranceModel::slotAbortDownload()
{
    m_error = true;
    m_reply->abort();
}


//----------------------------------------------------------------------------
// ARPEGE CLASS
//---------------------------------------------------------------------------

Arpege::Arpege(QNetworkAccessManager *networkManager,
               int lat_min, int lon_min, int lat_max, int lon_max)
: MeteoFranceModel(networkManager, lat_min, lon_min, lat_max, lon_max)
{
    getEndpoint();
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
}

QString Arpege::getFileName()
{
    QString fileName = getPartialFileName();
    return QString("Arpege") + fileName;
}


//----------------------------------------------------------------------------
// AROME CLASS
//---------------------------------------------------------------------------

Arome::Arome(QNetworkAccessManager *networkManager,
             int lat_min, int lon_min, int lat_max, int lon_max)
: MeteoFranceModel(networkManager, lat_min, lon_min, lat_max, lon_max)
{
    getEndpoint();
}

void Arome::getEndpoint()
{
    // API endpoint to donwload GRIB file
    // from the correct Meteo France model
    // http://195.154.231.142/grib/arome?
    //  x={long_min}&X={long_max}&y={lat_min}&Y={lat_max}&r={args}
    m_api = QUrl(QString("http://195.154.231.142/grib/arome?x=%1&X=%2&y=%3&Y=%4&r=%5").arg(
                     QString::number(m_lon_min), QString::number(m_lon_max),
                     QString::number(m_lat_min), QString::number(m_lat_max),
                     m_args)
                 );
}

QString Arome::getFileName()
{
    QString fileName = getPartialFileName();
    return QString("Arome") + fileName;
}
