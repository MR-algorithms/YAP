#include "PythonUserImpl.h"
#include "Utilities\macros.h"
#include "Implement\LogUserImpl.h"

#include <iostream>
#include <vcruntime_new.h>

using namespace Yap;

std::shared_ptr<PythonUserImpl> PythonUserImpl::s_instance;

Yap::PythonUserImpl::~PythonUserImpl()
{
	
}

Yap::PythonUserImpl& Yap::PythonUserImpl::GetInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<PythonUserImpl>(new (std::nothrow)PythonUserImpl());
	}
	return *s_instance.get();
}

void Yap::PythonUserImpl::SetPython(IPython& python)
{
	_python = &python;
}

void Yap::PythonUserImpl::SetReferenceData(void * data, int data_type, int input_dimensions, size_t * input_size)
{
	_python->SetRefData(data, data_type, input_dimensions, input_size);
}

void * Yap::PythonUserImpl::PythonProcess(
	const wchar_t* module, const wchar_t* method,
	int data_type, size_t input_dimensions,
	void * data, OUT size_t& output_dimensions,
	size_t input_size[], OUT size_t output_size[],
	bool is_need_ref_data)
{
	return _python->Process(module, method, data_type, input_dimensions, data,
		output_dimensions, input_size, output_size, is_need_ref_data);
}

void Yap::PythonUserImpl::DeleteRefData()
{
	_python->DeleteRefData();
}

Yap::PythonUserImpl::PythonUserImpl() : _python{ nullptr }
{
	
}
