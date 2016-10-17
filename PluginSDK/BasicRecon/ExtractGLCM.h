#pragma once
#include "Interface\Implement\ProcessorImpl.h"

namespace Yap 
{
	class ExtractGLCM :
		public ProcessorImpl
	{
	public:
		ExtractGLCM();
		ExtractGLCM(const ExtractGLCM & glcm);
		~ExtractGLCM();
	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
		IData * GLCM(IData * data);
	};

}

