#ifndef structVariable_h__20180128
#define structVariable_h__20180128

#include "variableShared.h"
#include "..\ContainerImpl.h"
#include "..\VariableTable.h"
#include <cassert>

namespace Yap
{
	namespace _details 
	{
		struct StructVariable : public IStructVariable
		{
			IMPLEMENT_SHARED2(StructVariable, IStructVariable)
			IMPLEMENT_VARIABLE_NO_ENABLE
		public:
			StructVariable(const wchar_t * id, const wchar_t * title = nullptr, const wchar_t * description = nullptr) :
				_id{ id },
				_title{ title != nullptr ? title : L"" },
				_description{ description != nullptr ? description : L"" },
				_type{ VariableStruct }
			{
			}

			StructVariable(const StructVariable& rhs) :
				_id{ rhs._id },
				_title{ rhs._title },
				_description{ rhs._description },
				_type{ rhs._type },
				_enabled{ rhs._enabled },
				_members{ rhs._members }
			{
			}

			StructVariable(IStructVariable& rhs) :
				_id{ rhs.GetId() },
				_title{ rhs.GetTitle() },
				_description{ rhs.GetDescription() },
				_type{ VariableStruct },
				_enabled{ rhs.IsEnabled() },
				_members{ rhs.Members() }
			{
			}

			StructVariable(IVariableContainer * variables, 
				const wchar_t * id, const wchar_t * title = nullptr, const wchar_t * description = nullptr) :
				_members{ variables },
				_id{ id },
				_title{ title != nullptr ? title : L"" },
				_description{ description != nullptr ? description : L"" },
				_enabled{ false },
				_type{ VariableStruct }
			{
			}

			virtual IVariableContainer * Members() override
			{
				return _members.Variables();
			}

			virtual void Enable(bool enable)
			{
				_enabled = enable;
				auto iter = _members.Variables()->GetIterator();
				for (auto member = iter->GetFirst(); member != nullptr; member = iter->GetNext())
				{
					member->Enable(enable);
				}
			}

			virtual bool IsEnabled() const override
			{
				return _enabled;
			}

			virtual const wchar_t * const ToString()
			{
				_value_string = L'{';

				auto iter = _members.Variables()->GetIterator();
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
					_value_string += L"\" : ";
					_value_string += member->ToString();
				}

				_value_string += L'}';

				return _value_string.c_str();
			}

			virtual size_t FromString(const wchar_t * value_string) override
			{
				assert(_members.Variables());
				_value_string = value_string;

				auto pos = _value_string.find_first_not_of(L" \t\n\r");
				if (pos == std::wstring::npos)
					return 0;

				if (_value_string[pos] != L'{')
					return 0;

				for (;;)
				{
					if (pos + 1 >= _value_string.size())
						return 0;

					pos = _value_string.find_first_not_of(L" \t\n\r", pos + 1);
					if (pos == std::wstring::npos || _value_string[pos] != L'\"')
						return 0;

					if (pos + 1 >= _value_string.size())
						return 0;

					auto quote_pos = _value_string.find(L'\"', pos + 1);
					if (quote_pos == std::wstring::npos)
						return 0;

					auto member_id = _value_string.substr(pos + 1, quote_pos - pos - 1);
					pos = quote_pos;
					if (pos + 1 > _value_string.size())
						return 0;

					pos = _value_string.find_first_not_of(L" \t\n\r", pos + 1);
					if (_value_string[pos] != L':')
						return 0;

					auto member = _members.Variables()->Find(member_id.c_str());
					auto char_consumed = member->FromString(_value_string.c_str() + pos + 1);
					if (char_consumed == 0)
						return 0;

					pos = pos + 1 + char_consumed;
					pos = _value_string.find_first_not_of(L" \t\n\r", pos);
					if (pos == std::wstring::npos)
						return 0;

					if (_value_string[pos] == L',')
					{
						if (pos + 1 >= _value_string.size())
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
			VariableTable _members;

			bool _enabled = false;
			std::wstring _value_string;
		};
	}
}
#endif // structVariable_h__20180128
