#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QPushButton>
#include <QTcpSocket>
#include <QNetworkSession>
namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();
    void moveToCenter();

private slots:
    //void requestNewFortune();
    //void readFortune();
    void displayError(QAbstractSocket::SocketError socketError);
    //void enableGetFortuneButton();
    void sessionOpened();
    void requestConnection();
    void getData();
private:
    Ui::Client *ui;
    QPushButton *connectButton;
    QPushButton *quitButton;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QNetworkSession *networkSession;

};

#endif // CLIENT_H
