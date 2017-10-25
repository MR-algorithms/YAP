#include "PythonUserImpl.h"
#include "Utilities\macros.h"

using namespace Yap;

std::shared_ptr<PythonUserImpl> PythonUserImpl::s_instance;

Yap::PythonUserImpl::~PythonUserImpl()
{
	TODO("~PythonUserImpl func")
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
	return (!_python) ? nullptr : _python;
}

Yap::PythonUserImpl::PythonUserImpl()
{
	TODO("PythonUserImpl constructor")
}
