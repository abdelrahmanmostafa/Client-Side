#include "client.h"
#include "ui_client.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QSize>


Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),networkSession(0)
{
    ui->setupUi(this);

    // move window to the center of Screen
    moveToCenter();

    tcpSocket = new QTcpSocket(this);
    connect (ui->connectButton,SIGNAL(clicked()),this,SLOT(requestConnection()));
    connect (ui->quitButton,SIGNAL(clicked()),this,SLOT(close()));
    connect (tcpSocket,SIGNAL(readyRead()),this,SLOT(getData()));
    connect(tcpSocket,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));


    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        //getFortuneButton->setEnabled(false);
        //statusLabel->setText(tr("Opening network session."));
        networkSession->open();
}

}

Client::~Client()
{
    delete ui;
}

void Client::moveToCenter()
{

    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width(); // get width of screen
    screenHeight = desktop->height(); // get height of screen

    windowSize = size(); // size of our application window
    width = windowSize.width();
    height = windowSize.height();

    // little computations
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

    // move window to desired coordinates
    move ( x, y );


}

void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();


}


void Client::requestConnection()
{
    blockSize = 0;
    tcpSocket->abort();
    tcpSocket->connectToHost("localhost" ,2000);
    qDebug("connection to host successfully started");

}
void Client::getData()
{
//! [9]
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
//! [8]

//! [10]
        in >> blockSize;
    }

    if (tcpSocket->bytesAvailable() < blockSize)
        return;
//! [10] //! [11]

    QImage recievedImage;
    in >> recievedImage;
    if (recievedImage.isNull())
        {// Check if the image was indeed received
            qDebug("The image is null. Something failed.");
    }
    else
    {
    if(recievedImage.save("/home/abdelrahman/mytest1.jpg"))
        {
          qDebug()<<("Saved");

    }else
          {
              qDebug()<<("Not Saved");
          }
}
    QString nextFortune;
    in >> nextFortune;
/*
    if (nextFortune == currentFortune) {
        QTimer::singleShot(0, this, SLOT(requestNewFortune()));
        return;
    }
    */
qDebug()<<"Recieving";
qDebug()<<blockSize;

    //! [11]

//! [12]
    //currentFortune = nextFortune;
//! [9]
    //statusLabel->setText(currentFortune);
    //getFortuneButton->setEnabled(true);
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the  server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    //getFortuneButton->setEnabled(true);
}
