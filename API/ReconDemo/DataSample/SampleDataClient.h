#ifndef SAMPLE_DATA_CLIENT_20171016_h_
#define SAMPLE_DATA_CLIENT_20171016_h_

#include <winsock2.h>
#include <stdint.h>
#include <string>
#include <boost\thread.hpp>
#include <complex>
#include <boost/shared_array.hpp>
#include "SpecControlGlobal.h"
#include <atomic>

struct TransferInfo;
class ETCPFidBin;
namespace cmr
{
	class MsgUnpack;
	class MsgPack;
};

typedef std::pair<boost::shared_array<std::complex<float>>, int> ComplexArray;

class SPEC_CONTROL_DECLARE RPSampleData
{
public:
    RPSampleData();
    int GetRP() const;
    void SetRP(int rp);
    std::vector<int> GetRec() const;
    void AddData(int rec, const std::complex<float>* data, int size);
    int GetDataSize(int rec) const;
    bool GetData(int rec, std::complex<float>* data, int size);
    std::vector<ComplexArray> GetData(int rec);
private:
    int _rec;
    int _rp;
    typedef std::pair<boost::shared_array<std::complex<float>>, int> ComplexArray;
    std::map<int, std::vector<ComplexArray>> _data;
};

class SPEC_CONTROL_DECLARE SampleDataObserver
{
public:
    virtual ~SampleDataObserver(){}
    virtual void OnDataBegin(uint32_t scan_id) = 0;
    virtual void OnData(uint32_t scan_id, RPSampleData* data) = 0;
    virtual void OnDataEnd(uint32_t scan_id, const std::wstring& path) = 0;
};

class SPEC_CONTROL_DECLARE SampleDataClient
{
public:
	SampleDataClient(void);
	~SampleDataClient(void);

	bool SetServerInfo(const std::string& ip, uint16_t port, const std::wstring& config_file, bool used_config);
    void SetObserver(std::shared_ptr<SampleDataObserver> observer);
    void SetObserverChannel(uint32_t channel_switch);

    uint32_t GetVersion() const;

    bool FileSessionStart(uint32_t scan_id, const std::wstring& folder, uint16_t session_id);
    //state 0 finished, 1 receive
    bool GetFileSessionState(bool& is_this_id, uint32_t& state, uint32_t& error_code, uint32_t scan_id);
    bool FileSessionEnd(uint16_t session_id);

    void ThreadFunction();

private:
	boost::mutex _state_mutex;
	boost::condition_variable _socket_cv;
	SOCKET _socket;
    std::atomic<uint32_t> _observer_channel;
    uint32_t _current_observer_channel_mask;
    std::shared_ptr<SampleDataObserver> _observer;

	std::string _server_ip;
	uint16_t _server_port;
	std::wstring _config_file;
	bool _use_config;

    std::unique_ptr<RPSampleData> _observer_data;
    uint32_t _last_rp;

	struct ScanInfo
	{
		uint32_t scan_id;
		std::wstring folder;
	};
	std::map<uint16_t, ScanInfo> _session_scan_map;//每一个session只维护最后一个scan_id

	ScanInfo _latest_scan_info;
	uint32_t _lastest_transfer_error_code;
	bool _lastest_finished;
	bool _session_valid;//最近传输的数据是否有客户连接

	bool Connect(const char* ip, int port);

	void RunSampleDataClientSession();

	void GetIpPort(std::string& ip, unsigned short& port);

	std::vector<std::shared_ptr<ETCPFidBin>> _channel_data;
	bool OnDataStart(cmr::MsgUnpack& msg_pack, TransferInfo& info);
	bool OnDataData(cmr::MsgUnpack& msg_pack, TransferInfo& info);
	bool OnDataEnd(cmr::MsgUnpack& msg_pack, TransferInfo& info);

	void ClearChannelBuffer();
};

#endif
