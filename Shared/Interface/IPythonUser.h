#pragma once
#ifndef _IPythonUser_h__
#define _IPythonUser_h__

#include "IPython.h"

namespace Yap {

	struct IPythonUser
	{
		virtual void SetPython(IPython& python) = 0;
	};
}

#endif // !_IPythonUser_h__

