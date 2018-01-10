#ifndef _IPython_h__
#define _IPython_h__
#pragma once

#ifndef OUT
#define OUT
#endif

namespace Yap {

	struct IPython
	{
		virtual void* Process(const wchar_t* module_name, const wchar_t* method_name, int data_type,
			size_t input_dimensions, void * data, OUT size_t &output_dimensions, size_t input_size[],
			OUT size_t output_size[], bool is_need_ref_data = 0) = 0;

		virtual void SetRefData(void * roi_data, int data_type, size_t dimension_count, size_t size[]) = 0;

		virtual void DeleteRefData() = 0;
		virtual ~IPython() = 0 {};
	};

};

#endif