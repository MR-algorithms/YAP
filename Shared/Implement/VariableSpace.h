#pragma once
#include "interface/interfaces.h"
#include "interface/smartptr.h"
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
			NotAnArray,
        };
        std::wstring variable_id;
        Type type;
        VariableException(const wchar_t * id, Type type_) : variable_id(id), type(type_) {}
    };

    class VariableSpace
    {
    public:
        VariableSpace();
        explicit VariableSpace(IVariableContainer * variables);
        VariableSpace(const VariableSpace& rhs);
		VariableSpace(VariableSpace&& rhs);

        ~VariableSpace();

		const VariableSpace& operator = (const VariableSpace& rhs);
		const VariableSpace& operator = (VariableSpace&& rhs);

		bool Add(int type, const wchar_t * name, const wchar_t * description);
        bool Add(const wchar_t * type, const wchar_t * name, const wchar_t * description);
        bool Add(IVariable* variable);

        IVariableContainer * Variables();
        const IVariableContainer * Variables() const;

		/// Enable or disable a variable in variable space.
		void Enable(const wchar_t * id, bool enable);

		/// Check to see if a variable is enabled.
		bool IsEnabled(const wchar_t * id) const;

		template <typename T>
		T Get(const wchar_t * id) {
			std::wstring variable_id{ id };

			if (variable_id[variable_id.size() - 1] == L']') {
				auto left_square_pos = variable_id.find_last_of(L'[');
                if (left_square_pos != std::wstring::npos) {
					return Element<T>(variable_id);
				}
				else {
					throw VariableException(id, VariableException::InvalidId);
				}
			}
			else {
				auto variable = GetVariable(id, variable_type_id<T>::type);
				assert(variable != nullptr && "If parameter not found, GetVariable() should throw an PropertyException.");
				auto simple_variable = dynamic_cast<ISimpleVariable<T>*>(variable);
				assert(simple_variable != nullptr);
				return simple_variable->Get();
			}
		}

		template<typename T>
		void Set(const wchar_t * id, T value) {
			std::wstring variable_id{ id };

			if (variable_id[variable_id.size() - 1] == L']') {
				auto left_square_pos = variable_id.find_last_of(L'[');
				if (left_square_pos != std::wstring::npos) {
					Element<T>(variable_id) = value;
				}
				else {
					throw VariableException(id, VariableException::InvalidId);
				}
			}
			else {
				auto variable = GetVariable(id, variable_type_id<T>::type);
				assert(variable != nullptr && "If parameter not found, GetProperty() should throw an PropertyException.");
				auto simple_variable = dynamic_cast<ISimpleVariable<T>*>(variable);
				assert(simple_variable != nullptr);
				simple_variable->Set(value);
			}
		}
		
		bool ResizeArray(const wchar_t * id, size_t size);

		template<typename T>
		T* GetArray(const wchar_t * id) {
			auto array = GetVariable(id, variable_type_id<T>::array_type);
			if (array == nullptr)
				return nullptr;

			assert(array->GetType() == variable_type_id<T>::array_type);
			auto array_variable = dynamic_cast<IArrayVariable<T>*>(array);
			assert(array_variable != nullptr);
			return array_variable->Elements();
		}

		template<typename T>
		std::pair<T*, size_t> GetArrayWithSize(const wchar_t * id)
		{
			auto array = GetVariable(id, variable_type_id<T>::array_type);
			if (array == nullptr)
				return std::make_pair(nullptr, 0);

			assert(array->GetType() == variable_type_id<T>::array_type);
			auto array_variable = dynamic_cast<IArrayVariable<T>*>(array);
			assert(array_variable != nullptr);
			return std::make_pair(array_variable->Elements(), array_variable->GetSize());
		}

		static std::shared_ptr<VariableSpace> Load(const wchar_t * path);

        void Reset();
        bool TypeExists(const wchar_t * type_id) const;
        bool VariableExists(const wchar_t * id) const;
        const IVariable * GetType(const wchar_t * type_id) const;
        bool AddType(const wchar_t * type_id, IVariable *type);
        bool AddType(const wchar_t * type_id, IPtrContainer<IVariable> * variables);

		IVariable * GetVariable(const wchar_t * name, int expected_type = VariableAllTypes);
	protected:

		template <typename T>
		T & Element(const std::wstring& id)
		{
			auto left_square_pos = id.find_last_of(L'[');
			assert(left_square_pos != std::wstring::npos);

			auto right_square_pos = id.find_last_of(L']');
			if (right_square_pos == std::wstring::npos)
				throw VariableException(id.c_str(), VariableException::VariableNotFound);

			auto variable = GetVariable(id.substr(0, left_square_pos).c_str(), variable_type_id<T>::array_type);
			if (variable == nullptr)
				throw VariableException(id.c_str(), VariableException::VariableNotFound);

			auto index = _wtoi(id.substr(left_square_pos + 1, right_square_pos - left_square_pos - 1).c_str());

			auto array_variable = dynamic_cast<IArrayVariable<T>*>(variable);
			assert(array_variable != nullptr);

			if (index < 0 || index >= int(array_variable->GetSize()))
				throw VariableException(id.c_str(), VariableException::OutOfRange);

			return array_variable->Elements()[index];
		}

		bool InitTypes();

		static IVariable * GetVariable(IVariableContainer * variables, const wchar_t * name, int type = VariableAllTypes);
        SmartPtr<IVariableContainer> _variables;

        std::map<std::wstring, SmartPtr<IVariable>> _types;
    };
}
