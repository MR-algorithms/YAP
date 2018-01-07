#include "reconclientsocket.h"
#include "SampleDataProtocol.h"
#include <QDebug>
#include <cassert>

ReconClientSocket::ReconClientSocket(QObject *parent) : QTcpSocket(parent), _endStruct(0)
{
    connect(this, &QTcpSocket::readyRead, this, &ReconClientSocket::slotDataReceived);
    connect(this, &QTcpSocket::disconnected, this, &ReconClientSocket::slotDisconnected);

}

void ReconClientSocket::slotDataReceived()
{
    int x =10;
    int lengthx = bytesAvailable();
    int y = x+ 1;


    qDebug()<< "Enter client::slotRecieved():  "<< lengthx<<" bytes";

   // bool TestDemo = false;

    //这段代码以后需要考虑一次不能完整读到一个结构体的数据；
    //大约这样考虑：
    //每次读取一个完整结构体的数据，如果没有读取完整，break；
    while (bytesAvailable() > 0)
    {
        /*int length = bytesAvailable();
        char buf[1024];
        read(buf, length);
        QString msg = buf;
        int msglen = msg.length();

        emit signalDataReceived(msg, length);
        */

        if(SampleData_TestDemo)
        {
            int length = bytesAvailable();
            QByteArray data = this->readAll();
            //FloatArray test;
            //test.Unpack(data);
            emit signalDataReceived(data,length);
        }
        else
        {
            int length = bytesAvailable();

            uint32_t cmd_id = -1;
            QByteArray dataArray = Read(cmd_id);

            //测试代码，解码检查数据。
            SampleDataStart start;
            switch(cmd_id)
            {
            case SAMPLE_DATA_START:
            {

                start.Unpack(dataArray);

            }
                break;
            case SAMPLE_DATA_DATA:
                break;
            case SAMPLE_DATA_END:
                break;

            }

            //
            int templ = dataArray.length();
            if(dataArray.length() >= 4)
            {
                emit signalDataReceived(dataArray,dataArray.length());

            }


        }

    }
}

QByteArray ReconClientSocket::Read(uint32_t &cmd_id)
{
    uint32_t markHead;//0xFFFFFFFF

    QByteArray data = this->read(sizeof(uint32_t));

    //如果前面有两个干扰字节怎么办？只读到两个字节，怎么办？

    if(data.length() < sizeof(uint32_t))
    {
        return QByteArray(nullptr);
    }

    markHead = *(uint32_t*)(data.data());

    if(markHead != 0xFFFFFFFF)
    {
        return QByteArray(nullptr);

    }
    else
    {
        QByteArray data1 = this->read(sizeof(uint32_t));
        assert(data1.length() == sizeof(uint32_t));
        int cmdidLen = data1.length();
        //已经读取了结构中的第一个uint32_t, 正式读取结构时应减少读取字节数，或回退。
        //如果读取相位编码数据不足，应回退还是暂时保存，下次接着读呢？

        cmd_id = *(uint32_t*)(char*)data1.data();

        switch(cmd_id)
        {

        case SAMPLE_DATA_START:
        {
            QByteArray data = data1 + this->read(sizeof(SampleDataStart) - cmdidLen);

            if(data.length() < sizeof(SampleDataStart))
            {
                return QByteArray(nullptr);
            }
            _startStruct.Unpack(data);
            QByteArray returnArray;
            _startStruct.Pack(returnArray);
            return returnArray;

        }
            break;
        case SAMPLE_DATA_DATA:
        {
            assert(_startStruct.dim1_size > 0);

            int len =    sizeof(uint32_t) * 4
                    +sizeof(float)
                    +sizeof(std::complex<float>) * _startStruct.dim1_size;
            QByteArray data = data1 + this->read(len - cmdidLen);
            assert(data.length() == len);
            _dataStruct.Unpack(data);

            QByteArray returnArray;
            _dataStruct.Pack(returnArray);
            return returnArray;


        }
            break;

        case SAMPLE_DATA_END:
        {
            QByteArray data = data1 + this->read(sizeof(SampleDataEnd) - cmdidLen);
            assert(data.length() == sizeof(SampleDataEnd));

            _endStruct.Unpack(data);

            QByteArray returnArray;
            _endStruct.Pack(returnArray);
            return returnArray;

        }
            break;
        default:
            return QByteArray(nullptr);


        }


    }



}

void ReconClientSocket::slotDisconnected()
{
    qDebug()<<"ClientSocket: Recieving disconnected message.";
    emit signalDisconnected(this->socketDescriptor());

}
