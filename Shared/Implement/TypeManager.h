#pragma once
#ifndef TypeManager_h__20180129
#define TypeManager_h__20180129

#include <map>
#include <string.h>
#include "interface/smartptr.h"
#include "interface/IVariable.h"

namespace Yap
{
	class TypeManager
	{
	public:
		TypeManager();
		~TypeManager();

		void Reset();
		bool AddType(const wchar_t * type_id, IVariable * type);
		bool AddType(const wchar_t * type_id, IVariableContainer * variables);
		bool TypeExists(const wchar_t * type) const;
		IVariable * CreateInstance(const wchar_t * type, const wchar_t * id) const;
		IVariable * CreateArray(const wchar_t * element_type_id, const wchar_t * id, size_t size);
	protected:
		bool InitTypes();

		std::map<std::wstring, SmartPtr<IVariable>> _types;
	};
}

#endif // TypeManager_h__20180129

