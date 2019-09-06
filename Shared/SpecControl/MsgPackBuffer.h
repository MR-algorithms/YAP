#ifndef MsgPackBuffer_h__
#define MsgPackBuffer_h__

#include "CmrPackType.h"
#include <memory>
#include <vector>
#include <stdint.h>
#include <boost/shared_array.hpp>
#include <stdlib.h>

#define DEFAULT_CHUNK_SIZE 1024

namespace cmr
{
	class MsgPackBufferSocketIO;

	class MsgPackBuffer
	{
	public:
		MsgPackBuffer(void);
		~MsgPackBuffer(void);

		int GetItemCount() const;

		void Reset();
		char* AddItem(uint16_t data_type, uint16_t second_type, char* data, int size);
		char* AddRefItem(uint16_t data_type, char* data, int size);
		char* AddUniqueBuffer(uint16_t data_type, std::unique_ptr<char[]>& unique_buffer, int size);

		bool GetItem(int idx, uint16_t& data_type, uint16_t& second_type, char*& data, int& size) const;
		bool GetUniqueBufferOwnership(int idx, uint16_t& data_type, std::unique_ptr<char[]>& unique_buffer, int& size);
	private:
		MsgPackBuffer(const MsgPackBuffer&);
		MsgPackBuffer& operator=(const MsgPackBuffer&);
		char* ChunkAllocate(int size);

		struct MsgPackItem
		{
			MsgPackItem();
			MsgPackItem(uint16_t data_type, uint16_t second_type, char* data, int size);
			MsgPackItem(uint16_t data_type, uint16_t second_type, std::unique_ptr<char[]>& unique_buffer, int size);
			MsgPackItem(MsgPackItem&& right);
			MsgPackItem& operator=(MsgPackItem&& right);

			uint16_t data_type;
			uint16_t second_type;
			char* data;// used for Value, Array RefArray
			std::unique_ptr<char[]> unique_buffer;// used for Unique Ptr
			int size;

		private:
			MsgPackItem& operator=(const MsgPackItem& right); // forbid
			MsgPackItem(const MsgPackItem& right); // forbid
		};

		struct MsgPackBufferChunk
		{
			MsgPackBufferChunk(): capacity(0), used(0){}

			boost::shared_array<char> ptr;
			int capacity;
			int used;
		};

		std::vector<MsgPackItem> _items;
		std::vector<MsgPackBufferChunk> _chunks;

		static const int align_size = 8;
	public:
		friend class MsgPackBufferSocketIO;
	};
}


#endif // MsgPackBuffer_h__
