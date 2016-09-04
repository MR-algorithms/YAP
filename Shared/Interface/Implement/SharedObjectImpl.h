#pragma once

#ifndef SharedObjectImpl_h_20160831
#define SharedObjectImpl_h_20160831

#include "../../../Shared/Interface/IMemory.h"

namespace Yap
{
	class SharedObjectImpl : public ISharedObject
	{
	public:
		SharedObjectImpl();
	private:
		// Inherited via ISharedObject
		virtual void Lock() override;
		virtual void Release() override;

		unsigned int _use_count;
	};
}

#endif