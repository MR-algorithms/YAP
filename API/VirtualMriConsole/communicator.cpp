#include "communicator.h"
#include <QtNetwork/QHostAddress>
#include <QDebug>
#include <cassert>
#include <memory>
#include <QString>
#include "datapackage.h"

Communicator::Communicator(QObject *parent): QTcpSocket(parent)
{
    connect(this, &QTcpSocket::connected,    this, &Communicator::slotConnected);
    connect(this, &QTcpSocket::readyRead,    this, &Communicator::slotDataReceived);
    connect(this, &QTcpSocket::disconnected, this, &Communicator::slotDisconnected);
}

bool Communicator::SetRemoteHost(const wchar_t * ip_address, unsigned short port)
{
    _reconHost = std::make_shared<QHostAddress>();


    std::wstring temp(ip_address);


    QString ipAddress = QString::fromStdWString(temp); // toStdWString

    if (!_reconHost->setAddress(ipAddress))
    {
        qDebug()<<L"Server ip address error";
        return false;
    }

    _port = port;
    return true;
}
void Communicator::slotConnected()
{
    qDebug()<<L"Communicator: slotConnected.";
}

void Communicator::slotDataReceived()
{
    qDebug()<<L"Communicator: slotDataReceived.";
}

void Communicator::slotDisconnected()
{
    qDebug()<<L"Communicator: slotDisconnected.";
}

bool Communicator::Connect()
{
    connectToHost(*_reconHost, _port);


    return  waitForConnected();
}

bool Communicator::Disconnect()
{
    disconnectFromHost();
    return true;
}

bool Communicator::Send(const DataPackage &data)
{
    QByteArray byteArray1 =
            QByteArray::fromRawData((char*)(&data.magic_anditem_count[0]), sizeof(uint32_t));


    QByteArray byteArray2;
    for(int i = 0; i < static_cast<int>( data.data.size() ); i ++)    {
         DataItem dataitem = data.data[i];
         QByteArray temp = QByteArray::fromRawData((char*)dataitem.data.data(), dataitem.data.size());
         byteArray2 += temp;
    }


    this->write(byteArray1 + byteArray2);
    bool succedd = this->waitForBytesWritten();
    return succedd;
}
