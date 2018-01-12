#pragma once
#ifndef Radiomics_h__20171223
#define Radiomics_h__20171223

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class Radiomics :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Radiomics)
	public:

		explicit Radiomics();

		Radiomics(const Radiomics& rhs);

		virtual bool Input(const wchar_t * port, IData * data) override;

	private:
		~Radiomics();

		Dimensions CreateDimension(size_t output_dims, size_t * output_size);

		bool FeedOut(void*& out_data, Dimensions dimensions, int data_type);

		void* Run(int data_type, const wchar_t* script, const wchar_t* method,
			int input_dims, IData * data, size_t& output_dims, size_t * input_size,
			size_t * output_size,bool is_need_ref = true);

		SmartPtr<IData> _ref_data;
	};

}
#endif