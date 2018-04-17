#include "reconclientsocket.h"
#include "SampleDataProtocol.h"
#include "datamanager.h"
#include <QDebug>
#include <cassert>

ReconClientSocket::ReconClientSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &QTcpSocket::readyRead, this, &ReconClientSocket::slotDataReceived);
    connect(this, &QTcpSocket::disconnected, this, &ReconClientSocket::slotDisconnected);
}

void ReconClientSocket:: slotDataReceived()
{
    int lengthx = bytesAvailable();

    emit signalDataReceived(lengthx);

    qDebug()<< "Enter ReconClientSocket:: slotDataReceived():  "<< lengthx<<" bytes available";

    while (true)
    {

        if( !Read(_bufferInfo.Next) )//长度不够。

        {
            return;
        }
        else
        {
            _bufferInfo.Next = static_cast<ReadinfoType>(
                        static_cast<int>(_bufferInfo.Next) + 1 );
            if(_bufferInfo.Next == ReadinfoType::rtFinished)
            {
                //process the package.
                DataManager::GetHandle().ReceiveData(this->_package, this->_bufferInfo.cmd_id);
                //
                _bufferInfo.Reset();

            }

        }



    }


}
bool ReconClientSocket::Read(ReadinfoType rt)
{
    switch(rt)
    {
        case rtFlag:
        {
            return ReadFlag();
        }
        case rtHeadItem:
        {
            return ReadHeaditem();
        }

        case rtValue:
        {
            return ReadValue();
        }
        default:
        {
           // assert(0);
            return false;
        }
    }

}

bool ReconClientSocket::ReadFlag()
{
    QByteArray byteArray;

    int bytesLeft = bytesAvailable();

    qDebug()<< "Enter ReconClientSocket::ReadFlag():  "<< bytesLeft<<" bytes available";
    if( bytesLeft < 2 * sizeof(uint32_t) )
    {
        return false;
    }
    else
    {
        byteArray = read(2 * sizeof(uint32_t));

        uint32_t first =  *(uint32_t*) (char*)byteArray.data();
        uint32_t second = *(uint32_t*)((char*)byteArray.data() + sizeof(uint32_t));

        _package.magic_anditem_count[0] = first;
        _package.magic_anditem_count[1] = second;
        _bufferInfo.headitem_count = second;
        if(first == 0xFFFFFFFF &&
                (second == 11 ||second == 6 || second == 2 ) )
        {
            return true;
        }
        else
        {
            //assert(0);
            return false;
        }

    }

}
bool ReconClientSocket::ReadHeaditem()
{
    QByteArray byteArray;

    int headitem_count = _bufferInfo.headitem_count;
    int headitem_bytes = _bufferInfo.headitem_count * sizeof(HeadItem);

    int bytesLeft = bytesAvailable();
    qDebug()<< "Enter ReconClientSocket::ReadHeaditem():  "<< bytesLeft<<" bytes available";

    if( bytesLeft < headitem_bytes)
    {
        return false;
    }
    else
    {
        byteArray = read(headitem_bytes);

        assert(byteArray.size() == headitem_bytes);

        _package.head.resize( headitem_count);

        memcpy(_package.head.data(), byteArray.data(), byteArray.size());

        _package.CheckSelf(true, false, false);



    }

    return true;
}

bool ReconClientSocket::ReadValue()
{

    int bytesToRead = _package.BytesFromHeaditem();

    int bytesLeft = bytesAvailable();
    qDebug()<< "Enter ReconClientSocket::ReadValue():  "<< bytesLeft<<" bytes available";

    if(bytesLeft < bytesToRead)
    {
        return false;
    }

    _package.data.resize( _package.head.size() );
    for(int i = 0; i < static_cast<int>( _package.head.size() ); i ++)
    {
        int dataitem_bytes = _package.head[i].size;
        QByteArray byteArray = read( dataitem_bytes );
        assert(byteArray.size() == dataitem_bytes);

        _package.data[i].data.resize(dataitem_bytes);
        memcpy( _package.data[i].data.data(), byteArray.data(), dataitem_bytes );//有问题。

    }

    assert(_package.data[0].data.size() == sizeof(uint32_t));
    memcpy( &_bufferInfo.cmd_id, _package.data[0].data.data(), sizeof(uint32_t));

    _package.CheckSelf();
    return true;
}


void ReconClientSocket::slotDisconnected()
{
    qDebug()<<"ReconClientSocket: Recieving disconnected message.";
    emit signalDisconnected(this->socketDescriptor());

}
