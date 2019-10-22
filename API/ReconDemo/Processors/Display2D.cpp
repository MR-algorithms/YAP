#include "Display2D.h"

#include "datamanager.h"
#include "imagelayoutwidget.h"
#include "Implement/LogUserImpl.h"

#include "Client/DataHelper.h"
#include "commonmethod.h"
#include <QThread>
#include "windows.h"

using namespace Yap;
using namespace std;

Display2D::Display2D(ImageLayoutWidget& display_window) :
    ProcessorImpl(L"Display2D"),
    _display_window(display_window)
{
    LOG_TRACE(L"<Display2D> constructor called.", L"ReconDemo");
    AddInput(L"Input", 2, DataTypeAll);


}

Display2D::Display2D(const Display2D &rhs) :
    ProcessorImpl(rhs),
    _display_window(rhs._display_window)
{
    LOG_TRACE(L"<Display2D> copy constructor called.", L"ReconDemo");
}

bool Display2D::Input(const wchar_t * port, IData *data)
{
    if (wstring(port) != L"Input")
        return false;

    if (data == nullptr)
        return false;

         //
    VariableSpace variables(data->GetVariables());


    int slice_index = variables.Get<int>(L"slice_index");
    int channel_index = variables.Get<int>(L"channel_index");
    int channel_switch = variables.Get<int>(L"channel_switch");
    int channel_count = CommonMethod::GetChannelCountUsed(channel_switch);

    //calcute the image index managed in the Layout Window.
    int index = channel_count * channel_count + slice_index;

    //return _display_window.AddImage(data);
    return _display_window.UpdateImage(data, index);

    //Test repaint the widget.
    /*
    QThread::sleep(5);

    if(channel_index <= 4)
    {
        qDebug()<<"Display2d::thread id = "<<GetCurrentThreadId();

        qDebug()<<"channel index = "<< channel_index;
        return _display_window.UpdateImage(data, 0);
    }
    else
        return true;
    */

}


Display2D::~Display2D()
{
    LOG_TRACE(L"<Display2D>  destructor called.", L"ReconDemo");
}
