#pragma once
#include "Interface\Implement\ProcessorImpl.h"

namespace Yap 
{
	namespace Direction
	{
		const unsigned int DirectionRight			= 0;
		const unsigned int DirectionDown			= 1;
		const unsigned int DirectionLeft			= 2;
		const unsigned int DirectionUp				= 3;
		const unsigned int DirectionRightDown		= 4;
		const unsigned int DirectionLeftDown		= 5;
		const unsigned int DirectionLeftUp			= 6;
		const unsigned int DirectionRightUp			= 7;
		const unsigned int DirectionRightAndDown	= 8;
		const unsigned int DirectionLeftAndDown		= 9;
		const unsigned int DirectionLeftAndUp		= 10;
		const unsigned int DirectionRightAndUp		= 11;
		const unsigned int DirectionAll				= 12;
	};

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
		
		void SetDirection(unsigned int direction);
		unsigned int GetDirection();

		void SetStepSize(unsigned int step_size);
		unsigned int GetStepSize();

		void SetGLCMSize(unsigned int glcm_size);
		unsigned int GetGLCMSize();

	private:
		void GLCM(unsigned int * input_data,  
			unsigned int * output_data, 
			unsigned int input_width, 
			unsigned int input_height);
		void Normalization(IData * data, unsigned int * out);	//the GLCM size or gray level number.

		void AddDirection(unsigned int * input_data,
			unsigned int * output_data,
			unsigned int input_width,
			unsigned int input_height,
			unsigned int out_size,
			int step_row,
			int step_column);

	private:
		unsigned int _glcm_size;
		unsigned int _direction;
		unsigned int _step_size;
	};
}

