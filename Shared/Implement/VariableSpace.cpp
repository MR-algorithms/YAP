#include "VariableSpace.h"
#include "Interface/Interfaces.h"
#include "ContainerImpl.h"
#include "../API/Yap/VdfParser.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Yap;
using namespace std;

namespace Yap
{
namespace _details
{



#define IMPLEMENT_VARIABLE         public: virtual int GetType() const override { return _type;} \
	virtual const wchar_t * GetId() const override { return _id.c_str(); }\
	virtual void SetId(const wchar_t * id) override { _id = id; } \
	virtual const wchar_t * GetDescription() const override	{ return _description.c_str();	} \
	virtual void SetDescription(const wchar_t * description) override {	_description = description;	}\
	protected: \
		std::wstring _id; \
		std::wstring _description; \
		int _type;

	template <typename TYPE>
	class SimpleVariable : public ISimpleVariable<TYPE>
	{
        IMPLEMENT_SHARED(SimpleVariable<TYPE>)
		IMPLEMENT_VARIABLE
    public:
		SimpleVariable(
			const wchar_t * id,
			const wchar_t * description) :
			_id{ id },
			_description{ description != nullptr ? description : L"" },
			_type{ variable_type_id<TYPE>::type } {}

		SimpleVariable(const SimpleVariable<TYPE>& rhs) :
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type },
			_value{ rhs._value } {}

		SimpleVariable(ISimpleVariable<TYPE> & rhs) :
			_id{ rhs.GetId() },
			_description{ rhs.GetDescription() },
			_type{ rhs.GetType() },
			_value{ rhs.Get() } {}

		virtual TYPE Get() const override {
			return _value;
		}

		virtual void Set(const TYPE value) override {
			_value = value;
		}
	private:
		TYPE _value;
	};

	template <>
	class SimpleVariable<const wchar_t *> : public ISimpleVariable<const wchar_t*>
	{
	private:
		IMPLEMENT_SHARED(SimpleVariable<const wchar_t*>)
		IMPLEMENT_VARIABLE
	public:
		SimpleVariable(
			const wchar_t * id,
			const wchar_t * description) :
			_id{ id },
			_description{ description != nullptr ? description : L"" },
			_type{ VariableString } {}

		SimpleVariable(const SimpleVariable<const wchar_t*>& rhs) :
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type }, 
			_value{ rhs._value } {}

		SimpleVariable(ISimpleVariable<const wchar_t*> & rhs) :
			_id{ rhs.GetId() },
			_description{ rhs.GetDescription() },
			_type{ rhs.GetType() },
			_value{ rhs.Get() } {}

		virtual const wchar_t * Get() const override {
			return _value.c_str();
		}

