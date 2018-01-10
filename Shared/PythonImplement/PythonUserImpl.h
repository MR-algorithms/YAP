#pragma once

#ifndef PythonUserImpl_H_20171020
#define PythonUserImpl_H_20171020

#include "Interface\IPythonUser.h"
#include <memory>

#define PYTHON_SET_REF_DATA(data, data_type, input_dimensions, input_size) \
	PythonUserImpl::GetInstance().SetReferenceData(data, data_type, input_dimensions, input_size)

#define PYTHON_PROCESS(module, method, data_type, input_dimensions, data, output_dimensions, input_size, output_size, is_need_ref_data) \
	PythonUserImpl::GetInstance().PythonProcess(module, method, data_type, input_dimensions, data, output_dimensions, input_size, output_size, is_need_ref_data)
#define PYTHON_DELETE_REF_DATA() PythonUserImpl::GetInstance().DeleteRefData()

namespace Yap
{
	class PythonUserImpl : public IPythonUser {

	public:
		~PythonUserImpl();

		static PythonUserImpl& GetInstance();
		
		virtual void SetPython(IPython& python) override;
		
		void SetReferenceData(void * data, int data_type, int input_dimensions, size_t * input_size);

		void * PythonProcess(const wchar_t* module, const wchar_t* method, int data_type,
			size_t input_dimensions, void * data, OUT size_t& output_dimensions, size_t input_size[], OUT size_t output_size[], bool is_need_ref_data);

		void DeleteRefData();

	private:

		PythonUserImpl();
		static std::shared_ptr<PythonUserImpl> s_instance;
		IPython* _python;

	};


}
#endif
