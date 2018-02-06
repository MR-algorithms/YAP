#pragma once
#ifndef simpleVariable_h__20180128
#define simpleVariable_h__20180128

#include "variableShared.h"
#include <type_traits>
#include <boost\lexical_cast.hpp>

namespace Yap
{
	namespace _details
	{
		template <typename TYPE>
		class SimpleVariable : public ISimpleVariable<TYPE>
		{
			static_assert(std::is_same<TYPE, int>::value ||
				std::is_same<TYPE, double>::value ||
				std::is_same<TYPE, bool>::value ||
				std::is_same<TYPE, std::wstring>::value,
				"You can only use one of the following types: int, double, bool, std::wstring");

			typedef typename variable_type_id<TYPE>::type type;

			IMPLEMENT_SHARED(SimpleVariable<TYPE>)
			IMPLEMENT_VARIABLE
		public:
			SimpleVariable(
				const wchar_t * id,
				const wchar_t * description) :
				_id{ id },
				_description{ description != nullptr ? description : L"" },
				_type{ variable_type_id<TYPE>::type_id },
				_value{ TYPE(0) }
			{
			}

			SimpleVariable(const SimpleVariable<TYPE>& rhs) :
				_id{ rhs._id },
				_description{ rhs._description },
				_type{ rhs._type },
				_value{ rhs._value },
				_enabled{rhs._enabled}
			{
			}

			SimpleVariable(ISimpleVariable<TYPE> & rhs) :
				_id{ rhs.GetId() },
				_description{ rhs.GetDescription() },
				_type{ rhs.GetType() },
				_value{ rhs.Get() }, 
				_enabled{rhs.IsEnabled()}
			{
			}

			virtual get_type Get() const override
			{
				return _value;
			}

			virtual void Set(set_type value) override
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

			virtual const wchar_t * const ToString() override
			{
				_value_string = boost::lexical_cast<std::wstring>(_value);
				return _value_string.c_str();
			}

		private:
			type _value;
			std::wstring _value_string;
		};

		// Begin specialization for string type.
		template <>
		class SimpleVariable<std::wstring> : public ISimpleVariable<std::wstring>
		{
			IMPLEMENT_SHARED(SimpleVariable<std::wstring>)
			IMPLEMENT_VARIABLE
		public:
			SimpleVariable(
				const wchar_t * id,
				const wchar_t * description) :
				_id{ id },
				_description{ description != nullptr ? description : L"" },
				_type{ VariableString },
				_value{ L"" }
			{
			}

			SimpleVariable(const SimpleVariable<std::wstring>& rhs) :
				_id{ rhs._id },
				_description{ rhs._description },
				_type{ rhs._type },
				_value{ rhs._value },
				_enabled{rhs._enabled}
			{
			}

			SimpleVariable(ISimpleVariable<std::wstring> & rhs) :
				_id{ rhs.GetId() },
				_description{ rhs.GetDescription() },
				_type{ rhs.GetType() },
				_value{ rhs.Get() }
			{
			}

			virtual const wchar_t * const Get() const override
			{
				return _value.c_str();
			}

			void Set(const wchar_t * const value)
			{
				_value = value;
			}

			const wchar_t * const ToString()
			{
				_value_string = L'\"';
				_value_string += _value + L'\"';
				return _value_string.c_str();
			}

			size_t FromString(const wchar_t * value_string)
			{
				assert(value_string != nullptr);
				_value_string = value_string;
				assert(!_value_string.empty());

				auto first_quote_pos = _value_string.find_first_not_of(L" \t\n\r");
				if (first_quote_pos == std::wstring::npos)
					return 0;

				if (_value_string[first_quote_pos] != L'\"')
					return 0;

				auto second_quote_pos = _value_string.find(L'\"', first_quote_pos + 1);
				if (second_quote_pos == std::wstring::npos)
					return 0;

				_value = _value_string.substr(first_quote_pos + 1, second_quote_pos - first_quote_pos - 1);
				return second_quote_pos + 1;
			}

		private:
			std::wstring _value;
			std::wstring _value_string;
		};
	}
}

#endif // simpleVariable_h__20180128