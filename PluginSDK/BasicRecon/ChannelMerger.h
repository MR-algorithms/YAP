#pragma once

#ifndef ChannelMerger_h__20160813
#define ChannelMerger_h__20160813

#include "Interface/Implement/processorimpl.h"
#include "Interface/Client/DataHelper.h"

namespace Yap
{
	class ChannelMerger :
		public ProcessorImpl
	{
	public:
		ChannelMerger(void);
		ChannelMerger(const ChannelMerger& rhs);

		virtual IProcessor* Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;
	
	protected:
		~ChannelMerger(void);

		struct MergeBuffer
		{
			SmartPtr<FloatData> buffer;
			unsigned int count;

			MergeBuffer() : count(0){}
			MergeBuffer(MergeBuffer& rhs) : count(rhs.count), buffer(rhs.buffer) {}
			MergeBuffer(MergeBuffer&& rhs) : count(rhs.count), buffer(rhs.buffer) {} 

			virtual ~MergeBuffer() {}
		};

		std::vector<unsigned int> GetKey(IDimensions * dimensions);
		std::map<std::vector<unsigned int>, MergeBuffer> _merge_buffers; 

	};
}

#endif // ChannelMerger_h__

