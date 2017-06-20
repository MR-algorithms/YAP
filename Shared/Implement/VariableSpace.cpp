#include "VariableSpace.h"
#include "Interface/Interfaces.h"
#include "ContainerImpl.h"
#include "../API/Yap/VdfParser.h"

#include <fstream>
#include <string>
#include <vector>
#include "utilities/StringHelper.h"
#include <sstream>

using namespace Yap;
using namespace std;

#define IMPLEMENT_VARIABLE_NO_ENABLE public: \
	virtual int GetType() const override { return _type;} \
	virtual const wchar_t * GetId() const override { return _id.c_str(); }\
	virtual void SetId(const wchar_t * id) override { _id = id; } \
	virtual const wchar_t * GetTitle() const override { return _title.c_str();}\
	virtual void SetTitle(const wchar_t * title) override { _title = title;}\
	virtual const wchar_t * GetDescription() const override	{ return _description.c_str();	} \
	virtual void SetDescription(const wchar_t * description) override {	_description = description;	}\
protected: \
	std::wstring _id; \
	std::wstring _description; \
	std::wstring _title;\
	int _type;

#define IMPLEMENT_VARIABLE   IMPLEMENT_VARIABLE_NO_ENABLE \
public: \
	virtual void Enable(bool enable) { _enabled = enable;} \
	virtual bool IsEnabled() const override { return _enabled;} \
protected: \
	bool _enabled;

namespace Yap
{
namespace _details
{
	template<typename T> struct variable_store_type
	{
		typedef T type;
		typedef std::vector<T> vector_type;
	};

	template <> struct variable_store_type<bool> 
	{
		typedef bool type;
		typedef std::vector<bool> vector_type;
	};

	template <> struct variable_store_type<wchar_t *> 
	{
		typedef std::wstring type;
		typedef std::vector<std::wstring> vector_type;
	};
	
	template <> struct variable_store_type<IVariable*>
	{
		typedef SmartPtr<IVariable> type;
		typedef std::vector<type> vector_type;
	};

	template <typename TYPE>
	class SimpleVariable : public ISimpleVariable<TYPE>
	{
		typedef typename variable_store_type<TYPE>::type type;
		typedef typename variable_type<TYPE>::const_type const_type;

		IMPLEMENT_SHARED(SimpleVariable<TYPE>)
		IMPLEMENT_VARIABLE
    public:
		SimpleVariable(
			const wchar_t * id,
			const wchar_t * description) :
			_id{ id },
			_description{ description != nullptr ? description : L"" },
			_type{ variable_type_id<TYPE>::type_id } 
		{
		}

		SimpleVariable(const SimpleVariable<TYPE>& rhs) :
			_id{ rhs._id },
			_description{ rhs._description },
			_type{ rhs._type },
			_value{ rhs._value } 
		{
		}

		SimpleVariable(ISimpleVariable<TYPE> & rhs) :
			_id{ rhs.GetId() },
			_description{ rhs.GetDescription() },
			_type{ rhs.GetType() },
			_value{ rhs.Get() } 
		{
		}

		virtual const_type Get() const override 
		{
			return _value;
		}

		virtual void Set(TYPE value) override 
		{
			_value = value;
		}

		virtual size_t FromString(const wchar_t * value_string) override
		{
			std::wistringstream input(value_string);
			auto begin = input.tellg();
			input >> _value;
			return static_cast<size_t>(input.tellg() - begin);
		}

		virtual const wchar_t * ToString() override
		{
			std::wostringstream output;
			output << _value;
			_value_string = output.str();

			return _value_string.c_str();
		}

	private:
		type _value;
		std::wstring _value_string;
	};

	// Begin specialization for string type.
 	template <>
	SimpleVariable<wchar_t*>::const_type SimpleVariable<wchar_t *>::Get() const
	{
		return _value.c_str();
	}

	template <>
	void SimpleVariable<wchar_t *>::Set(wchar_t * value) 
	{
		_value = value;
	}

	template <>
	const wchar_t * SimpleVariable<wchar_t*>::ToString() 
	{
		_value_string = L'\"';
		_value_string += _value + L'\"';
		return _value.c_str();
	}

	template <>
	size_t SimpleVariable<wchar_t *>::FromString(const wchar_t * value_string)
	{
		assert(value_string != nullptr);
		_value_string = value_string;
		assert(!_value_string.empty());

		auto first_quote_pos = _value_string.find_first_not_of(L" \t\n\r");
		if (first_quote_pos == wstring::npos)
			return 0;

		if (_value_string[first_quote_pos] != L'\"')
			return 0;

		auto second_quote_pos = _value_string.find(L'\"', first_quote_pos + 1);
		if (second_quote_pos == wstring::npos)
			return 0;

		_value = _value_string.substr(first_quote_pos + 1, second_quote_pos - first_quote_pos - 1);
		return second_quote_pos + 1;
	}
	 // End specialization for SimpleVariable<wchar_t *>.


