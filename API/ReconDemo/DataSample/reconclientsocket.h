#ifndef RECONCLINETSOCKET_H
#define RECONCLINETSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QObject>
#include "SampleDataProtocol.h"
#include "datapackage.h"
enum ReadinfoType
{
    rtFlag = 0,
    rtHeadItem,
    rtValue,
    rtFinished
};

struct BufferInfo
{
  ReadinfoType Next;
  int headitem_count;
  int cmd_id;
  int datapoint_count;
  BufferInfo(): Next(rtFlag), headitem_count(0), cmd_id(0), datapoint_count(0){}
  void Reset(){
      Next = rtFlag;
      headitem_count = 0;
      cmd_id = 0;
      datapoint_count = 0;
  }


};

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
private:
    //SampleDataStart _startStruct;
    //SampleDataData  _dataStruct;
    //SampleDataEnd   _endStruct;

    DataPackage _package;
    BufferInfo _bufferInfo;

    bool Read(ReadinfoType rt);
    bool ReadFlag();
    bool ReadHeaditem();
    bool ReadValue();

    //-------




};

#endif // RECONCLINETSOCKET_H
