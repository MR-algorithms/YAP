#pragma once

#ifndef SamplingTypeGenerator_h__20160814
#define SamplingTypeGenerator_h__20160814

#include "../../Shared/Interface/Implement/ProcessorImpl.h"
#include <vector>

namespace Yap
{
	class CSamplingTypeGenerator :
		public ProcessorImpl
	{
	public:
		CSamplingTypeGenerator(void);
		CSamplingTypeGenerator(const CSamplingTypeGenerator& rhs);
		virtual ~CSamplingTypeGenerator();

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

	};
}
#endif // SamplingTypeGenerator_h__


