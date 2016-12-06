#pragma once
#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	struct ParametterSet
	{
		unsigned int max_line_search_times;
		float gradient_tollerance;
		unsigned int max_conjugate_gradient_iteration_times;
		unsigned int typenorm;
		float tv_weight;
		float wavelet_weight;
		float line_search_alpha;
		float line_search_beta;
		float initial_line_search_step;
	};

	class CompressedSensing :
		public ProcessorImpl
	{
	public:
		CompressedSensing();

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		~CompressedSensing();

		SmartPtr<IData> _mask;

	};
}

