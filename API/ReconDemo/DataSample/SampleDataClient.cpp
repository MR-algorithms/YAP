#include "StdAfx.h"
#include "SampleDataClient.h"
#include "MsgPackBufferSocketIO.h"
#include "Utility\CmrLogger.h"
#include "FormatString.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "CmrPack.h"
#include "Sequence\ETCPFidFile.h"
#include "SampleDataProcotol.h"
#include "CmrTimer.h"
#include <algorithm>

using namespace boost::property_tree;
using namespace cmr;

RPSampleData::RPSampleData()
    : _rp(0), _rec(0){}

int RPSampleData::GetRP() const
{
    return _rp;
}

void RPSampleData::SetRP(int rp)
{
    _rp = rp;
}

std::vector<int> RPSampleData::GetRec() const
{
    std::vector<int> recs;
    for(auto iter = _data.begin(); iter != _data.end(); ++iter)
    {
        recs.push_back(iter->first);
    }
    std::sort(recs.begin(), recs.end());
    return recs;
}

void RPSampleData::AddData(int rec, const std::complex<float>* data, int size)
{
    if(size > 0)
    {
        boost::shared_array<std::complex<float>> b(new std::complex<float>[size]);
        std::copy_n(data, size, b.get());
        _data[rec].push_back(ComplexArray(b, size));
    }
}

int RPSampleData::GetDataSize(int rec) const
{
    auto iter = _data.find(rec);
    if(iter == _data.end())
    {
        return 0;
    }
    int size = 0;
    for(auto i = 0; i < iter->second.size(); ++i)
    {
        size += (iter->second)[i].second;
    }
    return size;
}

bool RPSampleData::GetData(int rec, std::complex<float>* data, int size)
{
    auto iter = _data.find(rec);
    if(iter == _data.end())
    {
        return false;
    }

    if(size < GetDataSize(rec))
    {
        return false;
    }
    auto dst = data;
    auto& complex_array = iter->second;
    for(size_t i = 0; i < complex_array.size(); ++i)
    {
        std::copy_n(complex_array[i].first.get(), complex_array[i].second, dst);
        dst += complex_array[i].second;
    }
    return true;
}

std::vector<RPSampleData::ComplexArray> RPSampleData::GetData(int rec)
{
    auto iter = _data.find(rec);
    if(iter == _data.end())
    {
        return std::vector<RPSampleData::ComplexArray>();
    }
    return iter->second;
}

#define MAX_CHANNEL 32

SampleDataClient::SampleDataClient(void)
	: _lastest_transfer_error_code(0)
	, _lastest_finished(true)
	, _socket(INVALID_SOCKET)
	, _session_valid(false)
	, _server_port(0)
	, _use_config(true)
    , _last_rp(-1)
{
	_channel_data.resize(MAX_CHANNEL);
	for (int i = 0; i < MAX_CHANNEL; ++i)
	{
		_channel_data[i] = std::shared_ptr<ETCPFidBin>(new ETCPFidBin);
	}
    _observer_channel.store(0);
}

SampleDataClient::~SampleDataClient(void)
{
}

bool SampleDataClient::SetServerInfo( const std::string& ip, uint16_t port, const std::wstring& config_file, bool used_config )
{
	boost::unique_lock<boost::mutex> lk(_state_mutex);
	_server_ip = ip;
	_server_port = port;
	_config_file = config_file;
	_use_config = used_config;
	return true;
}

uint32_t SampleDataClient::GetVersion() const
{
	return 1 << 16;//1.0
}

bool SampleDataClient::FileSessionStart( uint32_t scan_id, const std::wstring& folder, uint16_t session_id)
{
	boost::unique_lock<boost::mutex> lk(_state_mutex);
	ScanInfo info;
	info.scan_id = scan_id;
	info.folder = folder;
	_session_scan_map[session_id] = info; 
	
	if (_socket == INVALID_SOCKET)
	{
		if (_use_config)
		{
			GetIpPort(_server_ip, _server_port);
		}
		if (Connect(_server_ip.c_str(), _server_port))
		{
			_socket_cv.notify_all();
		}
		else
		{
            LOG_ERROR(FormatStringW(L"Try Connect Serve %s:%d failed.", _server_ip.c_str(), int(_server_port)).c_str());
		}
	}

	return true;
}

bool SampleDataClient::GetFileSessionState(bool& is_this_id, uint32_t& state, uint32_t& error_code, uint32_t scan_id)
{
	boost::unique_lock<boost::mutex> lk(_state_mutex);
	state = _lastest_finished ? 0 : 1;
	is_this_id = (_latest_scan_info.scan_id == scan_id);
	error_code = _lastest_transfer_error_code;
	return true;
}

