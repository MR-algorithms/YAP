#ifndef VIRTUALCONSOLE_H
#define VIRTUALCONSOLE_H
#include <memory>

namespace _details{
    class VirtualConsoleImpl;
};

struct Mask;

class VirtualConsole
{
public:
    VirtualConsole();
    bool SetMask(Mask& mask);
    bool SetData();
    bool SetTr(int tr_millisecond);
    bool SetReconHost(wchar_t * ip_address, unsigned short port);

    bool Start();
    void Stop();
private:
    std::shared_ptr<_details::VirtualConsoleImpl> _impl;
};

#endif // VIRTUALCONSOLE_H
