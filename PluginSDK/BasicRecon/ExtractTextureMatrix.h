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

	class ExtractTextureMatrix :
		public ProcessorImpl
	{
	public:
		ExtractTextureMatrix();
		~ExtractTextureMatrix();

		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
		
		void SetDirection(unsigned int direction);
		unsigned int GetDirection();

		void SetStepSize(unsigned int step_size);
		unsigned int GetStepSize();

		void SetGrayLevel(unsigned int glcm_size);
		unsigned int GetGrayLevel();

		void SetGLSZMSize(unsigned int glszm_size);
		unsigned int GetGLSZMSize();

	private:
		// GLSZM refers to the Gray Level Size Zone Matrix 灰度区域大小矩阵
		void GLSZM(unsigned int * input_data,
			unsigned int * output_data,
			unsigned int input_width,
			unsigned int input_height);

		// GLCM refers to the Gray Level Coordinate Matrix 灰度共生矩阵
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
		int GetDirectionFromName(std::wstring name);

	private:
		unsigned int _gray_level;
		unsigned int _glszm_size;
		unsigned int _direction;
		unsigned int _step_size;
	};
}

