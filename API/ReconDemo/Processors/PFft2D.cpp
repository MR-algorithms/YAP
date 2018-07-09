#include "PFft2D.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>

using namespace std;
using namespace Yap;

PFft2D::PFft2D():
    ProcessorImpl(L"PFft2D"),
	_plan_data_width(0),
	_plan_data_height(0),
	_plan_inverse(false),
	_plan_in_place(false),
	_fft_plan(nullptr)
{
    AddProperty<bool>( L"Inverse", false, L"The direction of PFFT2D.");
    AddProperty<bool>( L"InPlace", true, L"The position of PFFT2D.");
    AddProperty<bool>(L"Asynchronous",true,L"Is PFft2D Asynchronous.");//添加属性Asynchronous表示该处理器是否需要异步

	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
}


PFft2D::PFft2D(const PFft2D& rhs)
	:ProcessorImpl(rhs),
	_plan_data_width(rhs._plan_data_width),
	_plan_data_height(rhs._plan_data_height),
	_plan_inverse(rhs._plan_inverse),
	_plan_in_place(rhs._plan_in_place),
	_fft_plan(rhs._fft_plan)
{
}

void PFft2D::fft2d_thread2(PFft2D *pFft2D, IData *myData)
{
    pFft2D->ToFft2D(myData);

    //pFft2D->ToFft2D(pFft2D->newIData);//将上面表达式换成这个，也可以运行

    /*
    DataHelper input_data(myData);
    auto width = input_data.GetWidth();
    auto height = input_data.GetHeight();
    auto data_array = Yap::GetDataArray<complex<float>>(myData);
    bool i=pFft2D->GetProperty<bool>(L"InPlace");
    int j=100;
    if (pFft2D->GetProperty<bool>(L"InPlace"))
    {
        pFft2D->Fft(data_array, data_array, width, height, pFft2D->GetProperty<bool>(L"Inverse"));
        pFft2D->Feed(L"Output", myData);

    }
    else
    {
        auto output = pFft2D->CreateData<complex<double>>(myData);
        pFft2D->Fft(data_array, GetDataArray<complex<float>>(output.get()),
            width, height, pFft2D->GetProperty<bool>(L"Inverse"));
        pFft2D->Feed(L"Output", output.get());
    }
    */
}

void PFft2D::fft2d_thread(PFft2D* pFft2D)
{
    int j=0;
    while (1)
    {
//        std::vector<Yap::IData*> myData=pFft2D->_allData;
//        for(int i=0;i<myData.size();i++)
//        {
//            pFft2D->ToFft2D(myData.at(i));
//        }

//        std::unique_lock<mutex> lck(pFft2D->dataCacheMutex);
//        Yap::IData* myData=pFft2D->newIData;
//        lck.unlock();
//        pFft2D->ToFft2D(myData);

        std::unique_lock<std::mutex> lock(gv_fft2d_repopsitory.gv_mtx);
        Yap::IData *myData=gv_fft2d_repopsitory.gv_data;
        lock.unlock();
        j++;
        //
        //VariableSpace variables(myData->GetVariables());
        //int slice_index = variables.Get<int>(L"slice_index1");
        //int channel_index = variables.Get<int>(L"channel_index1");
        //
        //::_sleep(500);
        qDebug()<<"**************************"<<j<<"******************************************";//<<channel_index<<"-----------------------------------"<<slice_index;
        //pFft2D->ToFft2D(myData);
        //DataHelper input_data(myData);
        //auto width = input_data.GetWidth();
       // auto height = input_data.GetHeight();

        //auto data_array1 = GetDataArray<complex<float>>(myData);

//        if (GetProperty<bool>(L"InPlace"))
//        {
            //LOG_TRACE(L"<PFft2D> Input::InPlace is true, before Fft().", L"BasicRecon");
           // Fft(data_array, data_array, width, height, GetProperty<bool>(L"Inverse"));
           // return Feed(L"Output", data);
//        }
//        else
//        {
//            auto output = CreateData<complex<double>>(data);

//            //LOG_TRACE(L"<PFft2D> Input::InPlace is false, create data, before Fft().", L"BasicRecon");

//            Fft(data_array, GetDataArray<complex<float>>(output.get()),
//                width, height, GetProperty<bool>(L"Inverse"));
//            return Feed(L"Output", output.get());
//        }

//        pFft2D->ToFft2D(myData);
//         ::_sleep(1000);
//        j++;
//        if(j>10)
//        {
//            break;
//        }
//        qDebug()<<"j===================================="<<j;
   }

//   j=0;

    //pFft2D->JoinMyThread();
}

