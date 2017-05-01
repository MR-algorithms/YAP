#pragma once

#ifndef SamplingMaskCreator_h__20160814
#define SamplingMaskCreator_h__20160814

#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include <vector>
#include <fftw3.h>
#include <complex>

namespace Yap
{
	class SamplingMaskCreator :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(SamplingMaskCreator)
	public:
		SamplingMaskCreator();
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		~SamplingMaskCreator();

		std::vector<unsigned int> GetRandomSamplingPattern(unsigned int row_count,
			float pow, float sample_percent, float radius);
		std::vector<unsigned int> GetEqualSamplingPattern(unsigned int height, unsigned int acs, unsigned int rate);

		// Pdf 数据类型有待补充。
		std::vector<float> GeneratePdf(unsigned int row_count, float p, float sample_percent, float radius);
		std::vector<float> LineSpace(float begin, float end, unsigned int count);
		std::vector<float> GenerateRandomMask(unsigned int width, unsigned int height, float pow, float sample_percent, float radius);
		std::vector<float> GenerateEqualMask(unsigned int width, unsigned int height, unsigned int acs, unsigned int rate);

	private:
		unsigned int _try_count;
		unsigned int _tolerance;
	};
}

#endif // SamplingMaskCreator_h__


