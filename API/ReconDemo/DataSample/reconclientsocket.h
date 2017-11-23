#ifndef RECONCLINETSOCKET_H
#define RECONCLINETSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QObject>

class ReconClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ReconClientSocket(QObject *parent = 0);

signals:
    void signalDataReceived(QByteArray, int);
    void signalDisconnected(int);

public slots:
protected slots:
    void slotDataReceived();
    void slotDisconnected();
};

#endif // RECONCLINETSOCKET_H
