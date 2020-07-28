#include <cassert>
#include "StringHelper.h"
#include "QtUtilities\commonmethod.h"
#include "SampleDataServer.h"
#include "QtUtilities/FormatString.h"
#include "Processors/Rawdata.h"
#include <algorithm>
#include <qmessagebox>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <QDateTime>
#include <QFileInfo>
#include "Client/DataHelper.h"

#include <QApplication>

using namespace std;
using namespace Yap;
using namespace ScanInfo;

SampleDataServer SampleDataServer::s_instance;

SampleDataServer::SampleDataServer()
{

}
SampleDataServer& SampleDataServer::GetHandle()
{
    return s_instance;
}
SampleDataServer::~SampleDataServer()
{
}
bool SampleDataServer::OnDataParsing(DataPackage &package, int cmd_id)
{
    DebugInfo::Output(L"SampleDataServer::OnDataParsing()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);
    switch(cmd_id)
    {
    case SAMPLE_DATA_START:
    {
        SampleDataStart start;
        MessageProcess::Unpack(package, start);
        OnDataStart(start);

    }
        break;
    case SAMPLE_DATA_DATA:
    {
        SampleDataData data;
        MessageProcess::Unpack(package, data);
        OnDataData(data);

    }
        break;
    case SAMPLE_DATA_END:
    {
        SampleDataEnd end;
        MessageProcess::Unpack(package, end);
        OnDataEnd(end);

    }
        break;
    default:
        break;
    }
    return true;
}

bool SampleDataServer::OnDataStart(SampleDataStart &start)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    DebugInfo::Output(L"SampleDataServer::OnDataStart()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);

    _sample_start = start;
    ScanDimension scan_dim;
    scan_dim.channel_switch = _sample_start.channel_switch;
    scan_dim.scan_id = _sample_start.scan_id;
    scan_dim.freq_count = _sample_start.dim1_size;
    scan_dim.phase_count = _sample_start.dim2_size;
    scan_dim.slice_count = _sample_start.dim3_size;

    NotifyObserver(SSScanStart, scan_dim, shared_ptr<complex<float>>());

    return true;
}
#include <QDebug>
bool SampleDataServer::OnDataData(SampleDataData &data)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    //
    DebugInfo::Output(L"SampleDataServer::OnDataData()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);
    ScanInfo::ScanDimension scan_dim;
	scan_dim.scan_id = _sample_start.scan_id;
    scan_dim.channel_switch = _sample_start.channel_switch;
    scan_dim.freq_count = _sample_start.dim1_size;
    scan_dim.phase_count = _sample_start.dim2_size;
    scan_dim.slice_count = _sample_start.dim3_size;
    assert(scan_dim.phase_count == data.data_value.size());

    scan_dim.channel_index = data.rec;
    calculate_dimindex(_sample_start, data.dim23456_index,
                       scan_dim.phase_index, scan_dim.slice_index);

    qDebug()<<"allocate : phase_index = "<<scan_dim.phase_index;
    shared_ptr<complex<float>> buffer( new complex<float>[scan_dim.phase_count],
            [=](complex<float> *p) {
        qDebug()<<"DeAllocate : phase_index = " <<scan_dim.phase_index;
        delete[] p;
    } );
	/*
    for(int i = 0; i < scan_dim.phase_count; i ++)
    {
		*(buffer.get() + i) = complex<float>(i, 80);// data.data_value[i];
    }
	*/
    memcpy(buffer.get(), data.data_value.data(), scan_dim.phase_count * sizeof(complex<float>));

	complex<float>* p1 = buffer.get();

    NotifyObserver(SSChannelPhaseStep, scan_dim, buffer);
    //-
    return true;
}

bool SampleDataServer::OnDataEnd(SampleDataEnd &end)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    DebugInfo::Output(L"SampleDataServer::OnDataEnd()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);

	NotifyObserver(SSScanFinished, ScanInfo::ScanDimension(), shared_ptr<complex<float>>());
    return true;
}

void SampleDataServer::calculate_dimindex(SampleDataStart &start, int dim23456_index, int &phase_index, int &slice_index)
{

    //sending end: dim2 = phase_count, dim3 = slice_count;
    //dim4 maybe echo images or other else.
    int phase_count = start.dim2_size;
    int slice_count = start.dim3_size;
    int dim4_size = start.dim4_size;
    int dim5_size = start.dim5_size;
    int dim6_size = start.dim6_size;
    int dim23456_size = start.dim23456_size;


    assert(dim4_size == 1);
    assert(dim5_size == 1);
    assert(dim6_size == 1);
    assert(phase_count * slice_count == dim23456_size);

    //Decode the order of data received, according to the encoded order in the sender.

    CommonMethod::split_index(dim23456_index, phase_count, slice_count, phase_index, slice_index);

}

void SampleDataServer::NotifyObserver(ScanSignal scan_signal, const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer)
{
    _observer->OnData(scan_signal, scan_dim, buffer);
    return;
}


void SampleDataServer::SetObserver(std::shared_ptr<IDataObserver> observer)
{
    _observer = observer;
}
