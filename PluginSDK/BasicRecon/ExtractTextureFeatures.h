#pragma once
#include "Interface\Implement\ProcessorImpl.h"

namespace Yap
{
	class ExtractTextureFeatures :
		public ProcessorImpl
	{
	public:
		ExtractTextureFeatures();
		~ExtractTextureFeatures();

		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	};
}

