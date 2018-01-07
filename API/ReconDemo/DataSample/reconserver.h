#ifndef RECONSERVER_H
#define RECONSERVER_H

#include <QtNetwork/QTcpServer>

class ReconClientSocket;

class ReconServer : public QTcpServer
{
    Q_OBJECT
public:
    ReconServer(QObject * parent = nullptr, int port = 0);

signals:
    void signalDataReceived(QByteArray, int);
public slots:
    void slotDataReceived(QByteArray, int);    // message, length
    void slotDisconnected(int); // socket descriptor
protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;

private:
    ReconClientSocket * clientSocket;

};

#endif // RECONSERVER_H