	template <typename T>
	class ValueArrayVariable : public virtual IValueArrayVariable<T>
	{
		typedef typename variable_store_type<T>::vector_type vector_type;
		typedef typename variable_store_type<T>::type type;

		IMPLEMENT_SHARED(ValueArrayVariable<T>)
		IMPLEMENT_VARIABLE

	public:
		ValueArrayVariable(size_t size, T value, const wchar_t * id, const wchar_t * title = nullptr, const wchar_t * description = nullptr) :
			_id{ id },
			_title{ title },
			_description{ description != nullptr ? description : L"" },
			_type{ variable_type_id<T>::array_type_id }
		{
			_elements.resize(size, value);
		}

		ValueArrayVariable(const ValueArrayVariable<T>& rhs) :
			_elements{ rhs._elements },
			_id{ rhs._id },
			_title{ rhs._title },
			_description{ rhs._description },
			_type{ rhs._type }
		{
		}

		ValueArrayVariable(IValueArrayVariable<T>& rhs) :
			_id{ rhs.GetId() },
			_title{ rhs.GetTitle() },
			_type{ variable_type_id<VALUE_TYPE>::array_type_id },
			_description{ rhs.GetDescription() }
		{
			auto source_elements = rhs.Data();
			_elements.resize(rhs.GetSize());
			for (size_t i = 0; i < rhs.GetSize(); ++i) {
				_elements[i] = source_elements[i];
			}
		}

		virtual size_t GetSize() const override
		{
			return _elements.size();
		}

		virtual void SetSize(size_t size) override
		{
			_elements.resize(size);
		}

		virtual const_type Get(size_t index) const override
		{
			assert(index < _elements.size());
			return _elements[index];
		}

		virtual void Set(size_t index, T value) override
		{
			assert(index < _elements.size());
			_elements[index] = value;
		}

		virtual const wchar_t * ToString() override
		{
			_value_string = L'[';
			bool first = true;
			for (auto element : _elements)
			{
				if (!first)
				{
					_value_string += L", ";
				}
				else
				{
					first = false;
				}
				_value_string += ElementToString(element);
			}
			_value_string += L']';

			return _value_string.c_str();
		}

		virtual size_t FromString(const wchar_t * value_string) override
		{
			_elements.clear();
			_value_string = value_string;

			auto left_bracket_pos = _value_string.find_first_not_of(L" \t\n\r");
			if (left_bracket_pos == wstring::npos)
				return 0;

			if (_value_string[left_bracket_pos] != L'[' || _value_string.size() < left_bracket_pos + 3)
				return 0;

			size_t begin = left_bracket_pos + 1;

			for (;;)
			{
				type element;
				auto chars_consumed_by_element = ElementFromString(element, _value_string.c_str() + begin);
				if (chars_consumed_by_element == 0)
					return 0;

				_elements.push_back(element);
				auto separator_pos = _value_string.find_first_not_of(L" \t\n\r", begin + chars_consumed_by_element);
				if (separator_pos == wstring::npos)
					return 0;

				if (_value_string[separator_pos] == L']')
				{
					return separator_pos + 1;
				}
				else if (_value_string[separator_pos] == L',')
				{
					begin = separator_pos + 1;
					if (begin >= _value_string.size())
						return 0;
				}
				else
				{
					return 0;
				}
			}
		}

	protected:
		std::wstring ElementToString(type element)
		{
			std::wostringstream output;
			output << element;
			return output.str();
		}

		size_t ElementFromString(type& element, const wchar_t * value_string)
		{
			wistringstream input(value_string);
			input >> element;
			return static_cast<size_t>(input.tellg());
		}

		vector_type _elements;
		std::wstring _value_string;
	};

	template <typename T>
	class ReferenceArrayVariable : public ValueArrayVariable<T>, public IElementReference<T>
	{
		IMPLEMENT_SHARED(ReferenceArrayVariable<T>)
	public:
		using ValueArrayVariable<T>::ValueArrayVariable;

		virtual T& Element(size_t index) override
		{
			assert(index < _elements.size());
			return _elements[index];
		}
	};

	template <typename T>
	class RawArrayVariable : public ReferenceArrayVariable<T>, public IRawArray<T>
	{
		IMPLEMENT_SHARED(RawArrayVariable<T>)
	public:
		using ReferenceArrayVariable<T>::ReferenceArrayVariable;

		virtual T * Data() override
		{
			return _elements.data();
		}
	};

