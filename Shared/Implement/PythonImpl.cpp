#include "PythonImpl.h"
#include "Utilities\macros.h"
#include <Python.h>
#include "LogUserImpl.h"
#include <windows.h>

using namespace std;

std::shared_ptr<Yap::PythonImpl> Yap::PythonImpl::s_instance;

void Yap::PythonImpl::InitPython()
{
	InitializeCriticalSection(&cs);
	EnterCriticalSection(&cs);
	if (_initialzed)
		return;

	Py_Initialize();
	if (!Py_IsInitialized())
	{
		LOG_ERROR(L"Python Interpreter Can not be initialized.", L"PythonUserImpl");
		TODO("is that 'No Throw' used right?")
	}
	_initialzed = true;
	LeaveCriticalSection(&cs);
}

void Yap::PythonImpl::ReleasePython()
{
	EnterCriticalSection(&cs);
	if (!Py_IsInitialized())
		return;
	Py_Finalize();
	_initialzed = false;
	LeaveCriticalSection(&cs);
}

Yap::PythonImpl::~PythonImpl()
{
	DeleteCriticalSection(&cs);
	s_instance = nullptr;
	TODO("~PythonImpl()")
}

Yap::PythonImpl& Yap::PythonImpl::GetInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = std::shared_ptr<PythonImpl>(new PythonImpl());
	}
	return *s_instance;
}

Yap::PythonImpl::PythonImpl():
	_initialzed(false)
{
	TODO("PythonImpl constructor")
}
