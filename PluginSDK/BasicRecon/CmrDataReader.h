#pragma once

#ifndef CmrDataReader_h__20160813
#define CmrDataReader_h__20160813

#include "Implement/processorImpl.h"

namespace Yap
{
	/// Class used to read raw data file created by CMR.
	/**
		\remarks Properties:
		DataPath: specifies the path of the folder containing raw data file.
		ChannelCount: specifies the number of the total channels.
		ChannelSwitch: specifies which channels are used.
		GroupCount: specifies how many groups are used in the scan.

		Feel nullptr to the "Input" port to trigger file reading.
		"Output" data will be of type ComplexFloat.
	*/
	class CmrDataReader :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(CmrDataReader)
	public:
		CmrDataReader(void);
		CmrDataReader(const CmrDataReader& rhs);

	private:
		~CmrDataReader();

		virtual bool Input(const wchar_t * name, IData * data) override;

		bool ReadRawData(unsigned int channel_index);
		float* ReadEcnuFile(const wchar_t * file_path, unsigned int& width, unsigned int& height,
			unsigned int& slices, unsigned int& dim4);
	};
}


#endif // CmrDataReader_h__
