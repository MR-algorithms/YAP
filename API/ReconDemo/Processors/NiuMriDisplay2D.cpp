#include "NiuMriDisplay2D.h"

#include "datamanager.h"
#include "imagelayoutwidget.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"

using namespace Yap;
using namespace std;

NiuMriDisplay2D::NiuMriDisplay2D(ImageLayoutWidget& display_window) :
    ProcessorImpl(L"NiuMriDisplay2D"),
    _display_window(display_window)
{
    //LOG_TRACE(L"NiuMriDisplay2D constructor called.", L"ReconDemo");
    AddInput(L"Input", 2, DataTypeAll);
}

NiuMriDisplay2D::NiuMriDisplay2D(const NiuMriDisplay2D &rhs) :
    ProcessorImpl(rhs),
    _display_window(rhs._display_window)
{
    //LOG_TRACE(L"NiuMriDisplay2D constructor called.", L"NiuMri");
}

bool NiuMriDisplay2D::Input(const wchar_t * port, IData *data)
{
    if (wstring(port) != L"Input")
        return false;

    //
    VariableSpace variables(data->GetVariables());

    int freq_count = variables.Get<int>(L"freq_count");
    int phase_count = variables.Get<int>(L"phase_count");
    int slice_count = variables.Get<int>(L"slice_count");
    int channel_mask = variables.Get<int>(L"channel_mask");
    bool test_data = variables.Get<bool>(L"test_data");

    int channel_index=variables.Get<int>(L"channel_index");
    int slice_index = variables.Get<int>(L"slice_index");
    int phase_index=variables.Get<int>(L"phase_index");




    //return _display_window.AddImage(data, nullptr);
    return _display_window.UpdateImage(data,nullptr,slice_index);
}

NiuMriDisplay2D::~NiuMriDisplay2D()
{
//    LOG_TRACE(L"NiuMriDisplay2D destructor called.", L"NiuMri");
}
