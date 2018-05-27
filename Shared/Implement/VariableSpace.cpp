#include "VariableSpace.h"
#include "ContainerImpl.h"
// #include "../API/Yap/VdfParser.h"

#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "details/arrayVariable.h"

using namespace Yap;
using namespace std;


namespace Yap
{
VariableSpace::VariableSpace() :
	_types{new TypeManager}
{
}

VariableSpace::VariableSpace(IVariableContainer * variables) :
	VariableTable{ variables },
	_types{new TypeManager}
{
}

VariableSpace::VariableSpace(const VariableSpace& rhs) :
	VariableTable{rhs},
	_types {rhs._types}
{
}

VariableSpace::VariableSpace(VariableSpace&& rhs) :
	VariableTable{rhs},
	_types{rhs._types}
{
	rhs._variables.reset();
}

VariableSpace::~VariableSpace()
{
}

const VariableSpace& VariableSpace::operator = (const VariableSpace& rhs)
{
	VariableTable::operator = (rhs);
	_types = rhs._types;

	return *this;
}

const VariableSpace& VariableSpace::operator = (VariableSpace&& rhs)
{
	_variables = rhs._variables;
	_types = rhs._types;
	rhs._variables.reset();

	return *this;
}

using namespace _details;

bool VariableSpace::AddVariable(int type, const wchar_t * name, const wchar_t * description)
{
	static map<int, wstring> type_to_string{
		{VariableInt, L"int"},
		{VariableFloat, L"float"},
		{VariableBool, L"bool"},
		{VariableString, L"string"},
	};

	assert(type_to_string.find(type) != type_to_string.end());

	return AddVariable(type_to_string[type].c_str(), name, description);
}

bool VariableSpace::AddVariable(const wchar_t * type,
						const wchar_t * id,
						const wchar_t * description)
{
	auto variable = _types->CreateInstance(type, id);
	if (variable == nullptr)
		return false;

	variable->SetDescription(description);
	return _variables->Add(id, variable);
}

bool VariableSpace::AddArray(const wchar_t * element_type_id, 
	const wchar_t * id, 
	const wchar_t * description)
{
	auto variable = _types->CreateArray(element_type_id, id, 1);
	if (variable == nullptr)
		return false;

	variable->SetDescription(description);
	return _variables->Add(id, variable);
}

/**
  @param id Specify the id of the variable to be enabled or disabled. If it's null,
  then all variables will be enabled or disabled.
  @param enable @a true to enable, @a false to disable.
 */
bool VariableSpace::Enable(const wchar_t * id, bool enable)
{
	if (id != nullptr)
	{
		auto variable = GetVariable(id);

		if (variable == nullptr)
			return false;

		variable->Enable(enable);
	}
	else
	{
		auto iter = _variables->GetIterator();
		for (auto variable = iter->GetFirst(); variable != nullptr; variable = iter->GetNext())
		{
			variable->Enable(enable);
		}
	}

	return true;
}

bool VariableSpace::IsEnabled(const wchar_t * id) const
{
	auto variable = GetVariable(const_cast<VariableSpace*>(this)->_variables.get(), id);
	return (variable != nullptr) ? variable->IsEnabled() : false;
}

//shared_ptr<VariableSpace> VariableSpace::Load(const wchar_t * path)
//{
//	VdfParser parser;
//	return parser.CompileFile(path);
//}

void VariableSpace::Reset()
{
	_variables = YapShared(new PtrContainerImpl<IVariable>);
}

}	// end Yap
