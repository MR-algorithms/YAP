#pragma once

#ifndef JpegExporter_h__20160814
#define JpegExporter_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	namespace details { class JpegExporterImp; }

	class JpegExporter :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(JpegExporter)
	public:
		JpegExporter();
		JpegExporter(const JpegExporter& rhs);
		
	protected:
		~JpegExporter();

		virtual bool Input(const wchar_t * name, IData * data) override;

		std::shared_ptr<details::JpegExporterImp> _impl;
	};
}

#endif // JpegExporter_h__
