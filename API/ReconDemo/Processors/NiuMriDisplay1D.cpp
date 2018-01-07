#include "NiuMriDisplay1D.h"

#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"
#include "plot1d.h"

using namespace Yap;
using namespace std;

NiuMriDisplay1D::NiuMriDisplay1D(Plot1D& display_plot) :
    ProcessorImpl(L"NiuMriDisplay1D"),
    _display_plot(display_plot)
{
    LOG_TRACE(L"NiuMriDisplay1D constructor called.", L"NiuMri");
    AddInput(L"Input", 1, DataTypeComplexFloat | DataTypeFloat);

    _display_plot.setRenderHint(QPainter::Antialiasing);
}

NiuMriDisplay1D::NiuMriDisplay1D(const NiuMriDisplay1D &rhs) :
    ProcessorImpl(rhs),
    _display_plot(rhs._display_plot)
{
    LOG_TRACE(L"NiuMriDisplay1D constructor called.", L"NiuMri");
}

bool NiuMriDisplay1D::Input(const wchar_t *port, Yap::IData *data)
{
    if (wstring(port) != L"Input")
        return false;

    DataHelper input_data(data);
    if (input_data.GetActualDimensionCount() > 1 )
        return false;

    if (input_data.GetDataType() == DataTypeComplexFloat)
    {
        auto data_array = GetDataArray<complex<float>>(data);
        auto size = input_data.GetDataSize();
        vector<complex<float>> data_vector(data_array, data_array + size);
        return _display_plot.SetData(data_vector);
    }
    else if (input_data.GetDataType() == DataTypeFloat)
    {
        auto data_array = GetDataArray<float>(data);
        auto size = input_data.GetDataSize();
        vector<float> data_vector(data_array, data_array + size);
        return _display_plot.SetData(data_vector);
    }
    else
    {
        return false;
    }
}

NiuMriDisplay1D::~NiuMriDisplay1D()
{
//    LOG_TRACE(L"NiuMriDisplay1D destructor called.", L"NiuMri");
}
