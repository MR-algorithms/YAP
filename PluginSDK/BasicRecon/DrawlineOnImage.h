#pragma once
#ifndef DrawlineOnImage_h__20160814
#define DrawlineOnImage_h__20160814

#include "Implement/ProcessorImpl.h"
#include <boost/shared_array.hpp>
#include <complex>

namespace Yap
{
	template <typename T>
		struct SimplePoint
	{
		T x;
		T y;
		SimplePoint(T _x, T _y) { x = _x; y = _y; }
		~SimplePoint() {};
	};

	class DrawlineOnImage :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(DrawlineOnImage)
	public:
		DrawlineOnImage();
		DrawlineOnImage(const DrawlineOnImage& rhs);

	protected:
		~DrawlineOnImage();

		virtual bool Input(const wchar_t * port, IData * data) override;
		template <typename IN_TYPE>
		void Drawline(IN_TYPE* input_data2D, unsigned int width, unsigned int height,
			double* line1D, unsigned int size, double scaley);
		template <typename IN_TYPE>
		void SetPixel(IN_TYPE* input_data2D, unsigned int width, unsigned int height, unsigned int x, unsigned int y, IN_TYPE value);
		template <typename IN_TYPE>
		void LineTo(IN_TYPE* input_data2D, unsigned int width, unsigned int height, int x1, int y1, int x2, int y2);

	private:
		//SmartPtr<IData> _mask;
		boost::shared_array<double> GetTestline(int size);
		

	};
}

#endif //DrawlineOnImage_h__