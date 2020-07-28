#include "QDataEvent.h"

using namespace ScanInfo;
QScanEvent::QScanEvent(QEvent::Type type, ScanSignal scan_signal, const ScanDimension& scan_dim, complex<float>* buffer)
    : QEvent(type)
    , _scan_signal(scan_signal)
    , _scan_dim(scan_dim)
    , _buffer(buffer)
{

}

