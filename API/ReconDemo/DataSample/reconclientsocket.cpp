#include "reconclientsocket.h"
#include "SampleDataProtocol.h"

ReconClientSocket::ReconClientSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &QTcpSocket::readyRead, this, &ReconClientSocket::slotDataReceived);
    connect(this, &QTcpSocket::disconnected, this, &ReconClientSocket::slotDisconnected);
}

void ReconClientSocket::slotDataReceived()
{

    while (bytesAvailable() > 0)
    {
        /*int length = bytesAvailable();
        char buf[1024];
        read(buf, length);
        QString msg = buf;
        int msglen = msg.length();

        emit signalDataReceived(msg, length);
        */

        int length = bytesAvailable();
        QByteArray data = this->readAll();
        IntAndFloatArray test;

        test.Unpack(data);
        emit signalDataReceived(data,length);


    }
}

void ReconClientSocket::slotDisconnected()
{
    emit signalDisconnected(this->socketDescriptor());
}
