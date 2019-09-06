#pragma once

#ifndef ZeroFilling_h__20160814
#define ZeroFilling_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class ZeroFilling :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ZeroFilling)
	public:
		ZeroFilling();
		ZeroFilling(const ZeroFilling& rhs);

	protected:
		~ZeroFilling();

		virtual bool Input(const wchar_t * port, IData * data) override;
		unsigned int GetFillingCount(IData* data, int dest_width, int dest_height, int dest_depth);
		void UpdateStartLength(const int input_length, int &dest_start, int &dest_length, bool same = false);
		
	};
}
#endif // ZeroFilling_h__
