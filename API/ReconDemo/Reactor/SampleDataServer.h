#ifndef SAMPLE_DATA_SERVER_H_
#define SAMPLE_DATA_SERVER_H_
#include <thread>
#include "MessagePack/datapackage.h"
#include "QDataEvent.h"

///
/// \brief The DataManager class
/// Receiving data and storing data, while notifying the corresponding user.
///

class SampleDataObserver
{
public:
    virtual ~SampleDataObserver(){}
    virtual void OnDataBegin(uint32_t scan_id) = 0;
    virtual void OnDataData(uint32_t scan_id, DimensionType dimension_type) = 0;
    virtual void OnDataEnd(uint32_t scan_id) = 0;
};


class SampleDataServer
{
public:
    static SampleDataServer& GetHandle();
    ~SampleDataServer();

    bool OnDataServer(DataPackage &package, int cmd_id);
    void SetObserver(std::shared_ptr<SampleDataObserver> observer);
    bool Run();

private:
    SampleDataServer(void);
    void ThreadFunction();
    void ExitThread();

    bool OnDataStart(SampleDataStart &start);//cmr::MsgUnpack& msg_pack, TransferInfo& info);
    bool OnDataData(SampleDataData &data);//cmr::MsgUnpack& msg_pack, TransferInfo& info);
    bool OnDataEnd(SampleDataEnd &end);//cmr::MsgUnpack& msg_pack, TransferInfo& info);
    void NotifyObserver(std::shared_ptr<SampleDataObserver> observer, bool result, std::string = "");

    void calculate_dimindex(SampleDataStart &start, int dim23456, int &phase_index, int &slice_index);

    static SampleDataServer s_instance;
    std::shared_ptr<SampleDataObserver> _observer;
    std::shared_ptr<std::thread> _thread;
    SampleDataStart _sample_start;
    //std::mutex _state_mutex;

};


#endif
