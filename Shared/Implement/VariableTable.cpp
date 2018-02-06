#include "VariableTable.h"
#include "ContainerImpl.h"

#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "Utilities/macros.h"

using namespace Yap;
using namespace std;


namespace Yap
{

VariableTable::VariableTable() :
	_variables(YapShared(new PtrContainerImpl<IVariable>))
{
}

VariableTable::VariableTable(IVariableContainer * variables) :
	_variables(YapShared(variables))
{
}

VariableTable::VariableTable(const VariableTable& rhs) :
	_variables{ YapShared(rhs.Variables()->Clone()) }
{
}

VariableTable::VariableTable(VariableTable&& rhs) :
	_variables{rhs._variables}
{
	rhs._variables.reset();
}

VariableTable::~VariableTable()
{
}

const VariableTable& VariableTable::operator = (const VariableTable& rhs)
{
	_variables = YapShared(rhs.Variables()->Clone());

	return *this;
}

const VariableTable& VariableTable::operator = (VariableTable&& rhs)
{
	_variables = rhs._variables;
	rhs._variables.reset();

	return *this;
}

bool VariableTable::Add(IVariable* variable)
{
	assert(variable != nullptr);
	return _variables->Add(variable->GetId(), variable);
}

IVariableContainer* VariableTable::Variables()
{
	return _variables.get();
}

const IVariableContainer* VariableTable::Variables() const
{
	return _variables.get();
}

IVariable * VariableTable::GetVariable(IVariableContainer * variables,
	const wchar_t * id,
	int type)
{
	assert(variables != nullptr);
	assert(id != nullptr);

	wstring variable_id{ id };
	auto dot_pos = variable_id.find_first_of(L'.');
	if (dot_pos != wstring::npos)
	{
		IVariable * lhs_variable = nullptr;

		auto left_square_pos = variable_id.find_first_of(L'[');
		if (left_square_pos != wstring::npos && left_square_pos < dot_pos)
		{
			auto right_square_pos = variable_id.find_first_of(L']', left_square_pos);
			if (right_square_pos == wstring::npos || right_square_pos > dot_pos)
				throw VariableException(id, VariableException::InvalidId);

			auto variable = GetVariable(variables, variable_id.substr(0, left_square_pos).c_str(), VariableStructArray);
			if (variable == nullptr)
				throw VariableException(id, VariableException::VariableNotFound);

			auto index = _wtoi(variable_id.substr(left_square_pos + 1, right_square_pos - left_square_pos - 1).c_str());

			auto array_variable = dynamic_cast<IValueArray<SmartPtr<IVariable>>*>(variable);
			assert(array_variable != nullptr);

			if (index < 0 || index >= int(array_variable->GetSize()))
				throw VariableException(id, VariableException::OutOfRange);

			auto element_reference = dynamic_cast<IElementReference<SmartPtr<IVariable>>*>(variable);
			assert(element_reference != nullptr);

			lhs_variable = element_reference->Element(index).get();
		}
		else
		{
			auto lhs = variable_id.substr(0, dot_pos); // left hand side of the dot operator.
			lhs_variable = variables->Find(lhs.c_str());
		}
		if (!lhs_variable)
			throw VariableException(id, VariableException::VariableNotFound);

		if (lhs_variable->GetType() != VariableStruct)
			throw VariableException(id, VariableException::NotAStruct);

		auto struct_variable = dynamic_cast<IStructVariable*>(lhs_variable);
		assert(struct_variable != nullptr);

		auto members = struct_variable->Members();
		assert(members != nullptr);

		return GetVariable(members, variable_id.substr(dot_pos + 1).c_str(), type);
	}
	else
	{
		auto variable = variables->Find(id);
		//BUG('How to handle variable is nullptr?');
		//if (variable == nullptr)
		//		return nullptr; // throw VariableException(id, VariableException::VariableNotFound);

		if (type != VariableAllTypes && ((variable->GetType() | type) != type))
			throw VariableException(id, VariableException::TypeNotMatch);

		return variable;
	}
}

IVariable * VariableTable::GetVariable(const wchar_t * id, int expected_type)
{
	assert(id != nullptr && id[0] != 0);
	return GetVariable(_variables.get(), id, expected_type);
}

void VariableTable::Reset()
{
	_variables = YapShared(new PtrContainerImpl<IVariable>);
}

bool VariableTable::VariableExists(const wchar_t *variable_id) const
{
	auto This = const_cast<VariableTable*>(this);
	return This->GetVariable(variable_id) != nullptr;
}

bool VariableTable::ResizeArray(const wchar_t * id, size_t size)
{
	auto array = GetVariable(id, VariableBoolArray | VariableFloatArray | VariableIntArray | VariableStructArray);
	if (array == nullptr)
		return false;

	if (!(array->GetType() == VariableBoolArray || array->GetType() == VariableIntArray ||
		array->GetType() == VariableFloatArray || array->GetType() == VariableStructArray))
	{
		throw VariableException(id, VariableException::NotAnArray);
	}

	auto array_base = dynamic_cast<IArrayBase*>(array);
	assert(array_base != nullptr);
	array_base->SetSize(size);

	return true;
}

size_t VariableTable::GetArraySize(const wchar_t * id)
{
	auto variable = GetVariable(id);
	if (variable == nullptr)
		return -1;

	auto array = dynamic_cast<IArrayBase*>(variable);
	if (array == nullptr)
		return -1;

	return array->GetSize();
}

}	// end Yap
