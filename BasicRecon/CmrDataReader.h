#pragma once
#include "processorImp.h"

namespace Yap
{
	class CCmrDataReader :
		public CProcessorImp
	{
	public:
		CCmrDataReader(void);
		CCmrDataReader(const CCmrDataReader& rhs);
	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;  

		bool ReadRawData(unsigned int channel_index);
		float* ReadEcnuFile(const wchar_t * file_path, unsigned int& width, unsigned int& height,
			unsigned int& slices, unsigned int& dim4);
	};
}


