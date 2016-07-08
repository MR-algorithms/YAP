#pragma once
#include "processorimp.h"
#include "DataHelper.h"
#include "..\Interface\SmartPtr.h"

namespace Yap
{
	class CChannelMerger :
		public CProcessorImp
	{
	public:
		CChannelMerger(void);
		CChannelMerger(const CChannelMerger& rhs);
		virtual ~CChannelMerger(void);
		virtual IProcessor* Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;
	protected:
		struct MergeBuffer
		{
			CSmartPtr<CFloatData> buffer;
			unsigned int count;

			MergeBuffer() : count(0){}
			MergeBuffer(MergeBuffer& rhs) : count(rhs.count), buffer(rhs.buffer) {}
			MergeBuffer(MergeBuffer&& rhs) : count(rhs.count), buffer(rhs.buffer) {} 

			~MergeBuffer() {}
		};

		std::vector<unsigned int> GetKey(IDimensions * dimensions);
		std::map<std::vector<unsigned int>, MergeBuffer> _merge_buffers; 
	};
}

