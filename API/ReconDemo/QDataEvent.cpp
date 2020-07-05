#include "QDataEvent.h"

QDataEvent::QDataEvent(QEvent::Type type, DataEvent data_event, uint32_t scan_id, DimensionType dimension_type)
    : QEvent(type)
    , _data_event(data_event)
    , _scan_id(scan_id)
    , _dimenison_type(dimension_type)
{

}

DataEvent QDataEvent::GetDataEvent() const
{
    return _data_event;
}

uint32_t QDataEvent::GetScanID() const
{
    return _scan_id;
}

