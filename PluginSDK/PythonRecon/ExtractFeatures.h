#pragma once
#ifndef ExtractFeatures_h__20171223
#define ExtractFeatures_h__20171223

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class ExtractFeatures :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ExtractFeatures)
	public:

		explicit ExtractFeatures();

		ExtractFeatures(const ExtractFeatures& rhs);

		virtual bool Input(const wchar_t * port, IData * data) override;

	private:
		~ExtractFeatures();

		void CreateDimension(Dimensions& dimensions, size_t output_dims, size_t * output_size);

		bool FeedOut(IData * data, void* out_data, Dimensions dimensions, int data_type, size_t size);

		size_t GetTotalSize(size_t dimension_count, size_t size[]);

		bool SetRefData(IData * data);

		void* PythonRunScript(IData * data, OUT int & output_type, Dimensions& dimensions,
			OUT size_t & output_dims, size_t output_size[]);

		SmartPtr<IData> _ref_data;
	};

}
#endif