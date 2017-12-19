#include "communicator.h"
#include <QtNetwork/QHostAddress>
#include <QDebug>
#include <cassert>
#include <memory>

Communicator Communicator::s_instance;

Communicator::Communicator(QObject *parent): QTcpSocket(parent)
{
    connect(this, &QTcpSocket::connected,    this, &Communicator::slotConnected);
    connect(this, &QTcpSocket::readyRead,    this, &Communicator::slotDataReceived);
    connect(this, &QTcpSocket::disconnected, this, &Communicator::slotDisconnected);
}

bool Communicator::SetRemoteHost(QString ip_address, unsigned short port)
{
    //_ip_address = *ip_address;

    _reconHost = std::make_shared<QHostAddress>();

    if (!_reconHost->setAddress(ip_address))
    {
        qDebug()<<L"Server ip address error";
        return false;
    }

    _port = port;
    return true;
}

void Communicator::slotDataReceived()
{
    assert(0);
}

void Communicator::slotDisconnected()
{
    assert(0);
}

bool Communicator::Connect()
{
    connectToHost(*_reconHost, _port);

    /*QString message = "Hello!";

    if (tcpSocket->write(message.toLatin1(), message.length()) != message.length())
    {
        return;
    }*/

    return  true;
}

bool Communicator::Disconnect()
{
    disconnectFromHost();
    return true;
}
