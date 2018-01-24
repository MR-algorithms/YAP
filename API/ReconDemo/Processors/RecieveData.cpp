#include "RecieveData.h"

#include "Implement/DataObject.h"
#include "Client/DataHelper.h"

using namespace Yap;
using namespace std;

RecieveData::RecieveData() : ProcessorImpl(L"RecieveData")
{

    AddInput(L"Input",   YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
    AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
}


RecieveData::RecieveData(const RecieveData &rhs) :  ProcessorImpl(rhs)
{
    //LOG_TRACE(L"NiuMriDisplay2D constructor called.", L"NiuMri");
}

bool RecieveData::Input(const wchar_t * port, IData *data)
{
    if (wstring(port) != L"Input")
        return false;

    return Feed(L"Output", data);
}

RecieveData::~RecieveData()
{
//    LOG_TRACE(L"NiuMriDisplay2D destructor called.", L"NiuMri");
}
