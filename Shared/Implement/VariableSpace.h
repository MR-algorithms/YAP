#pragma once
#include "interface/interfaces.h"
#include "interface/smartptr.h"
#include "TypeManager.h"
#include "VariableTable.h"

namespace Yap {
	class VariableSpace : public VariableTable
	{
	public:
		VariableSpace();
		explicit VariableSpace(IVariableContainer * variables);
		VariableSpace(const VariableSpace& rhs);
		VariableSpace(VariableSpace&& rhs);

		~VariableSpace();

		const VariableSpace& operator = (const VariableSpace& rhs);
		const VariableSpace& operator = (VariableSpace&& rhs);

 		bool AddVariable(int type, const wchar_t * name, const wchar_t * description);
 		bool AddVariable(const wchar_t * type, const wchar_t * name, const wchar_t * description);

		/// Enable or disable a variable in variable space.
		bool Enable(const wchar_t * id, bool enable);

		/// Check to see if a variable is enabled.
		bool IsEnabled(const wchar_t * id) const;
		bool AddArray(const wchar_t * element_type_id,
			const wchar_t * id,
			const wchar_t * description);

		static std::shared_ptr<VariableSpace> Load(const wchar_t * path);

		void Reset();
		std::shared_ptr<TypeManager> GetTypes() {
			return _types;
		}

		void SetTypes(std::shared_ptr<TypeManager> types) {
			_types = types;
		}

	protected:

		std::shared_ptr<TypeManager> _types;
	};
}
