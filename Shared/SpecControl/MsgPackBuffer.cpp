#include "StdAfx.h"
#include "MsgPackBuffer.h"
#include <utility>

using namespace cmr;

MsgPackBuffer::MsgPackBuffer(void)
{
	_items.reserve(16);
	MsgPackBufferChunk chk;
	chk.ptr.reset(new char[DEFAULT_CHUNK_SIZE]);
	chk.capacity = DEFAULT_CHUNK_SIZE;
	chk.used = 0;
	_chunks.push_back(chk);
}

MsgPackBuffer::~MsgPackBuffer(void)
{
}

char* cmr::MsgPackBuffer::AddItem(uint16_t data_type, uint16_t second_type, char* data, int size)
{
	// UniquePtr use AddUniqueBuffer
	if (second_type == mstUniquePtr)
	{
		return nullptr;
	}

	// UniquePtr use AddRefItem
	if (second_type == mstRefArray)
	{
		return nullptr;
	}

	try
	{
		char* buffer = ChunkAllocate(size);
		if (buffer == nullptr)
		{
			return nullptr;
		}

		memcpy(buffer, data, size);
		MsgPackItem item(data_type, second_type, buffer, size);
		_items.push_back(std::move(item));
		return buffer;
	}
	catch(std::bad_alloc&)
	{
		return nullptr;
	}
}

void cmr::MsgPackBuffer::Reset()
{
	_items.clear();
	if (_chunks.size() > 1)
	{
		_chunks.erase(_chunks.begin() + 1, _chunks.end());
		_chunks[0].used = 0;
	}
}

char* cmr::MsgPackBuffer::AddRefItem( uint16_t data_type, char* data, int size )
{
	try
	{
		_items.push_back(MsgPackItem(data_type, mstRefArray, data, size));
		return _items.rbegin()->data;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

char* cmr::MsgPackBuffer::AddUniqueBuffer( uint16_t data_type, std::unique_ptr<char[]>& unique_buffer, int size )
{
	try
	{
		_items.push_back(MsgPackItem(data_type, mstUniquePtr, unique_buffer, size));
		return _items.rbegin()->unique_buffer.get();
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

int cmr::MsgPackBuffer::GetItemCount() const
{
	return static_cast<int>(_items.size());
}

bool cmr::MsgPackBuffer::GetItem( int idx, uint16_t& data_type, uint16_t& second_type, char*& data, int& size ) const
{
	if (idx < static_cast<int>(_items.size()))
	{
		data_type = _items[idx].data_type;
		second_type = _items[idx].second_type;
		if (second_type == mstUniquePtr)
		{
			data = _items[idx].unique_buffer.get();
		}
		else
		{
			data = _items[idx].data;
		}
		size = _items[idx].size;
		return true;
	}
	return false;
}

bool cmr::MsgPackBuffer::GetUniqueBufferOwnership( int idx, uint16_t& data_type, std::unique_ptr<char[]>& unique_buffer, int& size )
{
	if (idx < static_cast<int>(_items.size()))
	{
		data_type = _items[idx].data_type;
		uint16_t second_type = _items[idx].second_type;
		if (second_type == mstUniquePtr)
		{
			unique_buffer.reset(_items[idx].unique_buffer.release());
			size = _items[idx].size;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

struct ChunkDelete
{
	void operator ()(char* p)
	{
		_aligned_free(p);
	}
};

char* cmr::MsgPackBuffer::ChunkAllocate( int size )
{
	int mod = size % align_size;
	int padding_size = (mod == 0) ? size : (size + align_size - mod);
	if (padding_size < align_size)
	{
		padding_size = align_size;// case of size == 0
	}

	for (size_t i = 0; i < _chunks.size(); ++i)
	{
		if (_chunks[i].capacity - _chunks[i].used >= padding_size)
		{
			char* dst = _chunks[i].ptr.get() + _chunks[i].used;
			_chunks[i].used += padding_size;
			return dst;
		}
	}

	int cap_size = padding_size > DEFAULT_CHUNK_SIZE ? padding_size : DEFAULT_CHUNK_SIZE;
	boost::shared_array<char> buffer((char*)_aligned_malloc(cap_size, align_size), ChunkDelete());
	if (buffer.get() == nullptr)
	{
		return nullptr;
	}

	MsgPackBufferChunk chk;
	chk.ptr = buffer;
	chk.capacity = cap_size;
	chk.used = padding_size;

	try
	{
		_chunks.push_back(chk);
		return chk.ptr.get();
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

cmr::MsgPackBuffer::MsgPackItem::MsgPackItem()
	: data_type(mptNil)
	, second_type(mstValue)
	, data(nullptr)
	, unique_buffer(nullptr)
	, size(0)
{
}

cmr::MsgPackBuffer::MsgPackItem::MsgPackItem(uint16_t type1, uint16_t type2, char* dst, int s)
{
	data_type = type1;
	second_type = type2;
	data = dst;
	size = s;
}

cmr::MsgPackBuffer::MsgPackItem::MsgPackItem( uint16_t type1, uint16_t type2, std::unique_ptr<char[]>& unique_ptr, int s)
{
	data_type = type1;
	second_type = type2;
	unique_buffer = std::move(unique_ptr);
	data = nullptr;
	size = s;
}

cmr::MsgPackBuffer::MsgPackItem::MsgPackItem( MsgPackItem&& right )
{
	std::swap(data_type, right.data_type);
	std::swap(second_type, right.second_type);
	std::swap(data, right.data);
	std::swap(size, right.size);
	std::swap(unique_buffer, right.unique_buffer);
}

cmr::MsgPackBuffer::MsgPackItem& cmr::MsgPackBuffer::MsgPackItem::operator=( MsgPackItem&& right )
{
	std::swap(data_type, right.data_type);
	std::swap(second_type, right.second_type);
	std::swap(data, right.data);
	std::swap(size, right.size);
	std::swap(unique_buffer, right.unique_buffer);
	return *this;
}
