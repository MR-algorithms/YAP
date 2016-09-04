#pragma once

#ifndef JpegExporter_h__20160814
#define JpegExporter_h__20160814

#include "../../Shared/Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	namespace Implementation { class CJpegExporterImp; }

	class CJpegExporter :
		public ProcessorImpl
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

#endif // JpegExporter_h__