bool SampleDataClient::FileSessionEnd( uint16_t session_id )
{
	boost::unique_lock<boost::mutex> lk(_state_mutex);
	_session_scan_map.erase(session_id);
    return true;
}

void SampleDataClient::SetObserverChannel(uint32_t channel_mask)
{
    _observer_channel.exchange(channel_mask);
}

void SampleDataClient::SetObserver(std::shared_ptr<SampleDataObserver> observer)
{
    _observer = observer;
}

bool SampleDataClient::Connect(const char* ip, int port)
{
	if (_socket != INVALID_SOCKET)
	{
		return true;
	}

	WSADATA wsd;
	int result_code = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (result_code != 0)
	{
		result_code = WSAGetLastError();
        LOG_ERROR(FormatStringW(L"Client Startup Error, error code %d.", result_code).c_str());
		return false;
	}
	if (LOBYTE(wsd.wVersion) != 2 || HIBYTE(wsd.wVersion) != 2)
	{
        LOG_ERROR(FormatStringW(L"WSAVersion Error, Hi %d, LO %d.", HIBYTE(wsd.wVersion), LOBYTE(wsd.wVersion)).c_str());
		WSACleanup();
		return false;
	}

	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
	{
        LOG_ERROR(L"Create socket error.");
		return false;
	}
	sockaddr_in sever_addr;
	sever_addr.sin_family = AF_INET;
	sever_addr.sin_port = htons(port);
	sever_addr.sin_addr.s_addr = inet_addr(ip);

	if (connect(_socket, reinterpret_cast<sockaddr*>(&sever_addr), sizeof(sever_addr)) == SOCKET_ERROR)
	{
        LOG_ERROR(FormatStringW(L"Client connect error, error code %d, ip %s port %d.", WSAGetLastError(), ip, port).c_str());
		shutdown(_socket, SD_SEND);
		closesocket(_socket);
		_socket = INVALID_SOCKET;
		WSACleanup(); 
		return false;
	}
	return true;
}

struct TransferInfo 
{
	bool transfering;
	int data_count;
	int data_line;
	uint64_t channel_mask;
    CCmrTimer watch;
};

void SampleDataClient::ThreadFunction()
{
	while(true)
	{
		RunSampleDataClientSession();
	}
}

void SampleDataClient::RunSampleDataClientSession()
{
	cmr::MsgPackBufferSocketIO io;
	cmr::MsgPackBuffer msg_bufferr;
	std::vector<std::complex<float>> data;

	{
		boost::unique_lock<boost::mutex> lk(_state_mutex);
		if (_socket == INVALID_SOCKET)
		{
			if (_use_config)
			{
				GetIpPort(_server_ip, _server_port);
			}
			if (!Connect(_server_ip.c_str(), _server_port))
			{
                LOG_ERROR(FormatStringW(L"Try Connect Serve %s:%d failed.", _server_ip.c_str(), int(_server_port)).c_str());
			}
			else
			{
				_socket_cv.notify_all();
			}
		}
	}

	{
		boost::unique_lock<boost::mutex> lk(_state_mutex);
		while (_socket == INVALID_SOCKET)
		{
			_socket_cv.wait(lk);
		}
	}
	TransferInfo info;
	info.data_count = 0;
	info.data_line = 0;
	info.transfering = false;

	auto OnError = [&]()
	{
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
		boost::unique_lock<boost::mutex> lk(_state_mutex);
		_socket = INVALID_SOCKET;
		_lastest_finished = true;
		if (_lastest_transfer_error_code == 0)
		{
			_lastest_transfer_error_code = 100;
		}
	};

	while(true)
	{
		if(false == io.Read(_socket, msg_bufferr))
		{
            LOG_ERROR(L"IO Read error.");
			OnError();
			break;
		}
		uint32_t cmd_id;
		cmr::MsgUnpack unpack(&msg_bufferr);
		if(mpeSuccess != unpack.Convert(0, cmd_id))
		{
            LOG_ERROR(L"Convert Header Error.");
			OnError();
			break;
		}

		if(SAMPLE_DATA_START == cmd_id)
		{
			if(!OnDataStart(unpack, info))
			{
                LOG_ERROR(L"OnDataStart Error.");
				OnError();
				break;
			}
		}
		else if(SAMPLE_DATA_DATA == cmd_id)
		{
			if (!OnDataData(unpack, info))
			{
                LOG_ERROR(L"OnDataData Error.");
				OnError();
				break;
			}
		}
		else if(SAMPLE_DATA_END == cmd_id)
		{
			if(!OnDataEnd(unpack, info))
			{
                LOG_ERROR(L"OnDataEnd Error.");
				OnError();
				break;
			}
		}
		else
		{
            LOG_ERROR(L"Unknown Msgr.");
			OnError();
			break;
		}
	}
}

