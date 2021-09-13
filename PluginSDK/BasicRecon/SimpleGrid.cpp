#include "stdafx.h"
#include "SimpleGrid.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>
#include <cassert>

using namespace std;
using namespace Yap;
const double PI = 3.1415926535897932384626433832795028841971; 

SimpleGrid::SimpleGrid() : ProcessorImpl(L"SimpleGrid")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	
	AddProperty<bool>(L"OnlyKPosition", false, L"Only k position.");
	AddProperty<bool>(L"RadialData", true, L"Radial Data.");
	AddProperty<int>(L"DestWidth", 0, L"Destination width.");
	AddProperty<int>(L"DestHeight", 0, L"Destination height.");


}

SimpleGrid::SimpleGrid(const SimpleGrid& rhs)
	:ProcessorImpl(rhs)
{
}

SimpleGrid::~SimpleGrid()
{
}
/**
	\remark backup function.
*/

/**
	\note 
	
*/
//���ԣ���������׼k�ռ����ݿ��ɾ���ɼ����ݣ�����ɼ�����Ϣ���ɼ����غͲɼ��Ƕȣ���¼��ĩ���ϡ�
void SimpleGrid::TestSetup(IData * data)
{
	assert((data != nullptr) && 
		(Yap::GetDataArray<complex<float>>(data) != nullptr) );
	
	DataHelper helper(data);

	int width = helper.GetWidth();
	int height = helper.GetHeight();
	unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);

	Dimension slice_dimension = helper.GetDimension(DimensionSlice);
	//----------
	//
	bool LinearRadial = true;
	float delta_angle = LinearRadial ? PI / height : (111.25 / 180 * PI);
	int center_line_index = height / 2; 

	//
	for (unsigned int i = slice_dimension.start_index; i < slice_dimension.start_index + slice_dimension.length; ++i)
	{
		float angles[256];
		if (helper.GetDataType() == DataTypeComplexFloat)
		{
			complex<float> *slice = Yap::GetDataArray<complex<float>>(data) +
				(i - slice_dimension.start_index) * slice_block_size;
			//
			int interleaves = 1;
			int total_turns = height / interleaves;
			//���ԣ������ֲ��ֲɼ��ߡ�
			for (int j = 0; j < height; j++)
			{
				*(slice + j * width + width - 1) = complex<float>(0,0);
			}
		
			for (int j = 0; j < height/3; j++)
			{
				//
				int turn_index = j % total_turns;
				//�ٶ�totoal_turns = 10, interleaves =4; ��ô��
				//j = 2: turns_index = 2; n0 = 0;
				//j = 12: turn_index = 2; n0 = 1;
				int n0 = j / total_turns;
				//�²ⲽ��2��
				float angle1 = (turn_index * interleaves + n0) * delta_angle;
				//�²ⲽ��3���ٶ������Ƕȴ� pi / 2��-pi / 2��
				float angle = (PI / 2 - angle1);
			
				*(slice + j * width + width - 1) = complex<float>(1.0, angle);


			}
			int x = 3;
			
		}
		else
		{
			assert(0 && L"Not Implemented.");
		}

	}

}
//��������Ϊר�ô����������־���ɼ����ݣ��������񻯴�������
//1��dim1: columns,��ʾÿ�����ϵ����ݣ����һ�����ݣ�ʵ����ʾ�Ƿ�ɼ����鲿��ʾ�ɼ���k�ռ�Ƕȣ�
//2, �����ռ������ݣ�
//--radial_scan: ��ʾ�Ƿ�Ϊ����ɼ����ݡ�
//--center_line_index: �м�����������ʾ��Ӧky=0�ľ���������
//--cneter_column_index���м����������ʾ��Ӧk�ռ�ԭ��ľ������ϵ������㡣
//
//--��Ϊÿ�����ϵ�ĩ���������Ƿ�ɼ��ͽǶ���Ϣ����ʵ�����������Ϣ��Yap�������ݲ���û�������Ϣ��


