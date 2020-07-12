#include "QDataEvent.h"

QDataEvent::QDataEvent(QEvent::Type type, ScanSignal scan_signal,
                       uint32_t scan_id, int channel_index, int phase_index)
    : QEvent(type)
    , _scan_signal(scan_signal)
    , _scan_id(scan_id)
    , _channel_index(channel_index)
    , _phase_index(phase_index)
{

}