void SampleDataClient::GetIpPort(std::string& ip, unsigned short& port)
{
	try
	{
		ptree pt;
		read_xml("..\\config\\FileServerConfig.xml", pt);
		ip = pt.get<std::string>("SpecIP", "192.168.0.172");
		port = pt.get<unsigned short>("SpecDataPort", 1000);
	}
	catch (...)
	{
        LOG_ERROR(L"FileServerConfig Error.");
		ip = "192.168.0.172";
		port = 1000;
	}
    LOG_INFO(FormatStringW(L"Use IP %s:%d.", ip.c_str(), int(port)).c_str());
}

bool SampleDataClient::OnDataData(cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
	if (!info.transfering)
	{
		return false;
	}
	SampleDataDataRef cmd_data;
	if(!cmd_data.Unpack(msg_pack))
	{
		info.transfering = false;
        LOG_ERROR(L"Convert Fid Error.");
		return false;
	}
	info.data_count += cmd_data.data_size;
	int channel = cmd_data.rec;
	if (channel >= MAX_CHANNEL || channel < 0)
	{
		info.transfering = false;
        LOG_ERROR(FormatStringW(L"DataData Channel Index Error %d.", channel).c_str());
		return false;
	}
	//CmrLogInfo(FormatString("Data Ch %d Idx %d", channel, cmd_data.dim23456_index));
	int line_index = cmd_data.dim23456_index;
	std::pair<std::complex<float>*, int> buffer = _channel_data[channel]->GetFid(line_index);
	if (buffer.first == nullptr)
	{
		info.transfering = false;
        LOG_ERROR(FormatStringW(L"DataData Line Index Data Cursor Empty %d.", line_index).c_str());
		return false;
	}

	int length = cmd_data.data_size;
	if (buffer.second < length)
	{
		info.transfering = false;
        LOG_ERROR(FormatStringW(L"DataData Line %d Data Size error, size %d.", line_index, length).c_str());
		return false;
	}

    if(cmd_data.rp_id != _last_rp)
    {
        _current_observer_channel_mask = _observer_channel.load();
        _last_rp = cmd_data.rp_id;

        if(_observer && _observer_data)
        {
            _observer->OnData(_latest_scan_info.scan_id, _observer_data.release());
        }
        _observer_data.reset();
    }
    if((_current_observer_channel_mask & (1 << cmd_data.rec)) != 0)
    {
        if(!_observer_data)
        {
            _observer_data.reset(new RPSampleData);
            _observer_data->SetRP(_last_rp);
        }
        _observer_data->AddData(static_cast<int>(cmd_data.rec), cmd_data.data_ptr, cmd_data.data_size);
    }
	float coeff = cmd_data.coeff;
	std::complex<float>* cursor = buffer.first;
	for (int i = 0; i < length; ++i)
	{
		cursor[i] = cursor[i] + cmd_data.data_ptr[i] * coeff; 
	}
	++info.data_line;
	return true;
}

