#include "ReceiveData.h"

#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"
#include "Rawdata.h"
#include "QtUtilities\commonmethod.h"
#include "QtUtilities\DataWatch.h"
#include <QDebug>

using namespace Yap;
using namespace std;
using namespace Databin;

const bool cnstTEST = true;

ReceiveData::ReceiveData() : ProcessorImpl(L"ReceiveData")
{
    LOG_TRACE(L"<ReceiveData> constructor called.", L"ReconDemo");
    AddInput(L"Input",   YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
    AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);


}

ReceiveData::ReceiveData(const ReceiveData &rhs) :  ProcessorImpl(rhs)
{
    LOG_TRACE(L"<ReceiveData> copy constructor called.", L"ReconDemo");
}

bool ReceiveData::Input(const wchar_t * port, IData *data)
{
	
	if (wstring(port) != L"Input")
        return false;

    assert(data);
    assert(data->GetVariables() != nullptr);
    VariableSpace variables(data->GetVariables());
    ScanInfo::ScanSignal scan_signal = static_cast<ScanInfo::ScanSignal>( variables.Get<int>(L"scan_signal"));
    //qDebug()<<"scan_signal = "<<scan_signal;
    
    switch(scan_signal)
    {
	case ScanInfo::SSScanStart:
    {
        int scan_id = variables.Get<int>(L"scan_id");
        int channel_switch = variables.Get<int>(L"channel_switch");
        int freq_count = variables.Get<int>(L"freq_count");
        int phase_count = variables.Get<int>(L"phase_count");
        int slice_count = variables.Get<int>(L"slice_count");

        RawData::GetHandle().Prepare(scan_id, freq_count, phase_count, slice_count, channel_switch);
    }
        break;
	case ScanInfo::SSChannelPhaseStep:
    {
        assert(RawData::GetHandle().Ready());
        bool is_type_complexf = (Yap::GetDataArray<complex<float>>(data) != nullptr);
        assert(is_type_complexf);
		int scan_id = variables.Get<int>(L"scan_id");
		assert(scan_id == RawData::GetHandle().GetChannelInfo().scan_id);

        int freq_count;
        int phase_index;
        int slice_index;
        int channel_index;
		unsigned int i;
        for(i=0; i<data->GetDimensions()->GetDimensionCount(); i++)
        {
            unsigned int length;
            unsigned int start_index;
            DimensionType dimension_type;
            data->GetDimensions()->GetDimensionInfo(i, dimension_type, start_index, length);
			
            switch(dimension_type)
            {
            case Yap::DimensionReadout:
            {
				freq_count = length;
				assert(freq_count == RawData::GetHandle().GetChannelInfo().freq_count);
            }
                break;
            case Yap::DimensionPhaseEncoding:
            {
				assert(length == 1);
                phase_index = start_index;
                //qDebug()<<"---------scanning phase step =" <<phase_index << "**********";
            }
                break;
            case Yap::DimensionSlice:
            {
				assert(length == 1);
                slice_index = start_index;
            }
                break;
			case Yap::DimensionChannel:
			{
				assert(length == 1);
				channel_index = start_index;
			}

			default:
                break;
            }

        }
		assert(i == data->GetDimensions()->GetDimensionCount());
        //
        std::complex<float>* buffer = Yap::GetDataArray<std::complex<float>>(data);
        RawData::GetHandle().InsertPhasedata(buffer, freq_count, channel_index, slice_index, phase_index);
		//output:
		/*
		int num = RawData::GetHandle().GetNumofReadyphasesteps(channel_index);
		qDebug() << "Insert Phase: channel_index" << channel_index << "slice_index" << slice_index << "phase_index" << phase_index;
		if (!RawData::GetHandle().GetChannelInfo(channel_index).processed)
		{
			qDebug() << "-------------------channel_index" << channel_index << "Ready phase steps" << num;
		}
		*/
        std::complex<float> temp = buffer[3];

		Forward(data);

    }
        break;
	case ScanInfo::SSScanFinished:
    {

    }
        break;
    default:
        break;
    }

	return true;

}
void ReceiveData::Forward(Yap::IData *data)
{
	//---------
	int max_channelindex= RawData::GetHandle().MaxChannelIndex();
	for (int index = 0; index <= max_channelindex; index++)
	{
		int num = RawData::GetHandle().GetNumofReadyphasesteps(index);
		bool processed = RawData::GetHandle().GetChannelInfo(index).processed;
		if ( num%5==0 && !processed)
		{

			
			int freq_count;
			int phase_count;
			int slice_count;
			qDebug() << "--Forward.....reading buffer...";
			complex<float>* channel_raw_data = RawData::GetHandle().GetChannelRawdata(index, freq_count, phase_count, slice_count);

			if (channel_raw_data)
			{
				qDebug() << "--Forward----------channel_index" << index << "Ready phase steps" << num;
				auto output = CreateOutData(data, channel_raw_data, index, freq_count, phase_count, slice_count);
				//Log
				wstringstream wss;
				wss << L"Feed: channel index == " << index << L"\tPhase step counts = " << num;
				wstring ws;
				ws = wss.str(); //»ò ss>>strtEST;
				LOG_TRACE(ws.c_str(), L"ReconDemo");

				Feed(L"Output", output.get());
			}
		}
		else
		{
			RawData::GetHandle().SetChannelInfo(index, true);
		}
		
	}
}
void ReceiveData::Threadfunc_running()
{
    return;
}



bool ReceiveData::IsFinished(Yap::IData *data) const
{
    //
    //assert(data->GetVariables() != nullptr);
    //VariableSpace variables(data->GetVariables());
    //bool finished = variables.Get<bool>(L"Finished");
    //return finished;
    return true;

}

Yap::SmartPtr<Yap::IData>  ReceiveData::CreateOutData(Yap::IData* data, std::complex<float>* channel_raw_data, int channel_index,
                                int freq_count, int phase_count, int slice_count)
{
    unsigned int dim4 = 1;
    Dimensions dimensions;
    dimensions(DimensionReadout, 0U, freq_count)
        (DimensionPhaseEncoding, 0U, phase_count)
        (DimensionSlice, 0U, slice_count)
        (Dimension4, 0U, dim4)
        (DimensionChannel, channel_index, 1);
    //

    auto output = CreateData<complex<float>>(data, channel_raw_data, dimensions);
    
    LOG_TRACE(L"<ReceiveData> Feed channeldata.", L"ReconDemo");
    //
    //Log
    wstringstream wss;
    wss<<L"-------Feed : channel index == "<<channel_index;
    wstring ws ;
    ws=wss.str(); //»ò ss>>strtEST;
    LOG_TRACE(ws.c_str(), L"ReconDemo");
    //Out the specified channel rawdata.
    //

    if(false){
        for(int i=0; i <slice_count; i++)
        {
            wstringstream wss;
            wss<<L"d:\\temp";
            wstring wpath = wss.str();
            CDataWatch::ExportData(wpath.c_str(),
                                   channel_raw_data + channel_index*freq_count*phase_count*slice_count,
                                   256, 256, i);
        }
    }
    //

    return output;
}


ReceiveData::~ReceiveData()
{
    //LOG_TRACE(L"<ReceiveData>  destructor called.", L"ReconDemo");
}
