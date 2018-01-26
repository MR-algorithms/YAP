#ifndef VIRTUALCONSOLE_H
#define VIRTUALCONSOLE_H
#include <qstring>
#include <memory>
#include <complex>
using namespace std;

namespace _details{
    class VirtualConsoleImpl;
}
namespace Scan {
    struct Mask;
    struct ScanTask;
}
enum EThreadState{ idle, scanning, paused, stopped};
class VirtualConsole
{
public:


    static VirtualConsole& GetHandle(){
            return s_instance;
        }
    VirtualConsole();
    bool PrepareScantask(const Scan::ScanTask& task);

    bool Scan();
    void Stop();
private:
    std::shared_ptr<_details::VirtualConsoleImpl> _impl;

    static VirtualConsole s_instance;
};

#endif // VIRTUALCONSOLE_H
