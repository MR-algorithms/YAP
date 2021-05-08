#pragma once
#ifndef RadiaolSampling_h__20160814
#define RadiaolSampling_h__20160814

#include "Implement/ProcessorImpl.h"
#include <boost/shared_array.hpp>
#include <complex>

namespace Yap
{
	class RadialSampling :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(RadialSampling)
	public:
		RadialSampling();
		RadialSampling(const RadialSampling& rhs);

	protected:
		~RadialSampling();

		virtual bool Input(const wchar_t * port, IData * data) override;

		boost::shared_array<std::complex<float>>
			GetRadialLine(std::complex<float> * input_data2D,
				unsigned int width, unsigned int height,
				int columns, float angle, int center, float delta_k);

	private:
		//SmartPtr<IData> _mask;

	};
}

#endif //RadiaolSampling_h__