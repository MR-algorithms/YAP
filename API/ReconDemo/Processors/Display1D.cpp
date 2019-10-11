#include "Display1D.h"

#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"
#include "plot1d.h"

using namespace Yap;
using namespace std;

Display1D::Display1D(Plot1D& display_plot) :
    ProcessorImpl(L"Display1D"),
    _display_plot(display_plot)
{
    LOG_TRACE(L"<Display1D> constructor called.", L"ReconDemo");
    AddInput(L"Input", 1, DataTypeComplexFloat | DataTypeFloat);

    _display_plot.setRenderHint(QPainter::Antialiasing);
}

Display1D::Display1D(const Display1D &rhs) :
    ProcessorImpl(rhs),
    _display_plot(rhs._display_plot)
{
    LOG_TRACE(L"<Display1D> constructor called.", L"ReconDemo");
}

bool Display1D::Input(const wchar_t *port, Yap::IData *data)
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

Display1D::~Display1D()
{

}
