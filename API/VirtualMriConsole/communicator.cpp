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
    qDebug()<<"Communicator::slotConnected().";
}

void Communicator::slotDataReceived()
{
    qDebug()<<"Communicator::slotDataReceived().";
}

void Communicator::slotDisconnected()
{
    qDebug()<<"Communicator::slotDisconnected().";
}

bool Communicator::Connect()
{

    abort();//取消已有的连接

    connectToHost(*_reconHost, _port);

    return  waitForConnected();
}

bool Communicator::Disconnect()
{
    qDebug()<<"Communicator::Disconnect().";
    disconnectFromHost();
    //return true;
    return waitForDisconnected();
}

bool Communicator::Send(const DataPackage &package)
{


    QByteArray byteArray1 =
            QByteArray::fromRawData((char*)(&package.magic_anditem_count[0]), sizeof(uint32_t));


    //
    QByteArray byteArray1b =
            QByteArray::fromRawData((char*)(&package.magic_anditem_count[1]), sizeof(uint32_t));

    QByteArray byteArray1c =
            QByteArray::fromRawData((char*)package.headitems.data(), sizeof(HeadItem) * package.headitems.size());



    QByteArray byteArray1cc;
    for(int i = 0; i < static_cast<int>( package.headitems.size() ); i ++)
    {
         HeadItem headitem = package.headitems[i];
         QByteArray temp = QByteArray::fromRawData((char*)(&headitem), sizeof(HeadItem));
         byteArray1cc += temp;
    }

    assert(byteArray1c == byteArray1cc);
    //
    QByteArray byteArray2;
    for(int i = 0; i < static_cast<int>( package.valueitems.size() ); i ++)
    {
         ValueItem valueitem = package.valueitems[i];

         QByteArray temp = QByteArray::fromRawData((char*)valueitem.value.data(), valueitem.value.size());
         byteArray2 += temp;
    }

    int write_return = this->write(byteArray1 + byteArray1b + byteArray1c + byteArray2);
    if( write_return == -1)
    {
        qDebug()<<"Communicator::Send -- calling write function failed!";
        return false;
    }
    else
    {
        int bytesA = package.BytesFromValueitems();
        int bytesB = package.BytesFromHeaditems();
        int bytesC = byteArray2.size();
        assert( bytesA == bytesB && bytesA == bytesC);
        int bytesTotal = QByteArray(byteArray1 + byteArray1b + byteArray1c + byteArray2).size();
        assert(write_return == bytesTotal);
        qDebug()<<"Communicator::Send--calling write sending "<< bytesTotal <<" bytes";

        bool success = this->waitForBytesWritten(30000);
        assert(success);

        return true;
    }

}
