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

enum ScanSignal
{
    SSScanStart,
    SSSlicePhaseStep,   //Phase step is written in a slice.
    SSChannelPhaseStep, //phase step is wiitten in a channel,
    SSSliceFinished,    //
    SSChannelFinished,
    SSScanFinished,
    SSUnkown,

};

class QDataEvent : public QEvent
{
public:
    QDataEvent(QEvent::Type type, ScanSignal scan_signal, uint32_t scan_id, int channel_index, int phase_index);
    ScanSignal GetScanSignal() const{return _scan_signal;}
    uint32_t GetScanID() const {return _scan_id;}
    int GetChannelIndex() const {return _channel_index;}
    int GetPhaseIndex() const {return _phase_index;}
private:
    ScanSignal _scan_signal;
    uint32_t _scan_id;
    int _channel_index;
    int _phase_index;


};

class IDataObserver
{
public:
    virtual ~IDataObserver(){}
    virtual void OnData(ScanSignal scan_signal, uint32_t scan_id, int channel_index, int phase_index) = 0;

};
#endif // CDATAEVENT_H