bool SimpleGrid::Input(const wchar_t * port, IData * data)
{
	
	if (std::wstring(port) != L"Input")
	{
		LOG_ERROR(L"<SimpleGrid> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper helper(data);
	if (helper.GetDataType() != DataTypeComplexDouble && helper.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<SimpleGrid> Error input data type!(DataTypeComplexDouble and DataTypeComplexFloat are available)", L"BasicRecon");
		return false;
	}

	Dimensions data_dimensions(data->GetDimensions());
	//�������þ���ɼ���Ϣ������Yap��ֱ�Ӽ������ݲ��ԡ�
	if (!GetProperty<bool>(L"RadialData"))
	{
		TestSetup(data);
		assert(256 == helper.GetWidth());//Yap2�ڲ����������ݣ�
	}
	//����Դ�Ǿ������ݣ�
	int radial_columns = helper.GetWidth();
	int line_count = helper.GetHeight();
	int src_depth = helper.GetSliceCount();

	bool OnlyKPosition(GetProperty<bool>(L"OnlyKPosition"));
	
	//Grid parameters:
	//0. ������֪�������ع�k�ռ䡣
	//1. delta_k: ȱʡֵ2����ʾ��������k�ռ��в����ļ�ࣻ
	//2. width, height--k�ռ����ݵĿ��Ⱥ͸߶ȣ����Ѿ��ľ����߾����������ߵ���������λ�ã������߼��
	//��������һ����Լ����ϵ��
	
	int center_column_index = radial_columns / 2;
	int center_part_index = src_depth / 2;    // 21; //start from 0, slice direction, used in 3D FFT(?)
		
	int dest_width(GetProperty<int>(L"DestWidth")); 
	int dest_height(GetProperty<int>(L"DestHeight")); 
	int dest_depth = src_depth;
	assert(dest_width == dest_height);
	assert(dest_width > 0);
	float delta_k = static_cast<float>(dest_width) / static_cast<float>(radial_columns);
	
	
	//
	Yap::Dimensions dims;
	dims(DimensionReadout, 0, dest_width)
		(DimensionPhaseEncoding, 0, dest_height);
	Dimensions output_dimensions(data->GetDimensions());
	output_dimensions.Combine(&dims);
	//----
		
	if (data->GetDataType() == DataTypeComplexDouble)
	{
		assert(0 && L"Not Implemented!");
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
	
		for (int slice_index = 0; slice_index < dest_depth; ++slice_index)
		{
			std::complex<float>* slice =
				Yap::GetDataArray<complex<float>>(output.get())
				+ dest_width*dest_height * slice_index;
			if (1)
			{
				float angles[256];

				for (int line_index = 0; line_index < line_count; ++line_index)
				{
					std::complex<float>* radial =
						Yap::GetDataArray<complex<float>>(data)
						+ radial_columns * line_count * slice_index
						+ radial_columns * line_index;

					complex<float> temp = *(radial + radial_columns - 1);
					bool gate = fabs(temp.real()) > 0.5;
					
					if (gate) {
						float angle = temp.imag();
						angles[line_index] = angle;
						if (slice_index == 0)
						{
							//wstring info = wstring(L"<SimpleGrid>") + L"::Input "
							//	+ L"----slice_index = " + to_wstring(slice_index)
							//	+ L"----line_index = " + to_wstring(line_index)
							//	+ L"----line_angle = " + to_wstring(angle);
							//LOG_TRACE(info.c_str(), L"BasicRecon");
						}

						FillKSpace(slice, dest_width, dest_height,
							radial, radial_columns - 1, angle, center_column_index, delta_k, OnlyKPosition);
					}
				}

			}
			else {
				//test.ȷ��k�ռ�������ͼ��Ĺ�ϵ��
				//ͼ�����Ͻ�Ϊͼ��������ԭ�㡣
				//ͨ����k�ռ����ݴ�ŷ�ʽ����������ʽ��أ��ǣ�������������
				for (int i = 0; i < dest_width*0.75; i++)
				{
					*(slice + 10 * dest_width + i) = complex<float>(3, 4);
					*(slice + 100 * dest_width + i) = complex<float>(3, 4);
				}
				
			}
		}

		{//log

			wstring info = wstring(L"<SimpleGrid>") + L"::Feed "
				+ L"----columns = " + to_wstring(radial_columns - 1)
				+ L"----line count = " + to_wstring(line_count)
				+ L"----slice count = " + to_wstring(src_depth);
			LOG_TRACE(info.c_str(), L"BasicRecon");
		}
		return Feed(L"Output", output.get());
	}
}
//���ܣ����ݾ������ݣ����������ϵ�k�ռ����ݡ�ע�⣺
//1, �Ƕȣ������յ�����Ƕȵģ��磺kx,kyΪ��ֵ���Ƕ�Ϊ��һ���޽ǣ�k�ռ�λ�ô�
//�������޲�������һ����
//2, center:�ز����ĵ�����--�õ��k�ռ�ԭ���غϣ�
//3, �ٶ�k�ռ�ԭ���Ӧ��������width/2��height/2

void SimpleGrid::FillKSpace(complex<float> * slice, int width, int height,
	complex<float> * radial, int columns, float angle, float center, float delta_k, bool OnlyKPosition)
{
	float tempx = floor(-5.5);
	for (int i = 0; i < columns; i++)
	{
		//����ɼ��ߵĽǶ�����k�ռ侶�����ϵ�ĩ������ģ����ԣ�
		//--�����angleΪ���ʱ���ӵ�����������һ���޲�����
		//--�����angleΪ-40��ʱ���ӵڶ��������������޲�����
		float r = (i - center)*delta_k;
		float x, y;//x,y��ʾk�ռ�����
		//rֵ�Ӹ�ֵ��ʼ��������ʵ���ϲ��ü������㷨��rӦΪ�Ǹ�ֵ����ֵʵ�ʱ�ʾ�ڶ������ޣ�Ӧ�ýǶ����ʵ�������
		if (r > 0)
		{
			x = r * cos(angle);
			y = r * sin(angle);
		}
		else
		{
			x = -r * cos(angle+PI);
			y = -r * sin(angle+PI);
		}
		//ת���������������ꡣ
		int x1 = floor(x + 0.001);
		int y1 = floor(y + 0.001);

		int x_index =  x1 + width / 2;
		int y_index =  y1 + height / 2;
		x_index = x_index >= 0 ? x_index : 0;
		y_index = y_index >= 0 ? y_index : 0;

		x_index = x_index >= width-1 ? (width - 1) : x_index;
		y_index = y_index >= height-1 ? (height - 1) : y_index;

		*(slice + y_index * width + x_index) = OnlyKPosition? complex<float>(100,100): *(radial + i);//note: complex data.
		//����������ÿ�����ݶ������Χ�����㡣
		/*
		{//2
			int x1a = x1 + 1;
			int y1a = y1;
			int x_index = x1a + width / 2;
			int y_index = y1a + height / 2;
			x_index = x_index >= 0 ? x_index : 0;
			y_index = y_index >= 0 ? y_index : 0;

			x_index = x_index >= width - 1 ? (width - 1) : x_index;
			y_index = y_index >= height - 1 ? (height - 1) : y_index;

			*(slice + y_index * width + x_index) = OnlyKPosition ? complex<float>(100, 100) : *(radial + i);//note: complex data.
		}
		{//3
			int x1a = x1;
			int y1a = y1+1;
			int x_index = x1a + width / 2;
			int y_index = y1a + height / 2;
			x_index = x_index >= 0 ? x_index : 0;
			y_index = y_index >= 0 ? y_index : 0;

			x_index = x_index >= width - 1 ? (width - 1) : x_index;
			y_index = y_index >= height - 1 ? (height - 1) : y_index;

			*(slice + y_index * width + x_index) = OnlyKPosition ? complex<float>(100, 100) : *(radial + i);//note: complex data.
		}
		{//4
			int x1a = x1 + 1;
			int y1a = y1 + 1;
			int x_index = x1a + width / 2;
			int y_index = y1a + height / 2;
			x_index = x_index >= 0 ? x_index : 0;
			y_index = y_index >= 0 ? y_index : 0;

			x_index = x_index >= width - 1 ? (width - 1) : x_index;
			y_index = y_index >= height - 1 ? (height - 1) : y_index;

			*(slice + y_index * width + x_index) = OnlyKPosition ? complex<float>(100, 100) : *(radial + i);//note: complex data.
		}
		*/
	}
	int x = 3;
	return;
}