bool SampleDataClient::OnDataEnd(cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    _last_rp = -1;
	if (!info.transfering)
	{
		return false;
	}
	SampleDataEnd cmd_end(0);
	if (!cmd_end.Unpack(msg_pack))
	{
        LOG_ERROR(L"Convert CmdEnd Error.");
		return false;
	}

	info.transfering = false;
    LOG_INFO(FormatStringW(L"DataTransfer Use Time %d ms, Total Data %d, Total Line %d.", int(info.watch.Now()), info.data_count, info.data_line).c_str());
    if(_observer_data)
    {
        if(_observer)
        {
            _observer->OnData(_latest_scan_info.scan_id, _observer_data.release());
        }
        _observer_data.reset();
    }
	std::wstring folder;
	{
		boost::unique_lock<boost::mutex> lk(_state_mutex);
		if (_latest_scan_info.folder.empty())
		{
			for (auto iter = _session_scan_map.begin(); iter != _session_scan_map.end(); ++iter)
			{
				if (iter->second.scan_id == _latest_scan_info.scan_id)
				{
					_latest_scan_info.folder = iter->second.folder;
					break;
				}
			}
		}
		folder = _latest_scan_info.folder;
	}

    std::wstring first_path;

	for (int i = 0; i < MAX_CHANNEL; ++i)
	{
		if (((uint64_t(1) << i) & info.channel_mask) != 0)
		{
			std::wstring path = FormatStringW(L"ChannelData%02d.fid", i + 1);
			if (!folder.empty())
			{
				path = folder + L"\\" + path;
			}
            if(first_path.empty())
            {
                first_path = path;
            }
			ETCPFidFile fid_file(_channel_data[i]);
			if (0 != fid_file.SaveLastData(path))
			{
                LOG_ERROR(FormatStringW(L"Save File Error, Ch %d", i).c_str());
				ClearChannelBuffer();
				return false;
			}
		}
	}

    if(_observer)
    {
        _observer->OnDataEnd(_latest_scan_info.scan_id, first_path);
    }
	{
		boost::unique_lock<boost::mutex> lk(_state_mutex);
		_lastest_finished = true;
		_lastest_transfer_error_code = cmd_end.error_code;
	}

	ClearChannelBuffer();
	return true;
}

inline int BitCount2(uint64_t n)
{
	unsigned int c =0 ;
	for (c = 0; n; ++c)
	{
		n &= (n -1) ; // 清除最低位的1
	}
	return c ;
}

bool SampleDataClient::OnDataStart(cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    LOG_INFO(L"DataStart.");
    _last_rp = -1;
    _current_observer_channel_mask = 0;
    _observer_data.reset();
	if (info.transfering)
	{
        LOG_ERROR(L"Recv DataStart when last not finished.");
	}
	info.data_count = 0;
	info.data_line = 0;
	info.transfering = false;
	info.channel_mask = 0;

	SampleDataStart cmd_start;
	if (!cmd_start.Unpack(msg_pack))
	{
        LOG_ERROR(L"DataStart Cmd Error.");
		return false;
	}

	{
		boost::unique_lock<boost::mutex> lk(_state_mutex);
		_lastest_finished = false;
		_lastest_transfer_error_code = 0;
		_latest_scan_info.scan_id = cmd_start.scan_id;
		_latest_scan_info.folder.clear();
		for (auto iter = _session_scan_map.begin(); iter != _session_scan_map.end(); ++iter)
		{
			if (iter->second.scan_id == cmd_start.scan_id)
			{
				_latest_scan_info.folder = iter->second.folder;
				break;
			}
		}
	}

	if(cmd_start.dim23456_size != cmd_start.dim2_size * cmd_start.dim3_size * cmd_start.dim4_size * cmd_start.dim5_size * cmd_start.dim6_size)
	{
        LOG_ERROR(L"DataStart Size Error.");
		return false;
	}

	if (cmd_start.dim6_size != 1)
	{
        LOG_ERROR(FormatStringW(L"Data Size Dim6 error %d", cmd_start.dim6_size).c_str());
        return false;
	}

	uint64_t channel_mask = cmd_start.channel_mask;
	info.channel_mask = cmd_start.channel_mask;
	ClearChannelBuffer();
	int channel_count = BitCount2(channel_mask);
	if (channel_count == 0)
	{
        LOG_ERROR(L"Error ChannelCount 0.");
		return false;
	}
    LOG_INFO(FormatStringW(L"DimSize %d*%d*%d*%d*%d*%d, Ch %d",
        cmd_start.dim1_size, cmd_start.dim2_size, cmd_start.dim3_size, cmd_start.dim4_size, cmd_start.dim5_size, cmd_start.dim6_size, channel_count).c_str());
	for (int i = 0; i < MAX_CHANNEL; ++i)
	{
		if (((uint64_t(1) << i) & channel_mask) != 0)
		{
			if(!_channel_data[i]->Initialize(cmd_start.dim1_size, cmd_start.dim2_size, cmd_start.dim3_size, cmd_start.dim4_size, cmd_start.dim5_size))
			{

				ClearChannelBuffer();
				return false;
			}
		}
	}

    if(_observer)
    {
        _observer->OnDataBegin(_latest_scan_info.scan_id);
    }
	info.data_count = 0;
	info.data_line = 0;
	info.transfering = true;
	info.watch.Start();
	return true;
}

void SampleDataClient::ClearChannelBuffer()
{
	for (size_t i = 0; i < _channel_data.size(); ++i)
	{
		_channel_data[i]->Clear();
	}
}
