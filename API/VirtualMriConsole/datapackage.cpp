#include "datapackage.h"

bool MessageProcess::Pack(DataPackage &package, const SampleDataStart &start)
{
    return true;
}
bool MessageProcess::Pack(DataPackage &package, const SampleDataData &data)
{
    return true;

}
bool MessageProcess::Pack(DataPackage &package, const SampleDataEnd &end)
{
    return true;
}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataStart &start)
{
    return true;

}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataData  &data)
{
    return true;

}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataEnd   &end)
{
    return true;

}