void PFft2D::fft2d_thread0()
{
    while (1)
    {
        std::unique_lock<std::mutex> lock(gv_fft2d_repopsitory.gv_mtx);
        Yap::IData * myData=gv_fft2d_repopsitory.gv_data;
        PFft2D* pFft=gv_fft2d_repopsitory.gv_pFft2D;
         //auto myData=gv_fft2d_repopsitory.ptr_data.get();
     /* ////
        std::vector<std::complex<float> > slice_data=gv_fft2d_repopsitory.gv_slice_data;
        Dimensions dimensions;
                   dimensions(DimensionReadout, 0U, 512)
                       (DimensionPhaseEncoding, 0U, 512)
                       (DimensionSlice, 0U, 1);
                       //(Dimension4, 0U, dim4)
                       //(DimensionChannel, channel_index, 1);
         auto output = pFft->CreateData<complex<float>>(nullptr,  slice_data.data(), dimensions);
         Yap::VariableSpace variables;
                     output.get()->SetVariables(variables.Variables());

                     if (output->GetVariables() != nullptr)
                     {
                         VariableSpace variables(output->GetVariables());
                         variables.AddVariable(L"int",L"slice_index1",L"slice index1.");
                         variables.Set(L"slice_index1",gv_fft2d_repopsitory.index);

                     }
                     VariableSpace variables1(output->GetVariables());
                     int feed_slice_index=variables1.Get<int>(L"slice_index1");
                     Yap::IData * myData=output.get();

        ////*/

        //pFft->ToFft2D(myData);
        //lock.unlock();
        //Yap::IData * myData=pFft->newIData;
        DataHelper input_data(myData);
        auto width = input_data.GetWidth();
        auto height = input_data.GetHeight();

        auto data_array = Yap::GetDataArray<complex<float>>(myData);
        //complex<float>* data_array = gv_fft2d_repopsitory.array;
        bool i=pFft->GetProperty<bool>(L"InPlace");
        int j=std::thread::hardware_concurrency();
        if (pFft->GetProperty<bool>(L"InPlace"))
        {
            pFft->Fft(data_array, data_array, width, height, pFft->GetProperty<bool>(L"Inverse"));
            pFft->Feed(L"Output", myData);

        }
        else
        {
//            auto output = pFft->CreateData<complex<double>>(myData);
//            Fft(data_array, GetDataArray<complex<float>>(output.get()),
//                width, height, pFft->GetProperty<bool>(L"Inverse"));
//            Feed(L"Output", output.get());
        }
        lock.unlock();
    }
}

void PFft2D::ReceiveDataFromOtherThread(IData * datain,PFft2D* p)
{
    //std::unique_lock<mutex> lck(dataCacheMutex);
    //newIData=datain;
    //DataHelper input_data0(datain);
    std::complex<float> * data_array = GetDataArray<complex<float>>(datain);
    //const std::complex<float> * buff = data_array;
    VariableSpace variables(datain->GetVariables());
    int slice_index = variables.Get<int>(L"slice_index1");
    int channel_index = variables.Get<int>(L"channel_index1");
    DataHelper input_data(datain);
    auto width = input_data.GetWidth();
    auto height = input_data.GetHeight();

    std::unique_lock<std::mutex> lock(gv_fft2d_repopsitory.gv_mtx);//
    gv_fft2d_repopsitory.gv_slice_data.resize(width*height);
    memcpy(gv_fft2d_repopsitory.gv_slice_data.data(),data_array, width * height * sizeof(std::complex<float>));
    auto temp1=LookintoPtr(gv_fft2d_repopsitory.gv_slice_data.data(),width * height,102400,102400+1024);
    auto temp2=LookintoPtr(data_array,width * height,102400,102400+1024);
    gv_fft2d_repopsitory.index=slice_index;

    gv_fft2d_repopsitory.gv_data=datain;//
    gv_fft2d_repopsitory.gv_pFft2D=p;//
    gv_fft2d_repopsitory.array=data_array;
    //gv_fft2d_repopsitory.gv_data_helper=input_data0;
    //gv_fft2d_repopsitory.idata.push_back(datain);
    //gv_fft2d_repopsitory.ptr_data=shared_ptr<IData>(datain);
    //VariableSpace variables(gv_fft2d_repopsitory.gv_data->GetVariables());
    //int slice_index = variables.Get<int>(L"slice_index1");
    //int channel_index = variables.Get<int>(L"channel_index1");
    //qDebug()<<"+++++++++++++++++++++++"<<channel_index<<"+++++++++++++++++++++++++++"<<slice_index;
    lock.unlock();

}


PFft2D::~PFft2D()
{
    //JoinMyThread();
}

bool PFft2D::Input(const wchar_t * port, IData * data)
{
    if (wstring(port) != L"Input")
	{
        //LOG_ERROR(L"<PFft2D> Error input port name!", L"BasicRecon");
		return false;
	}
    if(!CheckIData(data))
    {
        return false;
    }
    //LOG_TRACE(L"<PFft2D> Input::After Check.", L"BasicRecon");
    if(GetProperty<bool>(L"Asynchronous"))//如果需要异步则创建一个子线程
    {
        ReceiveDataFromOtherThread(data,this);
        _count++;
////        if(_allData.size()>=64)
////        {
////            _allData.clear();
////        }
////        _allData.push_back(data);

//        if(_count==1)
//        {

//            _mythread=std::thread(fft2d_thread0);//因为IData *的易变，无法传入全局结构体变量并读取其所指向的对象

//        }


      //   std::thread _fft_thread(fft2d_thread2,this,data);//可以运行，只是每次都创建于销毁线程导致耗时


         Scoped_thread threadGuard(std::thread(fft2d_thread2,this,data));

         //_fft_thread.detach();
//         if(_fft_thread.joinable())
//         {
//             _fft_thread.join();
//             //_fft_thread.detach();

//         }

    }
    else
    {
        return ToFft2D(data);
    }
    return true;

}

