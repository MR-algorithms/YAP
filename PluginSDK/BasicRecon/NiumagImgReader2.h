#pragma once
#ifndef NiumagImgReader2_h__
#define NiumagImgReader2_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	/// Class used to read img data file created by Niumag.
	/**
	Feel nullptr to the "Input" port to trigger file reading.
	"Output" data will be of type Unsigned Int.
	*/
	class NiumagImgReader2 :
		public ProcessorImpl
	{
	public:
		NiumagImgReader2(void);
		NiumagImgReader2(const NiumagImgReader2& rhs);
	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;

		bool ReadNiumagImgData2();
	};
}


#endif // NiumagImgReader2_h__