	// Specialization for ValueArrayVariable<wchar_t *>.

	template <>
	ValueArrayVariable<wchar_t*>::const_type ValueArrayVariable<wchar_t *>::Get(size_t index) const
	{
		assert(index < _elements.size());
		return _elements[index].c_str();
	}

	template <>
	void ValueArrayVariable<wchar_t *>::Set(size_t index, wchar_t * value)
	{
		assert(index < _elements.size());
		_elements[index] = value;
	}

	template <>
	ValueArrayVariable<IVariable*>::const_type ValueArrayVariable<IVariable *>::Get(size_t index) const
	{
		assert(index < _elements.size());
		return _elements[index].get();
	}

	template <>
	void ValueArrayVariable<IVariable*>::Set(size_t index, IVariable * value) 
	{
		assert(index < _elements.size());
		_elements[index] = YapShared(value);
	}

	// Begin specialization for ValueArrayVariable<bool>.
	template <>
	std::wstring ValueArrayVariable<bool>::ElementToString(bool element)
	{
		return element ? L"true" : L"false";
	}

	template <>
	size_t ValueArrayVariable<bool>::ElementFromString(bool& element, const wchar_t * value_string)
	{
		wstring str;
		if (str.substr(0, 4) == L"true")
		{
			element = true;
			return 4;
		}
		else if (str.substr(0, 5) == L"false")
		{
			element = false;
			return 5;
		}
		else
		{
			return 0;
		}
	}


	// Begin specialization for ArrayVariable<SmartPtr<IVariable>>

	template <>
	ValueArrayVariable<IVariable*>::ValueArrayVariable(size_t size, 
		IVariable* value, const wchar_t * id, const wchar_t * title, const wchar_t * description) 
	{
		assert(size >= 1 && "There should be at least one element in the array.");
		assert(value != nullptr && "The template could not be null.");

		_type = variable_type_id<IVariable*>::array_type_id;

		_elements.resize(size);

		for (auto& element : _elements)
		{
			element = YapShared(value->Clone());
		}
	}

	template <>
	ValueArrayVariable<IVariable*>::ValueArrayVariable(const ValueArrayVariable<IVariable*>& rhs) :
		_id{ rhs._id },
		_title{rhs._title},
		_description{ rhs._description }
	{
		_type = rhs._type;

		_elements.reserve(rhs.GetSize());
		for (auto element : rhs._elements)
		{
			_elements.push_back(YapShared(element->Clone()));
		}
	}

	template <>
	IVariable *& ReferenceArrayVariable<IVariable*>::Element(size_t index)
	{
		assert(index < _elements.size());
		return _elements[index].get();
	}

	template <>
	void ReferenceArrayVariable<IVariable*>::SetSize(size_t size) 
	{
		size_t old_size = _elements.size();
		_elements.resize(size);
		for (size_t i = old_size; i < size; ++i) 
		{
			_elements[i] = YapShared(_elements[0]->Clone());
		}
	}

	template <>
	std::wstring ReferenceArrayVariable<IVariable*>::ElementToString(type element)
	{
		return element->ToString();
	}

	template <>
	size_t ReferenceArrayVariable<IVariable*>::ElementFromString(type& element, const wchar_t * value_string)
	{
		return element->FromString(value_string);
	}


	struct StructVariable : public IStructVariable
	{
		IMPLEMENT_SHARED(StructVariable)
		IMPLEMENT_VARIABLE_NO_ENABLE
	public:
		StructVariable(const wchar_t * id, const wchar_t * title = nullptr, const wchar_t * description = nullptr) :
			_id{ id }, 
			_title{title},
			_description{ description != nullptr ? description : L"" },
			_type{ VariableStruct },
			_enabled{false},
			_members{ YapShared(new ContainerImpl<IVariable>) } 
		{
		}

		StructVariable(const StructVariable& rhs) :
			_id{ rhs._id },
			_title{rhs._title},
			_description{ rhs._description },
			_type{ rhs._type },
			_members{ YapShared( rhs._members->Clone()) } 
		{
		}

		StructVariable(IStructVariable& rhs) :
			_id{ rhs.GetId() },
			_title {rhs.GetTitle()},
			_description{ rhs.GetDescription() },
			_type{ VariableStruct },
			_members{ YapShared(rhs.Members()->Clone()) } 
		{
		}

		StructVariable(IPtrContainer<IVariable> * variables, const wchar_t * id, const wchar_t * title = nullptr, const wchar_t * description = nullptr) :
			_members{ YapShared(variables) },
			_id{ id },
			_title{title},
			_description{ description != nullptr ? description : L"" },
			_type{ VariableStruct } 
		{
		}

