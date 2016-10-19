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
		void GLCM(unsigned int * input_data,  unsigned int * output_data, unsigned int input_width, unsigned int input_height, unsigned int out_size);
		void Normalization(IData * data, unsigned int * out, unsigned int size);	//the GLCM size or gray level number.
	};

}

