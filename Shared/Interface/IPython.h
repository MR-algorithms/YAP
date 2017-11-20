#ifndef _IPython_h__
#define _IPython_h__
#pragma once
#include <complex>

namespace Yap {
	namespace PyDataType {
		//dynamic python arguments type convert define.
		template<typename T> struct pydata_type_id { const char * type = ""; };
		// as below is implement struct pydata_type_id.
		template<> struct pydata_type_id<double> { const char * type = "d"; };
		template<> struct pydata_type_id<char> { const char * type = "s"; };
		template<> struct pydata_type_id<unsigned char> { const char* type = "b"; };
		template<> struct pydata_type_id<short> { const char * type = "h"; };
		template<> struct pydata_type_id<unsigned short> { const char * type = "h"; };
		template<> struct pydata_type_id<float> { const char * type = "f"; };
		template<> struct pydata_type_id<unsigned int> { const char * type = "i"; };
		template<> struct pydata_type_id<int> { const char * type = "i"; };
		template<> struct pydata_type_id<std::complex<float>> { const char * type = "O&"; };// convert to Python object. if error, return NULL.
		template<> struct pydata_type_id<std::complex<double>> { const char * type = "O&"; };
		template<> struct pydata_type_id<bool> { const char * type = "i"; };
	};

	struct IPython 
	{
		virtual void* Process2D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
			void * data, size_t width, size_t height, size_t& out_width, size_t& out_height) = 0;
		virtual void* Process3D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
			void * data, size_t width, size_t height, size_t slice, size_t& out_width,
			size_t& out_height, size_t& out_slice) = 0;
	};

};

#endif