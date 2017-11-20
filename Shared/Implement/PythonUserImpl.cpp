#include "PythonUserImpl.h"
#include "Utilities\macros.h"
#include "LogUserImpl.h"
#include <iostream>

using namespace Yap;

std::shared_ptr<PythonUserImpl> PythonUserImpl::s_instance;

Yap::PythonUserImpl::~PythonUserImpl()
{
	LOG_TRACE(L"PythonUserImpl Deconstructor", L"PythonUserImpl");
	std::cout << L"PythonUserImpl destructor" << std::endl;
}

Yap::PythonUserImpl& Yap::PythonUserImpl::GetInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<PythonUserImpl>(new PythonUserImpl());
	}
	return *s_instance;
}

void Yap::PythonUserImpl::SetPython(IPython* python)
{
	_python = python;
}

IPython* Yap::PythonUserImpl::GetPython()
{
	return _python;
}

Yap::PythonUserImpl::PythonUserImpl() : _python(nullptr)
{
	LOG_TRACE(L"PythonUserImpl constructor", L"PythonUserImpl");
}
