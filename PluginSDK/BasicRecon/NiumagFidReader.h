#pragma once
#ifndef NiumagFidReader_h__
#define NiumagFidReader_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	/// Class used to read fid data file created by Niumag.
	/**
	Feel nullptr to the "Input" port to trigger file reading.
	"Output" data will be of type Unsigned Int.
	*/
	class NiumagFidReader :
		public ProcessorImpl
	{
	public:
		NiumagFidReader(void);
		NiumagFidReader(const NiumagFidReader& rhs);

	private:
		~NiumagFidReader();

		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		bool ReadNiumagFidData();
	};
}

#endif // NiumagFidReader_h__