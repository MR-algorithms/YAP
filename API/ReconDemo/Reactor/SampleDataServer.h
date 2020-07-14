#ifndef SAMPLE_DATA_SERVER_H_
#define SAMPLE_DATA_SERVER_H_
#include <thread>
#include "MessagePack/datapackage.h"

///
/// \brief The SampleDataServer class
/// SampleDataServer is simplified for just parsing the protocol data.

class SampleDataServer
{
public:
    static SampleDataServer& GetHandle();
    ~SampleDataServer();

    bool OnDataParsing(DataPackage &package, int cmd_id);
    //bool Run();

private:
    SampleDataServer(void);
    //void ThreadFunction();
    //void ExitThread();

    bool OnDataStart(SampleDataStart &start);//cmr::MsgUnpack& msg_pack, TransferInfo& info);
    bool OnDataData(SampleDataData &data);//cmr::MsgUnpack& msg_pack, TransferInfo& info);
    bool OnDataEnd(SampleDataEnd &end);//cmr::MsgUnpack& msg_pack, TransferInfo& info);

    void calculate_dimindex(SampleDataStart &start, int dim23456, int &phase_index, int &slice_index);

    static SampleDataServer s_instance;
    //std::shared_ptr<std::thread> _thread;
    SampleDataStart _sample_start;
    //std::mutex _state_mutex;

};


#endif
