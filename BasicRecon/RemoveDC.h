#pragma once
#include "ProcessorImp.h"

namespace Yap
{
	class CRemoveDC :
		public CProcessorImp
	{
	public:
		CRemoveDC();
		~CRemoveDC();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		void RemoveDC(double * input_data, double * output_data, size_t width, size_t height, bool inplace, size_t corner_size)
		{
			
			assert(corner_size <= width / 2 && corner_size <= height / 2 && corner_size >= 2);
			double total = 0.0;
			for (unsigned int i = 0; i < corner_size; ++i)
			{
				for (unsigned int j = 0; j < corner_size; ++j)
				{
					total = total +
						*(input_data + width * i + j) +
						*(input_data + width * i + width - j - 1) +
						*(input_data + width * (height - i - 1) + j) +
						*(input_data + width * (height - i - 1) + width - j - 1);
				}
			}
			total /= corner_size * corner_size * 4;
			if (inplace)
			{
				for (unsigned int i = 0; i < width * height; ++i )
				{
					*(input_data + i) -= total;
				}
			}
			else
			{
				for (unsigned int i = 0; i < width * height; ++i)
				{
					*(output_data + i) = *(input_data + i) - total;
				}
			}

		};
	};
}

