#pragma once
#ifndef NiumagImgReader_h__
#define NiumagImgReader_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	/// Class used to read img data file created by Niumag.
	/**
	Feel nullptr to the "Input" port to trigger file reading.
	"Output" data will be of type Unsigned Int.
	*/
	class NiumagImgReader :
		public ProcessorImpl
	{
	public:
		NiumagImgReader(void);
		NiumagImgReader(const NiumagImgReader& rhs);
	private:
		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data);

		bool ReadNiumagImgData();
	};
}


#endif // NiumagImgReader_h__