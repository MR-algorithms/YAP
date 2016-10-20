#pragma once
#ifndef NiuMriImageReader_h__
#define NiuMriImageReader_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	/// Class used to read .niuimg data file created by Niumag.
	/**
	Feel nullptr to the "Input" port to trigger file reading.
	"Output" data will be of type Unsigned Int.
	*/
	class NiuMriImageReader :
		public ProcessorImpl
	{
	public:
		NiuMriImageReader(void);
		NiuMriImageReader(const NiuMriImageReader& rhs);
	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;

		bool ReadNiuMriImageData();
	};
}


#endif // NiuMriImageReader_h__