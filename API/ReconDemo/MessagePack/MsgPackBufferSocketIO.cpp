#include "MsgPackBufferSocketIO.h"
#include <array>
#include <iostream>
#include "QtUtilities/FormatString.h"
#include <QDebug>
#include <cassert>


//#include "Utility/CmrLogger.h"


using namespace cmr;

//This is for compatibility with QTcpSocket used in ReconClientSocket.cpp
int cmr::MsgPackBufferSocketIO::qt_bytesAvailable(SOCKET socket)
{
    unsigned long uBytes = 0;
    int nReturn = ioctlsocket(socket, FIONREAD, (unsigned long*)&uBytes);
    if(nReturn == SOCKET_ERROR)
    {
        //Failed to get the length of the receiving data.
    }
    //reinterpret_cast<int>(this)
    DebugInfo::Output(L"MsgPackBufferSocketIO::bytesAvailable()", L"Enter...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type1);


    return uBytes;
}

QByteArray cmr::MsgPackBufferSocketIO::qt_read(SOCKET socket, int length)
{

    std::vector<char> temp;
    temp.resize(length);

    std::vector<WSABUF> wsabuf;
    wsabuf.resize(1);
    wsabuf[0].buf = (char*)(temp.data());
    wsabuf[0].len = static_cast<ULONG>(length);

    if (false == MultiRead(socket, wsabuf))
    {
        assert(0);
        return QByteArray();
    }

    //
    QByteArray result;
    result.resize(length);
    memcpy(result.data(), wsabuf[0].buf, length);

    DebugInfo::Output(L"MsgPackBufferSocketIO::qt_read()", L"Enter...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type1);
    qDebug()<< "Enter MsgPackBufferSocketIO::qt_read():  read "<< length<<" bytes";


    return result;

}

bool cmr::MsgPackBufferSocketIO::Write(SOCKET socket)
{
    return true;
}
bool cmr::MsgPackBufferSocketIO::Read(SOCKET socket)
{
    return true;
}
/*
bool cmr::MsgPackBufferSocketIO::Write(SOCKET socket, const cmr::MsgPackBuffer& pack_buffer)
{
	try
	{
		_wsabuf_cache.clear();

		// 数量标志
        std::array<uint32_t, 2> magic_anditem_count = {0xFFFFFFFF, static_cast<uint32_t>(pack_buffer._items.size())};
		WSABUF wsabuf;
		wsabuf.len = 8;
		wsabuf.buf = (char*)(magic_anditem_count.data());
		_wsabuf_cache.push_back(wsabuf);

		// Item数据头
		_head_cache.resize(pack_buffer._items.size());
		wsabuf.buf = (char*)(_head_cache.data());
        wsabuf.len = static_cast<ULONG>(_head_cache.size() * sizeof(HeadItem));
		_wsabuf_cache.push_back(wsabuf);

		HeadItem* dst = _head_cache.data();
		const MsgPackBuffer::MsgPackItem* src = pack_buffer._items.data();

		// Item数据体，同时填充Item数据头内容
		for(size_t i = 0; i < _head_cache.size(); ++i, ++dst, ++src)
		{
			dst->primary_type = src->data_type;
			dst->second_type = src->second_type;
			if (dst->second_type == mstRefArray)
			{
				dst->second_type = mstArray;
			}
			dst->size = src->size;

			if (dst->size > 0)
			{
				wsabuf.len = src->size;
				if (src->second_type == mstUniquePtr)
				{
					wsabuf.buf = src->unique_buffer.get();
				}
				else
				{
					wsabuf.buf = src->data;
				}
				_wsabuf_cache.push_back(wsabuf);
			}
		}

		if(false == MultiSend(socket, _wsabuf_cache))
		{
			LOG_ERROR(L"MultiSend Error.");
			return false;
		}
		return true;
	}
	catch (std::bad_alloc&)
	{
		LOG_ERROR(L"Write Memory  Error.");
		return false;
	}
}

bool cmr::MsgPackBufferSocketIO::Read(SOCKET socket, cmr::MsgPackBuffer& pack_buffer)
{
	try
	{
		pack_buffer.Reset();

		// 读取数量标志
		std::array<uint32_t, 2> magic_and_count;
		_wsabuf_cache.resize(1);
		_wsabuf_cache[0].buf = (char*)(magic_and_count.data());
		_wsabuf_cache[0].len = 8;
		if (false == MultiRead(socket, _wsabuf_cache))
		{
			return false;
		}

		if (magic_and_count[0] != 0xFFFFFFFF)
		{
			return false;
		}

		uint32_t item_count = magic_and_count[1];
		if (item_count == 0)
		{
			return true;
		}

		// 读取Item数据头
		_head_cache.resize(item_count);
		_wsabuf_cache.resize(1);
		_wsabuf_cache[0].buf = (char*)(_head_cache.data());
        _wsabuf_cache[0].len = static_cast<ULONG>(_head_cache.size() * sizeof(HeadItem));

		if (false == MultiRead(socket, _wsabuf_cache))
		{
			return false;
		}

		// 分配内存，读取Item数据体, 注意空数据的处理
		_wsabuf_cache.clear();
		_wsabuf_cache.reserve(_head_cache.size());
		for (size_t i = 0; i < _head_cache.size(); ++i)
		{
			if (_head_cache[i].second_type == mstUniquePtr)
			{
				std::unique_ptr<char[]> ptr(new char[_head_cache[i].size]);
				MsgPackBuffer::MsgPackItem item(_head_cache[i].primary_type, _head_cache[i].second_type, ptr, _head_cache[i].size);
				if (item.size > 0)
				{
					WSABUF wsa_buf;
					wsa_buf.buf = item.unique_buffer.get();
					wsa_buf.len = item.size;
					_wsabuf_cache.push_back(wsa_buf);
				}
				pack_buffer._items.push_back(std::move(item));
			}
			else
			{
				char* buffer = pack_buffer.ChunkAllocate(_head_cache[i].size);
				MsgPackBuffer::MsgPackItem item(_head_cache[i].primary_type, _head_cache[i].second_type, buffer, _head_cache[i].size);
				if (item.size > 0)
				{
					WSABUF wsa_buf;
					wsa_buf.buf = item.data;
					wsa_buf.len = item.size;
					_wsabuf_cache.push_back(wsa_buf);
				}
				pack_buffer._items.push_back(std::move(item));
			}
		}

		if (_wsabuf_cache.size() > 0)
		{
			if (false == MultiRead(socket, _wsabuf_cache))
			{
				return false;
			}
		}

		return true;
	}
	catch (std::bad_alloc&)
	{
		return false;		
	}
}
*/

cmr::MsgPackBufferSocketIO::MsgPackBufferSocketIO()
	: _total_read(0)
	, _total_send(0)
{
	_wsabuf_cache.reserve(16);
	_head_cache.reserve(16);
	_body_cache.reserve(16);
}

bool cmr::MsgPackBufferSocketIO::MultiSend(SOCKET socket, std::vector<WSABUF>& buf )
{
	int total_bytes = 0;
	for (size_t i = 0; i < buf.size(); ++i)
	{
		total_bytes += buf[i].len;
	}

	size_t begin_idx = 0;
	size_t count = buf.size();

	int bytes_sended = 0;
	while (bytes_sended < total_bytes)
	{
		DWORD current_bytes_send = 0;
        if(0 != WSASend(socket, buf.data() + begin_idx, static_cast<DWORD>(count - begin_idx), &current_bytes_send, 0, 0, 0))
		{
			int error = WSAGetLastError();
			std::cout << "WSASend Error " << error << "\n";
			return false;
		}
		int accumulate = 0;
		for (size_t idx = begin_idx; idx < count; ++idx)
		{
			if(accumulate + buf[idx].len > current_bytes_send)
			{
				int wsabuf_send = current_bytes_send - accumulate;
				buf[idx].buf += wsabuf_send;
				buf[idx].len -= wsabuf_send;
				begin_idx = idx;
                break;//added by xhb.
            }
            accumulate += buf[idx].len;//added by xhb.
		}

		bytes_sended += current_bytes_send;
	}
	_total_send += bytes_sended;
	return true;
}

bool cmr::MsgPackBufferSocketIO::MultiRead(SOCKET socket,  std::vector<WSABUF>& buf )
{
	int total_bytes = 0;
	for (size_t i = 0; i < buf.size(); ++i)
	{
		total_bytes += buf[i].len;
	}

	size_t begin_idx = 0;
	size_t count = buf.size();

	int bytes_readed = 0;
	while (bytes_readed < total_bytes)
	{
		DWORD current_bytes_read = 0;
		DWORD flag = 0;
        if(0 != WSARecv(socket, buf.data() + begin_idx, static_cast<DWORD>(count - begin_idx), &current_bytes_read, &flag, 0, 0))
		{
//            auto error_code = WSAGetLastError();
			return false;
		}

		if (current_bytes_read == 0)
		{
			//Closed
			return false;
		}
		int remain_bytes = current_bytes_read;
		for (size_t idx = begin_idx; idx < count; ++idx)
		{
			if(static_cast<int>(buf[idx].len) > remain_bytes)
			{
				buf[idx].buf += remain_bytes;
				buf[idx].len -= remain_bytes;
				begin_idx = idx;
				remain_bytes = 0;
				break;
			}
			else
			{
				remain_bytes -= buf[idx].len;
			}
		}

		bytes_readed += current_bytes_read;
	}
	_total_read += bytes_readed;
	return true;
}
