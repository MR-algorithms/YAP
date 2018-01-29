#include "TypeManager.h"
#include "Interface/smartptr.h"
#include "details/simpleVariable.h"
#include "details/arrayVariable.h"
#include "details/structVariable.h"
#include <vcruntime_exception.h>

namespace Yap
{
	using namespace _details;

	TypeManager::TypeManager()
	{
		InitTypes();
	}


	TypeManager::~TypeManager()
	{
	}

	bool TypeManager::InitTypes()
	{
		_types.emplace(L"int", YapShared(new SimpleVariable<int>(L"int", nullptr)));
		_types.emplace(L"float", YapShared(new SimpleVariable<double>(L"float", nullptr)));
		_types.emplace(L"string", YapShared(new SimpleVariable<const wchar_t * const>(L"string", nullptr)));
		_types.emplace(L"bool", YapShared(new SimpleVariable<bool>(L"bool", nullptr)));
		
		_types.emplace(L"array<int>", YapShared(new RawArray<int>(1, 0, L"array<int>")));
		_types.emplace(L"array<float>", YapShared(new RawArray<double>(1, 0.0, L"array<float>", nullptr)));
		_types.emplace(L"array<bool>", YapShared(new ValueArray<bool>(1, false, L"array<bool>", nullptr)));
		_types.emplace(L"array<string>", YapShared(new ValueArray<const wchar_t * const>(1, L"", L"array<string>", nullptr)));

		return true;
	}

	void TypeManager::Reset()
	{
		_types.clear();

		InitTypes();
	}

	bool TypeManager::AddType(const wchar_t * type_id, IVariable *type)
	{
		assert(_types.find(type_id) == _types.end());
		assert(type != nullptr);
		assert(type_id != nullptr);

		if (_types.find(type_id) != _types.end())
			return false;

		_types.insert({ type_id, YapShared(type) });

		return true;
	}

	bool TypeManager::AddType(const wchar_t * type_id, 
		IVariableContainer * variables)
	{
		try
		{
			auto variable = new StructVariable(variables, type_id, nullptr);
			return AddType(type_id, variable);
		}
		catch (std::bad_alloc&)
		{
			return false;
		}
	}

	bool TypeManager::TypeExists(const wchar_t * type) const
	{
		return _types.find(type) != _types.end();
	}

	IVariable * TypeManager::CreateInstance(const wchar_t * type) const
	{
		auto iter = _types.find(type);
		return (iter != _types.end()) ? 
			dynamic_cast<IVariable*>(iter->second->Clone()) : nullptr;
	}

}

