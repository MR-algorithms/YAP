#pragma once
#include "variableShared.h"
#include <type_traits>
#include <string>
#include <sstream>

namespace Yap 
{
	namespace _details
	{
		template <typename T>
		class ValueArray : public virtual IValueArray<T>
		{
			static_assert(std::is_same<T, int>::value ||
				std::is_same<T, double>::value ||
				std::is_same<T, bool>::value ||
				std::is_same<T, std::wstring>::value ||
				std::is_same<T, typename SmartPtr<IVariable>>::value,
				"You can only use one of the following types: int, double, bool, std::wstring, SmartPtr<IVariable>");

			IMPLEMENT_SHARED(ValueArray<T>)
			IMPLEMENT_VARIABLE

		public:
			ValueArray(size_t size, T value, const wchar_t * id, const wchar_t * title = nullptr, const wchar_t * description = nullptr) :
				_id{ id },
				_title{ title != nullptr ? description : L"" },
				_description{ description != nullptr ? description : L"" },
				_type{ variable_type_id<T>::array_type_id },
				_element_template{ Clone(value) }
			{
				_elements.reserve(size);
				for (size_t i = 0; i < size; ++i)
				{
					_elements.push_back(Clone(value));
				}
			}

			ValueArray(const ValueArray<T>& rhs) :
				_id{ rhs._id },
				_title{ rhs._title },
				_description{ rhs._description },
				_type{ rhs._type },
				_element_template{ Clone(rhs.GetElementTemplate()) }
			{
				_elements.reserve(rhs.GetSize());

				for (size_t i = 0; i < rhs.GetSize(); ++i)
				{
#pragma warning(disable:4800)
					_elements.push_back(Clone(rhs.Get(i)));
				}
			}

			ValueArray(IValueArray<T>& rhs) :
				_id{ rhs.GetId() },
				_title{ rhs.GetTitle() },
				_type{ variable_type_id<VALUE_TYPE>::array_type_id },
				_description{ rhs.GetDescription() },
				_element_template{ Clone(rhs.GetElementTemplate()) }
			{
				_elements.resize(rhs.GetSize());
				for (size_t i = 0; i < rhs.GetSize(); ++i) {
					_elements[i] = Clone(rhs.Get(i));
				}
			}

			virtual type GetElementTemplate() const
			{
				return _element_template;
			}

			virtual size_t GetSize() const override
			{
				return _elements.size();
			}

			virtual void SetSize(size_t size) override
			{
				_elements.resize(size);
			}

			virtual get_type Get(size_t index) const override
			{
				assert(index < _elements.size());
#pragma warning(disable:4800)	// casting from unsigned char to bool
				return _elements[index];
			}

			virtual void Set(size_t index, set_type value) override
			{
				assert(index < _elements.size());
				_elements[index] = value;
			}

			virtual const wchar_t * const ToString() override
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
#pragma warning(disable:4800)
					_value_string += ElementToString(element);
				}
				_value_string += L']';

				return _value_string.c_str();
			}

			T Clone(const T& source) const
			{
				return source;
			}

			virtual size_t FromString(const wchar_t * value_string) override
			{
				_elements.clear();
				_value_string = value_string;

				auto left_bracket_pos = _value_string.find_first_not_of(L" \t\n\r");
				if (left_bracket_pos == std::wstring::npos)
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
					if (separator_pos == std::wstring::npos)
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
				std::wistringstream input(value_string);
				input >> element;
				return static_cast<size_t>(input.tellg());
			}

