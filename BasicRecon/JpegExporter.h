#pragma once


#include "ProcessorImp.h"

namespace Yap
{
	namespace Implementation { class CJpegExporterImp; }

	class CJpegExporter :
		public CProcessorImp
	{
	public:
		CJpegExporter();
		CJpegExporter(const CJpegExporter& rhs);
		~CJpegExporter();
	protected:
		virtual bool OnInit() override;
		virtual IProcessor* Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

		std::shared_ptr<Implementation::CJpegExporterImp> _impl;
	};


}
