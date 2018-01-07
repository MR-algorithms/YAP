#include "NiuMriDisplay2D.h"

#include "datamanager.h"
#include "imagelayoutwidget.h"
#include "Implement/LogUserImpl.h"

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

    return _display_window.AddImage(data, nullptr);
}

NiuMriDisplay2D::~NiuMriDisplay2D()
{
//    LOG_TRACE(L"NiuMriDisplay2D destructor called.", L"NiuMri");
}