			vector_type _elements;
			std::wstring _value_string;
			type _element_template;
		};

		// Specialization for ValueArrayVariable<wchar_t *>.

		template <>
		ValueArray<std::wstring>::get_type ValueArray<std::wstring>::Get(size_t index) const
		{
			assert(index < _elements.size());
			return _elements[index].c_str();
		}

		template <>
		void ValueArray<std::wstring>::Set(size_t index, ValueArray<std::wstring>::set_type value)
		{
			assert(index < _elements.size());
			_elements[index] = value;
		}

		template <>
		ValueArray<std::wstring>::type ValueArray<std::wstring>::GetElementTemplate() const
		{
			return _element_template.c_str();
		}

		template <>
		SmartPtr<IVariable> ValueArray<SmartPtr<IVariable>>::Get(size_t index) const
		{
			assert(index < _elements.size());
			auto This = const_cast<ValueArray<SmartPtr<IVariable>>*>(this);
			return This->_elements[index];
		}

		template <>
		void ValueArray<SmartPtr<IVariable>>::Set(size_t index, SmartPtr<IVariable> value)
		{
			assert(index < _elements.size());
			_elements[index] = value;
		}


		// Begin specialization for ValueArrayVariable<bool>.
		template <>
		std::wstring ValueArray<bool>::ElementToString(bool element)
		{
			return element ? L"true" : L"false";
		}

		template <>
		size_t ValueArray<bool>::ElementFromString(bool& element, const wchar_t * value_string)
		{
			std::wstring str;
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
		template<>
		SmartPtr<IVariable> ValueArray<SmartPtr<IVariable>>::Clone(const SmartPtr<IVariable>& source) const
		{
			return YapShared<IVariable>(source->Clone());
		}

		template <>
		ValueArray<SmartPtr<IVariable>>::ValueArray(size_t size,
			SmartPtr<IVariable> value, const wchar_t * id, const wchar_t * title, const wchar_t * description) :
			_id{ id == nullptr ? L"" : id },
			_title{ title == nullptr ? L"" : title },
			_description{ description == nullptr ? L"" : _description },
			_element_template{ YapShared(value->Clone()) }
		{
			assert(size >= 1 && "There should be at least one element in the array.");
			assert(value && "The template could not be null.");

			_type = variable_type_id<IVariable*>::array_type_id;

			_elements.resize(size);

			for (auto& element : _elements)
			{
				element = YapShared(value->Clone());
			}
		}

		template <>
		ValueArray<SmartPtr<IVariable>>::ValueArray(const ValueArray<SmartPtr<IVariable>>& rhs) :
			_id{ rhs._id },
			_title{ rhs._title },
			_description{ rhs._description },
			_element_template{ YapShared(rhs._element_template->Clone()) }
		{
			_type = rhs._type;

			_elements.reserve(rhs.GetSize());
			for (auto element : rhs._elements)
			{
				_elements.push_back(YapShared(element->Clone()));
			}
		}

		template <>
		std::wstring ValueArray<SmartPtr<IVariable>>::ElementToString(SmartPtr<IVariable> element)
		{
			return element->ToString();
		}

		template <>
		size_t ValueArray<SmartPtr<IVariable>>::ElementFromString(type& element, const wchar_t * value_string)
		{
			assert(_element_template);
			element = YapShared(_element_template->Clone());
			return element->FromString(value_string);
		}

		template <>
		void ValueArray<SmartPtr<IVariable>>::SetSize(size_t size)
		{
			size_t old_size = _elements.size();
			_elements.resize(size);
			for (size_t i = old_size; i < size; ++i)
			{
				_elements[i] = YapShared(_elements[0]->Clone());
			}
		}

		template <typename T>
		class ReferenceArray : public ValueArray<T>, public IElementReference<T>
		{
			IMPLEMENT_SHARED(ReferenceArray<T>)
		public:
			/// delegate constructors to base class ValueArrayVariable<T>.
			using ValueArray<T>::ValueArray;

			virtual T& Element(size_t index) override
			{
				assert(index < _elements.size());
				return _elements[index];
			}
		};

		template <>
		SmartPtr<IVariable>& ReferenceArray<SmartPtr<IVariable>>::Element(size_t index)
		{
			assert(index < _elements.size());
			return _elements[index];
		}

		template <typename T>
		class RawArray : public ReferenceArray<T>, public IRawArray<T>
		{
			IMPLEMENT_SHARED(RawArray<T>)
		public:
			using ReferenceArray<T>::ReferenceArray;

			virtual T * Data() override
			{
				return _elements.data();
			}
		};

	}
	
}