		virtual void Set(const wchar_t * value) override{
			_value = value;
		}
	private:
		std::wstring _value;
	};


	template <typename VALUE_TYPE>
	class ArrayVariable : public IArrayVariable<VALUE_TYPE>
	{
		IMPLEMENT_SHARED(ArrayVariable<VALUE_TYPE>)
		IMPLEMENT_VARIABLE
	public:
		ArrayVariable(size_t size, VALUE_TYPE value, const wchar_t * id, const wchar_t * description) :
			_id{ id },
			_description{ description },
			_type{ variable_type_id<VALUE_TYPE>::array_type } {
			_elements.resize(size, value);
		}

		ArrayVariable(const ArrayVariable<VALUE_TYPE>& rhs) :
			_elements{ rhs._elements },
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type } {}

		ArrayVariable(IArrayVariable<VALUE_TYPE>& rhs) :
			_id{ rhs.GetId() },
			_type{ variable_type_id<VALUE_TYPE>::array_type },
			_description{ rhs.GetDescription() } {
			auto source_elements = rhs.Elements();
			_elements.resize(rhs.GetSize());
			for (size_t i = 0; i < rhs.GetSize(); ++i) {
				_elements[i] = source_elements[i];
			}
		}

		virtual size_t GetSize() const override {
			return _elements.size();
		}

		virtual void SetSize(size_t size) override 	{
			_elements.resize(size);
		}

		virtual VALUE_TYPE * Elements() override {
			return _elements.data();
		}

	private:
		std::vector<VALUE_TYPE> _elements;
	};


	template <>
	class ArrayVariable<bool> : public IArrayVariable<bool>
	{
		IMPLEMENT_SHARED(ArrayVariable<bool>)
		IMPLEMENT_VARIABLE
	public:
		ArrayVariable(size_t size, bool value, const wchar_t * id, const wchar_t * description) :
			_id{ id },
			_description{ description },
			_type{ variable_type_id<bool>::array_type } {
			_elements.resize(size, value);
		}

		ArrayVariable(const ArrayVariable<bool>& rhs) :
			_elements{ rhs._elements },
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type } {}

		ArrayVariable(IArrayVariable<bool>& rhs) :
			_id{ rhs.GetId() },
			_type{ variable_type_id<bool>::array_type },
			_description{ rhs.GetDescription() } {
			auto source_elements = rhs.Elements();
			_elements.resize(rhs.GetSize());
			for (size_t i = 0; i < rhs.GetSize(); ++i) {
				_elements[i] = source_elements[i];
			}
		}

		virtual size_t GetSize() const override {
			return _elements.size();
		}

		virtual void SetSize(size_t size) override {
			_elements.resize(size);
		}

		virtual bool * Elements() override {
			return reinterpret_cast<bool*>(_elements.data());
		}

	private:
		std::vector<unsigned char> _elements;
	};

	template<>
	class ArrayVariable<SmartPtr<IVariable>> : public IArrayVariable<SmartPtr<IVariable>>
	{
		IMPLEMENT_SHARED(ArrayVariable<SmartPtr<IVariable>>)
		IMPLEMENT_VARIABLE

	public:
		ArrayVariable(size_t size, IVariable* value, const wchar_t * id, const wchar_t * description) :
			_id{ id },
			_description{ description },
			_type{ variable_type_id<IVariable*>::array_type } {
			assert(size >= 1 && "There should be at least one element in the array.");
			assert(value != nullptr && "The template could not be null.");
			_elements.resize(size);
			for (auto& element : _elements) {
				element = YapShared(value->Clone());
			}
		}

		ArrayVariable(const ArrayVariable<SmartPtr<IVariable>>& rhs) :
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type } {
			_elements.reserve(rhs.GetSize());
			for (auto element : rhs._elements) {
				_elements.push_back(YapShared(element->Clone()));
			}
		}

		ArrayVariable(IArrayVariable<SmartPtr<IVariable>>& rhs) :
			_id{ rhs.GetId() },
			_type{ variable_type_id<IVariable*>::array_type },
			_description{ rhs.GetDescription() } 
		{
			auto source_elements = rhs.Elements();
			_elements.resize(rhs.GetSize());
			for (size_t i = 0; i < rhs.GetSize(); ++i)
			{
				_elements[i] = YapShared(source_elements[i]->Clone());
			}
		}

		virtual size_t GetSize() const override {
			return _elements.size();
		}

		virtual void SetSize(size_t size) override {
			size_t old_size = _elements.size();
			_elements.resize(size);
			for (size_t i = old_size; i < size; ++i) {
				_elements[i] = YapShared(_elements[0]->Clone());
			}
		}

		virtual SmartPtr<IVariable> * Elements() override {
			return _elements.data();
		}

	private:
		std::vector<SmartPtr<IVariable>> _elements;
	};

	struct StructVariable : public IStructVariable
	{
		IMPLEMENT_SHARED(StructVariable)
		IMPLEMENT_VARIABLE
	public:
		StructVariable(const wchar_t * id, const wchar_t * description) :
			_id{ id }, _description{ description }, _type{ VariableStruct },
			_members{ YapShared(new ContainerImpl<IVariable>) } {}

		StructVariable(const StructVariable& rhs) :
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type },
			_members{ YapShared( rhs._members->Clone()) } {
		}

		StructVariable(IStructVariable& rhs) :
			_id{ rhs.GetId() },
			_description{ rhs.GetDescription() },
			_type{ VariableStruct },
			_members{ YapShared(rhs.Members()->Clone()) } {}

		StructVariable(IPtrContainer<IVariable> * variables, const wchar_t * id, const wchar_t * description) :
			_members{ YapShared(variables) },
			_id{ id },
			_description{ description },
			_type{ VariableStruct } {
		}
		virtual IPtrContainer<IVariable> * Members() override {
			return _members.get();
		};

		SmartPtr<ContainerImpl<IVariable>> _members;
	};
}  // end Yap::_details

using namespace _details;

VariableSpace::VariableSpace() :
	_variables(YapShared(new ContainerImpl<IVariable>))
{
    InitTypes();
}

VariableSpace::VariableSpace(IVariableContainer * variables) :
	_variables(YapShared(variables))
{
    InitTypes();
}

VariableSpace::VariableSpace(const VariableSpace& rhs) :
	_variables(YapShared(rhs.Variables()->Clone()))
{
    InitTypes();
}

VariableSpace::~VariableSpace()
{
}

