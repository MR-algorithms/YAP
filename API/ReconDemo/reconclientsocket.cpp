#include "reconclientsocket.h"

ReconClientSocket::ReconClientSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &QTcpSocket::readyRead, this, &ReconClientSocket::slotDataReceived);
    connect(this, &QTcpSocket::disconnected, this, &ReconClientSocket::slotDisconnected);
}

void ReconClientSocket::slotDataReceived()
{
    while (bytesAvailable() > 0)
    {
        int length = bytesAvailable();
        char buf[1024];
        read(buf, length);
        QString msg = buf;
        emit signalDataReceived(msg, length);
    }
}

void ReconClientSocket::slotDisconnected()
{
    emit signalDisconnected(this->socketDescriptor());
}
