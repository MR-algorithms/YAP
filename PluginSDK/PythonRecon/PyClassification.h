#pragma once

#ifndef PyClassification_h_20180319
#define PyClassification_h_20180319

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	
	class PyClassification: public ProcessorImpl
	{
		IMPLEMENT_SHARED(PyClassification)
	public:
		PyClassification();
		~PyClassification();
		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		void* PythonRunScript(IData * data, OUT int & output_type, Dimensions& dimensions,
			OUT size_t & output_dims, size_t output_size[]);

		void CreateDimension(Dimensions & dimensions, size_t output_dims, size_t * output_size);

		bool FeedOut(IData * inut_data, void* out_data, Dimensions dimensions, int data_type, size_t size);

		size_t GetTotalSize(size_t dimension_count, size_t size[]);

	};
}
#endif // !PyClassification_h_20180319
