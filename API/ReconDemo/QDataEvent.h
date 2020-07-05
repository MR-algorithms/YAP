#ifndef QDATAEVENT_H
#define QDATAEVENT_H
#include <QEvent>
#include <string>

enum DimensionType{
    frequence = 1,
    phase,
    slice,
    channele,
    unknown
};

enum DataEvent
{
    DataBegin,
    DataData,
    DataEnd,
    DataUnkown,
};

class QDataEvent : public QEvent
{
public:
    QDataEvent(QEvent::Type type, DataEvent data_event, uint32_t scan_id, DimensionType dimension_type=unknown);
    DataEvent GetDataEvent() const;
    uint32_t GetScanID() const;
private:
    DataEvent _data_event;
    uint32_t _scan_id;
    DimensionType _dimenison_type;
    int _start_index;
    int _size_in_dimension;


};
#endif // CDATAEVENT_H