		virtual IPtrContainer<IVariable> * Members() override 
		{
			return _members.get();
		}

		virtual void Enable(bool enable)
		{
			_enabled = enable;
			assert(_members);
			auto iter = _members->GetIterator();
			for (auto member = iter->GetFirst(); member != nullptr; member = iter->GetNext())
			{
				member->Enable(enable);
			}
		}

        virtual bool IsEnabled() const override
		{
			return _enabled;
		}

		virtual const wchar_t * ToString()  
		{
			assert(_members);
			_value_string = L'{';

			auto iter = _members->GetIterator();
			bool first = true;
			for (auto member = iter->GetFirst(); member != nullptr; member = iter->GetNext())
			{
				if (!first)
				{
					_value_string += L", ";
				}
				else
				{
					first = false;
				}

				_value_string += L'\"';
				_value_string += member->GetId();
				_value_string += L"\":";
				_value_string += member->ToString();
			}

			_value_string += L'}';

			return _value_string.c_str();
		}

		virtual size_t FromString(const wchar_t * value_string) override
		{
			assert(_members);
			_value_string = value_string;

			auto pos = _value_string.find_first_not_of(L" \t\n\r");
			if (pos == wstring::npos)
				return 0;

			if (_value_string[pos] != L'{')
				return 0;

			for (;;)
			{
				if (pos + 1 >= _value_string.size())
					return 0;

				pos = _value_string.find_first_not_of(L" \t\n\r", pos + 1);
				if (pos == wstring::npos || _value_string[pos] != L'\"')
					return 0;

				if (pos + 1 >= _value_string.size())
					return 0;

				auto quote_pos = _value_string.find(L'\"', pos + 1);
				if (quote_pos == wstring::npos)
					return 0;

				auto member_id = _value_string.substr(pos + 1, quote_pos - pos - 1);
				pos = quote_pos;
				if (pos + 1 > _value_string.size())
					return 0;

				pos = _value_string.find_first_not_of(L" \t\n\r", pos + 1);
				if (_value_string[pos] != L':')
					return 0;

				auto member = _members->Find(member_id.c_str());
				auto char_consumed = member->FromString(_value_string.c_str() + pos + 1);
				if (char_consumed == 0)
					return 0;

				pos = pos + 1 + char_consumed;
				pos = _value_string.find_first_not_of(L" \t\n\r", pos);
				if (pos == wstring::npos)
					return 0;

				if (_value_string[pos] == L',')
				{
					pos = pos + 1;
					if (pos >= _value_string.size())
						return 0;
				}
				else if (_value_string[pos] == L'}')
				{
					return pos + 1;
				}
				else 
				{
					return 0;
				}
			}
		}

	private:
		SmartPtr<ContainerImpl<IVariable>> _members;
		bool _enabled;
		wstring _value_string;

	};
};  // end Yap::_details

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
	_variables{ YapShared(rhs.Variables()->Clone()) },
	_types {rhs._types}
{
    InitTypes();
}

VariableSpace::VariableSpace(VariableSpace&& rhs) :
	_variables{rhs._variables},
	_types{rhs._types}
{
	rhs._variables.reset();
}

VariableSpace::~VariableSpace()
{
}

const VariableSpace& VariableSpace::operator = (const VariableSpace& rhs)
{
	_variables = YapShared(rhs.Variables()->Clone());
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

bool VariableSpace::InitTypes()
{
    _types.emplace(L"int", YapShared(new SimpleVariable<int>(L"int", nullptr)));
    _types.emplace(L"float", YapShared(new SimpleVariable<double>(L"float", nullptr)));
    _types.emplace(L"string", YapShared(new SimpleVariable<wchar_t*>(L"string", nullptr)));
    _types.emplace(L"bool", YapShared(new SimpleVariable<bool>(L"bool", nullptr)));
	
	_types.emplace(L"array<int>", YapShared(new RawArrayVariable<int>(1, 0, L"array<int>", nullptr)));
	_types.emplace(L"array<float>", YapShared(new RawArrayVariable<double>(1, 0.0, L"array<float>", nullptr)));
	_types.emplace(L"array<bool>", YapShared(new ValueArrayVariable<bool>(1, false, L"array<bool>", nullptr)));
	_types.emplace(L"array<string>", YapShared(new ValueArrayVariable<wchar_t*>(1, L"", L"array<string>", nullptr)));

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

			auto element_reference = dynamic_cast<IElementReference<IVariable*>*>(variable);
			assert(element_reference != nullptr);

			lhs_variable = element_reference->Element(index);
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

		if (type != VariableAllTypes && ((variable->GetType() | type) != type))
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
    return This->GetVariable(variable_id) != nullptr;
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
