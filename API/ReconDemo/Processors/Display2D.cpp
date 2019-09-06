#include "Display2D.h"

#include "datamanager.h"
#include "imagelayoutwidget.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"


using namespace Yap;
using namespace std;

Display2D::Display2D(ImageLayoutWidget& display_window) :
    ProcessorImpl(L"Display2D"),
    _display_window(display_window)
{
    //LOG_TRACE(L"Display2D constructor called.", L"ReconDemo");
    AddInput(L"Input", 2, DataTypeAll);


}

Display2D::Display2D(const Display2D &rhs) :
    ProcessorImpl(rhs),
    _display_window(rhs._display_window)
{
    //LOG_TRACE(L"Display2D constructor called.", L"Display2D");
}

bool Display2D::Input(const wchar_t * port, IData *data)
{
    if (wstring(port) != L"Input")
        return false;

    if (data == nullptr)
        return false;

         //
    VariableSpace variables(data->GetVariables());

    /*int freq_count = variables.Get<int>(L"freq_count");
    int phase_count = variables.Get<int>(L"phase_count");
    int slice_count = variables.Get<int>(L"slice_count");
    int channel_mask = variables.Get<int>(L"channel_mask");
    bool test_data = variables.Get<bool>(L"test_data");

    int channel_index=variables.Get<int>(L"channel_index");
    int slice_index = variables.Get<int>(L"slice_index");
    int phase_index=variables.Get<int>(L"phase_index");
    */
    int slice_index = variables.Get<int>(L"slice_index");
    //int channel_index = variables.Get<int>(L"channel_index");
    //int slice_index = variables.Get<int>(L"slice_index");

    //return _display_window.AddImage(data, nullptr);
    return _display_window.UpdateImage(data,nullptr,slice_index);

}


Display2D::~Display2D()
{
//    LOG_TRACE(L"Display2D destructor called.", L"Display2D");
}
