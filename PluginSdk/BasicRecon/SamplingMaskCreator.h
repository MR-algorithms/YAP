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
		virtual ~SamplingMaskCreator();

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		unsigned int _try_count;
		unsigned int _tolerance;

		std::vector<unsigned char> GetMinInterferenceSamplingPattern(unsigned int row_count,
			float pow, float sample_percent, float radius);

		// Pdf 数据类型有待补充。
		std::vector<float> GeneratePdf(unsigned int row_count, float p, float sample_percent, float radius);

		std::vector<float> LineSpace(float begin, float end, unsigned int count);


		// Inherited via ProcessorImpl
		virtual bool OnInit() override;

	};
}
#endif // SamplingMaskCreator_h__


