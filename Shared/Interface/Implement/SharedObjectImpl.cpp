#include "SharedObjectImpl.h"

Yap::SharedObjectImpl::SharedObjectImpl() :
	_use_count(0)
{
}

Yap::SharedObjectImpl::~SharedObjectImpl()
{
}

void Yap::SharedObjectImpl::Lock()
{
	++_use_count;
}

void Yap::SharedObjectImpl::Release()
{
	if (_use_count == 0 || --_use_count == 0)
	{
		delete this;
	}
}