bool PFft2D::CheckIData(IData *data)
{
    // Do some check.
    if (data == nullptr)
    {
        //LOG_ERROR(L"<PFft2D> Invalid input data!", L"BasicRecon");
        return false;
    }
    DataHelper input_data(data);
    if (input_data.GetActualDimensionCount() != 2)
    {
        //LOG_ERROR(L"<PFft2D> Error input data dimention!(2D data is available)!", L"BasicRecon");
        return false;
    }
    if (input_data.GetDataType() != DataTypeComplexFloat)
    {
        //LOG_ERROR(L"<PFft2D> Error input data type!(DataTypeComplexFloat is available)!", L"BasicRecon");
        return false;
    }
    return true;
}

bool PFft2D::ToFft2D(IData *data)
{
    DataHelper input_data(data);
    auto width = input_data.GetWidth();
    auto height = input_data.GetHeight();

    auto data_array = GetDataArray<complex<float>>(data);

    if (GetProperty<bool>(L"InPlace"))
    {
        //LOG_TRACE(L"<PFft2D> Input::InPlace is true, before Fft().", L"BasicRecon");
        Fft(data_array, data_array, width, height, GetProperty<bool>(L"Inverse"));
        return Feed(L"Output", data);
    }
    else
    {
        auto output = CreateData<complex<double>>(data);

        //LOG_TRACE(L"<PFft2D> Input::InPlace is false, create data, before Fft().", L"BasicRecon");

        Fft(data_array, GetDataArray<complex<float>>(output.get()),
            width, height, GetProperty<bool>(L"Inverse"));
        return Feed(L"Output", output.get());
    }
}

void PFft2D::FftShift(std::complex<float>* data, size_t  width, size_t height)
{
    //LOG_TRACE(L"<PFft2D> FftShift::Before SwapBlock().", L"BasicRecon");
	SwapBlock(data, data + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width, width / 2, height / 2, width);
    //LOG_TRACE(L"<PFft2D> FftShift::After SwapBlock().", L"BasicRecon");
}

void PFft2D::SwapBlock(std::complex<float>* block1, std::complex<float>* block2, size_t width, size_t height, size_t line_stride)
{
	std::vector<std::complex<float>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;
    //LOG_TRACE(L"<PFft2D> SwapBlock::Before memcpy().", L"BasicRecon");
	for (unsigned int row = 0; row < height; ++row)
	{
		memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<float>));
		memcpy(cursor1, cursor2, width * sizeof(std::complex<float>));
		memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<float>));

		cursor1 += line_stride;
		cursor2 += line_stride;
	}
    //LOG_TRACE(L"<PFft2D> SwapBlock::After memcpy().", L"BasicRecon");
}

bool PFft2D::Fft(std::complex<float> * data, std::complex<float> * result_data, size_t width, size_t height, bool inverse)
{
	bool in_place = (data == result_data);

    //LOG_TRACE(L"<PFft2D> Fft::Before Plan().", L"BasicRecon");
	if (width != _plan_data_width || height != _plan_data_height || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan(width, height, inverse, in_place);
	}
    //LOG_TRACE(L"<PFft2D> Fft::After Plan(), before fftwf_execute_dft().", L"BasicRecon");
	fftwf_execute_dft(_fft_plan, (fftwf_complex*)data, (fftwf_complex*)result_data);
    //LOG_TRACE(L"<PFft2D> Fft::After fftwf_execute_dft().", L"BasicRecon");

	for (auto data = result_data; data < result_data + width * height; ++data)
	{
		*data /=  float(sqrt(width * height));
	}

    //LOG_TRACE(L"<PFft2D> Fft::Before FftShift().", L"BasicRecon");
	FftShift(result_data, width, height);
    //LOG_TRACE(L"<PFft2D> Fft::After FftShift().", L"BasicRecon");
	return true;
}

void PFft2D::Plan(size_t width, size_t height, bool inverse, bool in_place)
{
	vector<fftwf_complex> data(width * height);

	if (in_place)
	{
		_fft_plan = fftwf_plan_dft_2d(int(height), int(width), (fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_UNALIGNED);
	}
	else
	{
		vector<fftwf_complex> result(width * height);
		_fft_plan = fftwf_plan_dft_2d(int(height), int(width),  (fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_UNALIGNED);
	}

	_plan_data_width = static_cast<unsigned int> (width);
	_plan_data_height = static_cast<unsigned int> (height);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}
std::vector<std::complex<float> > PFft2D::LookintoPtr(std::complex<float> *data, int size, int start, int end)
{
        std::vector<std::complex<float>> view_data;
        int view_size = end - start;

        assert(start < end);
        assert(end <= size);

        view_data.resize(view_size);
        memcpy(view_data.data(), data + start, view_size * sizeof(std::complex<float>));
        return view_data;
}

