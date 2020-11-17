#pragma once

#ifndef SimpleGrid_h__20160814
#define SimpleGrid_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class SimpleGrid :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(SimpleGrid)
	public:
		SimpleGrid();
		SimpleGrid(const SimpleGrid& rhs);

	protected:
		~SimpleGrid();
		virtual bool Input(const wchar_t * port, IData * data) override;
		void FillKSpace(std::complex<float> * slice, int width, int height,
			std::complex<float> * radial, int columns, float angle, float center, float delta_k);
		
	};
}
#endif // SimpleGrid_h__
