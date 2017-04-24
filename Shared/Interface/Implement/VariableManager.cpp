#include "VariableManager.h"
#include "../Interfaces.h"
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
    template <typename VALUE_TYPE>
    class ValueImpl : public IValue<VALUE_TYPE>
    {
    public:
        explicit ValueImpl(VALUE_TYPE value) : _value{ value } {}
        ValueImpl(const ValueImpl& rhs) : _value{ rhs.value } {}
        ValueImpl(const IValue<VALUE_TYPE>& rhs) : _value{ rhs.Get() } {}

        virtual VALUE_TYPE Get() const {
            return _value;
        }

        virtual void Set(const VALUE_TYPE value) {
            _value = value;
        }
    private:
        VALUE_TYPE _value;
    };

    class StringValueImpl : public IStringValue
    {
    public:
        StringValueImpl(const wchar_t * value) : _value{ value } {}
        StringValueImpl(const StringValueImpl& rhs) : _value{ rhs._value } {}
        StringValueImpl(const IStringValue& rhs) : _value{ rhs.Get() } {}

        virtual const wchar_t * Get() const override {
            return _value.c_str();
        }

        virtual void Set(const wchar_t * value) override {
            _value = value;
        }
    private:
        std::wstring _value;
    };

    template <typename VALUE_TYPE>
    class ArrayValueImpl : public IArrayValue<VALUE_TYPE>
    {
    public:
        ArrayValueImpl() {}
        ArrayValueImpl(const ArrayValueImpl& rhs) : _elements(rhs._elements){}
        ArrayValueImpl(const IArrayValue& rhs) : _elements(rhs.GetSize())
        {
            for (size_t i = 0; i < _elements.size(); ++i)
            {
                _elements[i] = const_cast<IArrayValue&>(rhs).Elements()[i];
            }
        }

        ArrayValueImpl(size_t size)
        {
            _elements.resize(size);
        }

        virtual size_t GetSize() const override
        {
            return _elements.size();
        }

        virtual void SetSize(size_t size) override
        {
            _elements.resize(size);
        }

        virtual VALUE_TYPE * Elements() override
        {
            return _elements.data();
        }
    private:
        std::vector<VALUE_TYPE> _elements;
    };

    template<>
    class ArrayValueImpl<bool> : public IArrayValue<bool>
    {
    public:
        ArrayValueImpl()
        {}

        ArrayValueImpl(const ArrayValueImpl<bool>& rhs) : _elements(rhs.GetSize()){
            for (size_t i = 0; i < _elements.size(); ++i)
            {
                _elements[i] = const_cast<ArrayValueImpl<bool>&>(rhs).Elements()[i];
            }
        }

        ArrayValueImpl(const IArrayValue<bool>& rhs) : _elements(rhs.GetSize()){
            for (size_t i = 0; i < _elements.size(); ++i)
            {
                _elements[i] = const_cast<IArrayValue<bool>&>(rhs).Elements()[i];
            }
        }

        ArrayValueImpl(size_t size)
        {
            _elements.resize(size);
        }

        virtual size_t GetSize() const override
        {
            return _elements.size();
        }

        virtual void SetSize(size_t size) override
        {
            _elements.resize(size);
        }

        virtual bool * Elements() override
        {
            return reinterpret_cast<bool*>(_elements.data());
        }

    private:
        std::vector<unsigned char> _elements;
    };

    template<>
    class ArrayValueImpl<IVariable*> : public IArrayValue<IVariable*>
    {
    public:
        ArrayValueImpl()
        {}

        ArrayValueImpl(const ArrayValueImpl<IVariable*>& rhs) : _elements(rhs.GetSize())
        {
            for (size_t i = 0; i < _elements.size(); ++i)
            {
                _elements[i] = dynamic_cast<IVariable*>(const_cast<ArrayValueImpl<IVariable*>&>(rhs).Elements()[i]->Clone());
                _elements[i]->Lock();
            }
        }

        ArrayValueImpl(const IArrayValue<IVariable*>& rhs) : _elements(rhs.GetSize())
        {
            for (size_t i = 0; i < _elements.size(); ++i)
            {
                _elements[i] = dynamic_cast<IVariable*>(const_cast<IArrayValue<IVariable*>&>(rhs).Elements()[i]->Clone());
                _elements[i]->Lock();
            }
        }

        ArrayValueImpl(size_t size)
        {
            _elements.resize(size);
        }

        ~ArrayValueImpl()
        {
            for (auto element : _elements)
            {
                element->Release();
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

        virtual IVariable ** Elements() override
        {
            return reinterpret_cast<IVariable**>(_elements.data());
        }

    private:
        std::vector<IVariable*> _elements;
    };

    class VariableImpl :
        public IVariable
    {
        IMPLEMENT_SHARED(VariableImpl)

    public:
        VariableImpl(int type,
            const wchar_t * id,
            const wchar_t * description) :
            _type(type),
            _id(id),
            _description(description != nullptr ? description : L"")
        {
            switch (type)
            {
            case VariableBool:
                _value_interface = new ValueImpl<bool>(false);
                break;
            case VariableInt:
                _value_interface = new ValueImpl<int>(0);
                break;
            case VariableFloat:
                _value_interface = new ValueImpl<double>(0.0);
                break;
            case VariableString:
                _value_interface = new StringValueImpl(L"");
                break;
            case VariableBoolArray:
                _value_interface = new ArrayValueImpl<bool>();
                break;
            case VariableIntArray:
                _value_interface = new ArrayValueImpl<int>();
                break;
            case VariableFloatArray:
                _value_interface = new ArrayValueImpl<double>();
                break;
            case VariableStringArray:
                _value_interface = new ArrayValueImpl<std::wstring>();
                break;
            case VariableStruct:
                _value_interface = nullptr;
                break;
            case VariableStructArray:
                _value_interface = new ArrayValueImpl<IVariable*>();
                break;
            default:
                throw VariableException(id, VariableException::InvalidType);
            }
        }

        VariableImpl(const IVariable& rhs) :
            _type{ rhs.GetType() },
            _id{ rhs.GetId() },
            _description{ rhs.GetDescription() }
        {
            auto rhs_value_interface = const_cast<IVariable&>(rhs).ValueInterface();
            switch (rhs.GetType())
            {
            case VariableBool:
                _value_interface = new ValueImpl<bool>(
                    *reinterpret_cast<IBoolValue*>(rhs_value_interface));
                break;
            case VariableInt:
                _value_interface = new ValueImpl<int>(
                    *reinterpret_cast<IIntValue*>(rhs_value_interface));
                break;
            case VariableFloat:
                _value_interface = new ValueImpl<double>(
                    *reinterpret_cast<IDoubleValue*>(rhs_value_interface));
                break;
            case VariableString:
                _value_interface = new StringValueImpl(
                    *reinterpret_cast<IStringValue*>(rhs_value_interface));
                break;
            case VariableStruct:
                _value_interface = reinterpret_cast<IStructValue*>(rhs_value_interface)->Clone();
                break;
            case VariableBoolArray:
                _value_interface = new ArrayValueImpl<bool>(
                    *reinterpret_cast<IArrayValue<bool>*>(rhs_value_interface));
                break;
            case VariableIntArray:
                _value_interface = new ArrayValueImpl<int>(
                    *reinterpret_cast<IArrayValue<int>*>(rhs_value_interface));
                break;
            case VariableFloatArray:
                _value_interface = new ArrayValueImpl<double>(
                    *reinterpret_cast<IArrayValue<double>*>(rhs_value_interface));
                break;
            case VariableStringArray:
                _value_interface = new ArrayValueImpl<wstring>(
                    *reinterpret_cast<IArrayValue<wstring>*>(rhs_value_interface));
                break;
            case VariableStructArray:
                _value_interface = new ArrayValueImpl<IVariable*>(
                    *reinterpret_cast<IArrayValue<IVariable*>*>(rhs_value_interface));
                break;
            default:
                throw VariableException(_id.c_str(), VariableException::InvalidType);

            }
        }

        ~VariableImpl()
        {
            switch (_type)
            {
            case VariableBool:
            case VariableInt:
            case VariableFloat:
            case VariableString:
            case VariableBoolArray:
            case VariableIntArray:
            case VariableStringArray:
            case VariableFloatArray:
                delete _value_interface;
                break;
            case VariableStruct:
                if (_value_interface != nullptr)
                {
                    reinterpret_cast<IStructValue*>(_value_interface)->Release();
                }
                break;
            case VariableStructArray:
                BUG(Not implemented yet.);
                break;
            }
        }

        virtual int GetType() const override
        {
            return _type;
        }

        virtual const wchar_t * GetId() const override
        {
            return _id.c_str();
        }

        virtual void SetId(const wchar_t * id) override
        {
            _id = id;
        }

        virtual const wchar_t * GetDescription() const override
        {
            return _description.c_str();
        }

        virtual void SetDescription(const wchar_t * description) override
        {
            _description = description;
        }

        virtual void * ValueInterface() override
        {
            return _value_interface;
        }

    protected:
        std::wstring _id;
        std::wstring _description;
        int _type;

        void * _value_interface;
        friend class VariableManager;
    };

}  // end Yap::_details

using namespace _details;

VariableManager::VariableManager() :
	_variables(YapShared(new ContainerImpl<IVariable>))
{
    InitTypes();
}

VariableManager::VariableManager(IVariableContainer * variables) :
	_variables(YapShared(variables))
{
    InitTypes();
}

VariableManager::VariableManager(const VariableManager& rhs) :
	_variables(YapShared(rhs.Variables()->Clone()))
{
    InitTypes();
}

VariableManager::~VariableManager()
{
}

bool VariableManager::InitTypes()
{
    _types.insert(make_pair(L"int", YapShared(new VariableImpl(VariableInt, L"int", nullptr))));
    _types.insert(make_pair(L"float", YapShared(new VariableImpl(VariableFloat, L"float", nullptr))));
    _types.insert(make_pair(L"string", YapShared(new VariableImpl(VariableString, L"string", nullptr))));
    _types.insert(make_pair(L"bool", YapShared(new VariableImpl(VariableBool, L"bool", nullptr))));

    return true;
}

using namespace _details;

bool VariableManager::Add(int type,
	const wchar_t * id,
	const wchar_t * description)
{
	try
	{
        auto new_variable = new VariableImpl(type, id, description);
        _variables->Add(id, new_variable);
		return true;
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

bool VariableManager::Add(const wchar_t * type,
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

bool VariableManager::Add(IVariable* variable)
{
    assert(variable != nullptr);
    return _variables->Add(variable->GetId(), variable);
}

IVariableContainer* VariableManager::Variables()
{
	return _variables.get();
}

const IVariableContainer* VariableManager::Variables() const
{
	return _variables.get();
}

IVariable * VariableManager::GetVariable(IVariableContainer * variables,
	const wchar_t * id,
	int type)
{
	assert(variables != nullptr);
	assert(id != nullptr);

	wstring variable_id{ id };
	auto dot_pos = variable_id.find_first_of(L'.');
	if (dot_pos != wstring::npos)
	{
		auto left_square_pos = variable_id.find_first_of(L'[');
		if (left_square_pos != wstring::npos && left_square_pos < dot_pos)
		{
			auto right_square_pos = variable_id.find_first_of(L']', left_square_pos);
			if (right_square_pos == wstring::npos || right_square_pos > dot_pos)
				throw VariableException(id, VariableException::InvalidId);

			auto array_variable = GetVariable(variables, variable_id.substr(0, left_square_pos).c_str(), VariableStructArray);
			if (array_variable == nullptr)
				throw VariableException(id, VariableException::VariableNotFound);

			auto index = _wtoi(variable_id.substr(left_square_pos + 1, right_square_pos - left_square_pos - 1).c_str());
			auto array_interface = reinterpret_cast<IArrayValue<IVariable*>*>(array_variable->ValueInterface());
			assert(array_interface != nullptr);

			if (index < 0 || index >= int(array_interface->GetSize()))
				throw VariableException(id, VariableException::OutOfRange);

			auto struct_variable = array_interface->Elements()[index];

			if (struct_variable == nullptr)
				throw VariableException(id, VariableException::VariableNotFound);

			if (struct_variable->GetType() != VariableStruct)
				throw VariableException(id, VariableException::NotAStruct);

			auto sub_variables = reinterpret_cast<IVariableContainer*>(struct_variable->ValueInterface());

			return GetVariable(sub_variables, variable_id.substr(dot_pos + 1).c_str(), type);
		}
		else
		{
			auto lhs = variable_id.substr(0, dot_pos); // left hand side of the dot operator.
			auto struct_variable = variables->Find(lhs.c_str());
			if (struct_variable == nullptr)
				throw VariableException(lhs.c_str(), VariableException::VariableNotFound);

			if (struct_variable->GetType() != VariableStruct)
				throw VariableException(lhs.c_str(), VariableException::NotAStruct);
			auto sub_variables = reinterpret_cast<IVariableContainer*>(struct_variable->ValueInterface());

			return GetVariable(sub_variables, variable_id.substr(dot_pos + 1).c_str(), type);
		}
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

IVariable * VariableManager::GetVariable(const wchar_t * id, int expected_type)
{
	assert(id != nullptr && id[0] != 0);
	return GetVariable(_variables.get(), id, expected_type);
}

shared_ptr<VariableManager> VariableManager::Load(const wchar_t * path)
{
    VdfParser parser;
    return parser.CompileFile(path);
}

void VariableManager::Reset()
{
	_variables = YapShared(new ContainerImpl<IVariable>);
}

bool VariableManager::TypeExists(const wchar_t * type) const
{
    return _types.find(type) != _types.end();
}

bool VariableManager::VariableExists(const wchar_t *variable_id) const
{
    auto This = const_cast<VariableManager*>(this);
    return This->_variables->Find(variable_id) != nullptr;
}

const IVariable * VariableManager::GetType(const wchar_t * type) const
{
    auto iter = _types.find(type);
    return (iter != _types.end()) ? iter->second.get() : nullptr;
}

bool VariableManager::AddType(const wchar_t * type_id, IContainer<IVariable> * variables)
{
    try
    {
        auto variable = new VariableImpl(VariableStruct, type_id, nullptr);
        variables->Lock();
        variable->_value_interface = variables;
        return AddType(type_id, variable);
    }
    catch(bad_alloc&)
    {
        return false;
    }
}

bool VariableManager::AddType(const wchar_t * type_id, IVariable *type)
{
    assert(_types.find(type_id) == _types.end());
    assert(type != nullptr);
    assert(type_id != nullptr);

    if (_types.find(type_id) != _types.end())
        return false;

    _types.insert({type_id, YapShared(type)});

    return true;
}

bool VariableManager::ResizeArray(const wchar_t * id, size_t size)
{
	auto array = GetVariable(id, VariableBoolArray | VariableFloatArray | VariableIntArray | VariableStructArray);
	if (array == nullptr)
		return false;

	assert(array->GetType() == VariableBoolArray || array->GetType() == VariableIntArray ||
		array->GetType() == VariableFloatArray || array->GetType() == VariableStructArray);
	auto array_interface = reinterpret_cast<IArray*>(array->ValueInterface());
	assert(array_interface != nullptr);
	array_interface->SetSize(size);

	return true;
}

}	// end Yap