bool VariableSpace::InitTypes()
{
    _types.emplace(L"int", YapShared(new SimpleVariable<int>(L"int", nullptr)));
    _types.emplace(L"float", YapShared(new SimpleVariable<double>(L"float", nullptr)));
    _types.emplace(L"string", YapShared(new SimpleVariable<const wchar_t*>(L"string", nullptr)));
    _types.emplace(L"bool", YapShared(new SimpleVariable<bool>(L"bool", nullptr)));
	
	_types.emplace(L"array<int>", YapShared(new ArrayVariable<int>(1, 0, L"array<int>", nullptr)));
	_types.emplace(L"array<float>", YapShared(new ArrayVariable<double>(1, 0.0, L"array<float>", nullptr)));
	_types.emplace(L"array<bool>", YapShared(new ArrayVariable<bool>(1, false, L"array<bool>", nullptr)));
	_types.emplace(L"array<string>", YapShared(new ArrayVariable<const wchar_t*>(1, L"", L"array<string>", nullptr)));

	return true;
}

using namespace _details;

bool VariableSpace::Add(int type, const wchar_t * name, const wchar_t * description)
{
	static map<int, wstring> type_to_string{
		{VariableInt, L"int"},
		{VariableFloat, L"float"},
		{VariableBool, L"bool"},
		{VariableString, L"string"},
		{VariableIntArray, L"array<int>"},
		{VariableFloatArray, L"array<float>"},
		{VariableBoolArray, L"array<bool>"},
		{VariableStringArray, L"array<string>"},};
	assert(type_to_string.find(type) != type_to_string.end());

	return Add(type_to_string[type].c_str(), name, description);
}

bool VariableSpace::Add(const wchar_t * type,
                          const wchar_t * id,
                          const wchar_t * description)
{
    auto iter = _types.find(type);
    if (iter == _types.end())
        return false;

    if (!iter->second)
        return false;

    auto new_variable = dynamic_cast<IVariable*>(iter->second->Clone());
    if (new_variable == nullptr)
        return false;

    new_variable->SetId(id);
    new_variable->SetDescription(description);
    _variables->Add(id, new_variable);

    return true;
}

bool VariableSpace::Add(IVariable* variable)
{
    assert(variable != nullptr);
    return _variables->Add(variable->GetId(), variable);
}

IVariableContainer* VariableSpace::Variables()
{
	return _variables.get();
}

const IVariableContainer* VariableSpace::Variables() const
{
	return _variables.get();
}

IVariable * VariableSpace::GetVariable(IVariableContainer * variables,
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
			auto array_variable = dynamic_cast<IStructArray*>(variable);
			assert(array_variable != nullptr);

			if (index < 0 || index >= int(array_variable->GetSize()))
				throw VariableException(id, VariableException::OutOfRange);

			lhs_variable = array_variable->Elements()[index].get();
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
		if (variable == nullptr)
			throw VariableException(id, VariableException::VariableNotFound);

		if ((variable->GetType() | type) != type)
			throw VariableException(id, VariableException::TypeNotMatch);

		return variable;
	}
}

IVariable * VariableSpace::GetVariable(const wchar_t * id, int expected_type)
{
	assert(id != nullptr && id[0] != 0);
	return GetVariable(_variables.get(), id, expected_type);
}

shared_ptr<VariableSpace> VariableSpace::Load(const wchar_t * path)
{
    VdfParser parser;
    return parser.CompileFile(path);
}

void VariableSpace::Reset()
{
	_variables = YapShared(new ContainerImpl<IVariable>);
}

bool VariableSpace::TypeExists(const wchar_t * type) const
{
    return _types.find(type) != _types.end();
}

bool VariableSpace::VariableExists(const wchar_t *variable_id) const
{
    auto This = const_cast<VariableSpace*>(this);
    return This->_variables->Find(variable_id) != nullptr;
}

const IVariable * VariableSpace::GetType(const wchar_t * type) const
{
    auto iter = _types.find(type);
    return (iter != _types.end()) ? iter->second.get() : nullptr;
}

bool VariableSpace::AddType(const wchar_t * type_id, IPtrContainer<IVariable> * variables)
{
    try
    {
        auto variable = new StructVariable(variables, type_id, nullptr);
        return AddType(type_id, variable);
    }
    catch(bad_alloc&)
    {
        return false;
    }
}

bool VariableSpace::AddType(const wchar_t * type_id, IVariable *type)
{
    assert(_types.find(type_id) == _types.end());
    assert(type != nullptr);
    assert(type_id != nullptr);

    if (_types.find(type_id) != _types.end())
        return false;

    _types.insert({type_id, YapShared(type)});

    return true;
}

bool VariableSpace::ResizeArray(const wchar_t * id, size_t size)
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

}	// end Yap
