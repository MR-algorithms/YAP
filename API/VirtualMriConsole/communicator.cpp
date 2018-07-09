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
        qDebug()<<"Server ip address error";
        return false;
    }

    _port = port;
    return true;
}
void Communicator::slotConnected()
{
    qDebug()<<"Communicator: slotConnected.";
}

void Communicator::slotDataReceived()
{
    qDebug()<<"Communicator: slotDataReceived.";
}

void Communicator::slotDisconnected()
{
    qDebug()<<"Communicator: slotDisconnected.";
}

bool Communicator::Connect()
{

    abort();//取消已有的连接

    connectToHost(*_reconHost, _port);

    return  waitForConnected();
}

bool Communicator::Disconnect()
{
    disconnectFromHost();
    //return true;
    return waitForDisconnected();
}

bool Communicator::Send(const DataPackage &data)
{


    QByteArray byteArray1 =
            QByteArray::fromRawData((char*)(&data.magic_anditem_count[0]), sizeof(uint32_t));


    //
    QByteArray byteArray1b =
            QByteArray::fromRawData((char*)(&data.magic_anditem_count[1]), sizeof(uint32_t));

    QByteArray byteArray1c =
            QByteArray::fromRawData((char*)data.head.data(), sizeof(HeadItem) * data.head.size());


    //或者
    QByteArray byteArray1cc;
    for(int i = 0; i < static_cast<int>( data.head.size() ); i ++)
    {
         HeadItem headitem = data.head[i];
         QByteArray temp = QByteArray::fromRawData((char*)(&headitem), sizeof(HeadItem));
         byteArray1cc += temp;
    }

    assert(byteArray1c == byteArray1cc);
    //
    QByteArray byteArray2;
    for(int i = 0; i < static_cast<int>( data.data.size() ); i ++)
    {
         DataItem dataitem = data.data[i];
         //每一个item的成员又是vector。
         QByteArray temp = QByteArray::fromRawData((char*)dataitem.data.data(), dataitem.data.size());
         byteArray2 += temp;
    }


    this->write(byteArray1 + byteArray1b + byteArray1c + byteArray2);

    int bytesA = data.BytesFromDataitem();
    int bytesB = data.BytesFromHeaditem();
    int bytesC = byteArray2.size();
    assert( bytesA == bytesB && bytesA == bytesC);

    int bytesTotal = QByteArray(byteArray1 + byteArray1b + byteArray1c + byteArray2).size();
    qDebug()<<"Send "<< bytesTotal <<" bytes";

    bool succeed = this->waitForBytesWritten();
    return succeed;
}
