#pragma once
#ifndef _IPythonUser_h__
#define _IPythonUser_h__

#include "IPython.h"

namespace Yap {

	struct IPythonUser
	{
		virtual void SetPython(IPython& python) = 0;

		virtual void SetReferenceData(void * data, int data_type, int input_dimensions, size_t * input_size) = 0;

		virtual void * PythonProcess(const wchar_t* module, const wchar_t* method, int data_type,
						size_t input_dimensions, void * data, OUT size_t& output_dimensions, 
						size_t input_size[], OUT size_t output_size[], bool is_need_ref_data) = 0;

		virtual void DeleteRefData() = 0;
	};
}

#endif // !_IPythonUser_h__

