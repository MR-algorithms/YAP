#pragma once
#include "..\interfaces.h"
#include "..\smartptr.h"
#include <string>
#include <map>

namespace Yap {

    struct VariableException
    {
        enum Type
        {
            VariableNotFound,
            TypeNotMatch,
            InvalidType,
			OutOfRange,
			InvalidId,
			NotAStruct,
        };
        std::wstring variable_id;
        Type type;
        VariableException(const wchar_t * id, Type type_) : variable_id(id), type(type_) {}
    };

    class VariableManager
    {
    public:
        VariableManager();
        explicit VariableManager(IVariableContainer * variables);
        VariableManager(const VariableManager& rhs);

        ~VariableManager();

        bool Add(int type, const wchar_t * name, const wchar_t * description);
        bool Add(const wchar_t * type, const wchar_t * name, const wchar_t * description);
        bool Add(IVariable* variable);

        IVariableContainer * Variables();
        const IVariableContainer * Variables() const;

		template <typename T>
		T Get(const wchar_t * id) {
			std::wstring variable_id{ id };

			auto left_square_pos = variable_id.find_last_of(L'[');
			if (left_square_pos != wstring::npos) {
				return Element<T>(variable_id);
			}
			else {
				auto variable = GetVariable(id, variable_type_id<T>::type);
				assert(variable != nullptr && "If parameter not found, GetProperty() should throw an PropertyException.");
				assert(variable->ValueInterface() != nullptr);
				return reinterpret_cast<IValue<T>*>(variable->ValueInterface())->Get();
			}
		}

		template<typename T>
		void Set(const wchar_t * id, T value) {
			std::wstring variable_id{ id };

			auto left_square_pos = variable_id.find_last_of(L'[');
			if (left_square_pos != std::wstring::npos) {
				Element<T>(variable_id) = value;
			}
			else {
				auto variable = GetVariable(id, variable_type_id<T>::type);
				assert(variable != nullptr && "If parameter not found, GetProperty() should throw an PropertyException.");
				assert(variable->ValueInterface() != nullptr);
				reinterpret_cast<IValue<T>*>(variable->ValueInterface())->Set(value);
			}
		}
		
		bool ResizeArray(const wchar_t * id, size_t size);

		template<typename T>
		T* GetArray(const wchar_t * id) {
			auto array = GetVariable(id, variable_type_id<T>::array_type);
			if (array == nullptr)
				return nullptr;

			assert(array->GetType() == variable_type_id<T>::array_type);
			auto array_interface = reinterpret_cast<IArrayValue<T>*>(array->ValueInterface());
			assert(array_interface != nullptr);
			return array_interface->Elements();
		}

		static std::shared_ptr<VariableManager> Load(const wchar_t * path);

        void Reset();
        bool TypeExists(const wchar_t * type_id) const;
        bool VariableExists(const wchar_t * id) const;
        const IVariable * GetType(const wchar_t * type_id) const;
        bool AddType(const wchar_t * type_id, IVariable *type);
        bool AddType(const wchar_t * type_id, IContainer<IVariable> * variables);

    protected:

		template <typename T>
		T & Element(const std::wstring& id)
		{
			auto left_square_pos = id.find_last_of(L'[');
			assert(left_square_pos != std::wstring::npos);

			auto right_square_pos = id.find_last_of(L']');
			if (right_square_pos == std::wstring::npos)
				throw VariableException(id.c_str(), VariableException::VariableNotFound);

			auto array_variable = GetVariable(id.substr(0, left_square_pos).c_str(), variable_type_id<T>::array_type);
			if (array_variable == nullptr)
				throw VariableException(id.c_str(), VariableException::VariableNotFound);

			auto index = _wtoi(id.substr(left_square_pos + 1, right_square_pos - left_square_pos - 1).c_str());
			auto array_interface = reinterpret_cast<IArrayValue<T>*>(array_variable->ValueInterface());
			assert(array_interface != nullptr);

			if (index < 0 || index >= int(array_interface->GetSize()))
				throw VariableException(id.c_str(), VariableException::OutOfRange);

			return array_interface->Elements()[index];
		}

		bool InitTypes();
        IVariable * GetVariable(const wchar_t * name, int expected_type);
		IVariable * GetVariable(IVariableContainer * variables, const wchar_t * name, int type);
        SmartPtr<IVariableContainer> _variables;

        std::map<std::wstring, SmartPtr<IVariable>> _types;
    };
}
