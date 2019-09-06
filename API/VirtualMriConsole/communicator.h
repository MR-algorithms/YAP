#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H
#include <QtNetwork/QTcpSocket>
#include <QObject>
#include <memory>
#include <complex>

using namespace std;


struct DataPackage;

class Communicator : public QTcpSocket
{
    Q_OBJECT
public:

    explicit Communicator(QObject *parent = 0);

    bool SetRemoteHost(const wchar_t * ip_address, unsigned short port);
    bool Connect();
    bool Disconnect();
    bool Send(const DataPackage& package);

protected slots:
    void slotDataReceived();
    void slotDisconnected();
    void slotConnected();

protected:

    std::shared_ptr<QHostAddress> _reconHost;
    unsigned short _port;

};

#endif // COMMUNICATOR_H
