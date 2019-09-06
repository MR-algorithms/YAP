#ifndef CmrMsgBufferReadWrite_h__
#define CmrMsgBufferReadWrite_h__

#include <WinSock2.h>
#include <stdint.h>
#include "MsgPackBuffer.h"
//socket必须是阻塞模式
namespace cmr
{
	class MsgPackBufferSocketIO
	{
	public:
		MsgPackBufferSocketIO();

		bool Write(SOCKET socket, const cmr::MsgPackBuffer&);
		bool Read(SOCKET socket, cmr::MsgPackBuffer&);

	private:
		bool MultiSend(SOCKET socket, std::vector<WSABUF>& buf);
		bool MultiRead(SOCKET socket, std::vector<WSABUF>& buf);

	private:
		struct HeadItem
		{
			uint16_t primary_type;
			uint16_t second_type;
			uint32_t size;
		};

		struct BodyItem
		{
			const char* address;
			uint32_t size;
		};
		std::vector<WSABUF> _wsabuf_cache;
		std::vector<HeadItem> _head_cache;
		std::vector<BodyItem> _body_cache;
		int _total_read;
		int _total_send;
	};
};
#endif // CmrMsgBufferReadWrite_h__
