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

		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
	private:
		IData * GLCM(IData * data, unsigned int size);
		Yap::SmartPtr<IData> Normalization(IData * data, unsigned int size);	//the GLCM size or gray level number.
	};

}

