#pragma once

#ifndef ChannelMerger_h__20160813
#define ChannelMerger_h__20160813

#include "Implement/processorimpl.h"
#include "Client/DataHelper.h"

namespace Yap
{
	class ChannelMerger :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ChannelMerger)
	public:
		ChannelMerger(void);
		ChannelMerger(const ChannelMerger& rhs);

	protected:
		~ChannelMerger(void);

		virtual bool Input(const wchar_t * name, IData * data) override;

		struct MergeBuffer
		{
			SmartPtr<FloatData> buffer;
			unsigned int count;

			MergeBuffer() : count(0){}
			MergeBuffer(MergeBuffer& rhs) : count(rhs.count), buffer(rhs.buffer) {}
			MergeBuffer(MergeBuffer&& rhs) : count(rhs.count), buffer(rhs.buffer) {} 

			virtual ~MergeBuffer() {}
		};
		bool HasChannelDimension(IDimensions *dimensions) const;
		std::vector<unsigned int> GetKey(IDimensions * dimensions);
		std::map<std::vector<unsigned int>, MergeBuffer> _merge_buffers; 

	};
}

#endif // ChannelMerger_h__

