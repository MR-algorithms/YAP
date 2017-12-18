#include "virtualconsole.h"
#include <cassert>

namespace _details{

class VirtualConsoleImpl
{
public:
    VirtualConsoleImpl();
    bool SetMask(Mask& mask);
    bool SetData();
    bool SetTr(int tr_millisecond);
    bool SetReconHost(wchar_t * ip_address, unsigned short port);

    bool Start();
    void Stop();
private:

}

VirtualConsoleImpl::VirtualConsoleImpl()
{

}

bool VirtualConsoleImpl::SetMask(Mask& mask)
{
    assert(0);
    return false;
}

bool VirtualConsoleImpl::SetData()
{
    assert(0);
    return false;
}

bool VirtualConsoleImpl::SetTr(int tr_millisecond)
{
    assert(0);
    return false;
}

bool VirtualConsoleImpl::SetReconHost(wchar_t *ip_address, unsigned short port)
{
    assert(0);
    return false;
}

bool VirtualConsoleImpl::Start()
{
    assert(0);
    return false;
}

void VirtualConsoleImpl::Stop()
{
    assert(0);
}

};

// ===========================

VirtualConsole::VirtualConsole()
{
    _impl = std::make_shared<_details::VirtualConsoleImpl>();
}

bool VirtualConsole::SetMask()
{
    assert(_impl);

    return _impl->SetMask();
}

bool VirtualConsole::SetData()
{
    assert(_impl);

    return _impl->SetData();
}

bool VirtualConsole::SetTr(int tr_millisecond)
{
    assert(_impl);

    return _impl->SetTr(tr_millisecond);
}

bool VirtualConsole::SetReconHost(wchar_t *ip_address, unsigned short port)
{
    assert(_impl);

    return _impl->SetReconHost(ip_address, port);
}

bool VirtualConsole::Start()
{
    assert(_impl);

    return _impl->Start();
}

void VirtualConsole::Stop()
{
    assert(_impl);
    _impl->Stop();
}
