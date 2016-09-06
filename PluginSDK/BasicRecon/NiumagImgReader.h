#pragma once
#ifndef NiumagImgReader_h__
#define NiumagImgReader_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	/// Class used to read img data file created by CMR.
	/**
	Feel nullptr to the "Input" port to trigger file reading.
	"Output" data will be of type Unsigned Int.
	*/
	class CNiumagImgReader :
		public ProcessorImpl
	{
	public:
		CNiumagImgReader(void);
		CNiumagImgReader(const CNiumagImgReader& rhs);
	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
		
		bool ReadNiumagImgData();
	};
}


#endif // NiumagImgReader_h__