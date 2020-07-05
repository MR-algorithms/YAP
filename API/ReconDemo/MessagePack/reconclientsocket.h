#ifndef RECONCLINETSOCKET_H
#define RECONCLINETSOCKET_H

#include "SampleDataProtocol.h"
#include "datapackage.h"
#include "WinSock2.h"

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

class ReconClientSocket
{
public:
    ReconClientSocket(SOCKET socket);
    void DataReceived();
private:
    bool Read(ReadinfoType rt);
    bool ReadFlag();
    bool ReadHeaditem();
    bool ReadValue();

    DataPackage _package;
    BufferInfo _bufferInfo;
    SOCKET _socket;

};

#endif // RECONCLINETSOCKET_H
