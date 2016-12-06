#pragma once

#ifndef SamplingMaskCreator_h__20160814
#define SamplingMaskCreator_h__20160814

#include "Interface/Implement/ProcessorImpl.h"
#include <vector>

namespace Yap
{
	class SamplingMaskCreator :
		public ProcessorImpl
	{
	public:
		SamplingMaskCreator(void);
		SamplingMaskCreator(const SamplingMaskCreator& rhs);

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;


	private:
		unsigned int _try_count;
		unsigned int _tolerance;

		~SamplingMaskCreator();

		std::vector<unsigned char> GetRandomSamplingPattern(unsigned int row_count,
			float pow, float sample_percent, float radius);
		std::vector<unsigned char> GetEqualSamplingPattern(unsigned int height, unsigned int acs, unsigned int rate);

		// Pdf 数据类型有待补充。
		std::vector<float> GeneratePdf(unsigned int row_count, float p, float sample_percent, float radius);

		std::vector<float> LineSpace(float begin, float end, unsigned int count);

		std::vector<unsigned char> GenerateRandomMask(unsigned int width, unsigned int height, float pow, float sample_percent, float radius);
		std::vector<unsigned char> GenerateEqualMask(unsigned int width, unsigned int height, unsigned int acs, unsigned int rate);
	};
}

#endif // SamplingMaskCreator_h__


