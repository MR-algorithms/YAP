
#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class SubSampling :
		public ProcessorImpl
	{
	public:
		SubSampling(void);
		SubSampling(const SubSampling& rhs);

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		~SubSampling();

		SmartPtr<IData> _mask;

	};
}