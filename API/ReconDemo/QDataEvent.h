#ifndef QDATAEVENT_H
#define QDATAEVENT_H
#include <QEvent>
#include <string>
#include <complex>
#include <memory>

using namespace std;
namespace ScanInfo{
enum DimensionType{
    frequence = 1,
    phase,
    slice,
    channele,
    unknown
};

enum ScanSignal
{
    SSScanStart = 1,
    SSChannelPhaseStep, //phase step is wiitten in a channel,
    SSScanFinished,

    SSSlicePhaseStep,   //Phase step is written in a slice.
    SSSliceFinished,    //
    SSChannelFinished,
    SSUnkown,

};
struct ScanDimension
{
    int scan_id;
    int channel_switch;
    int freq_count;
    int phase_count;
    int slice_count;

    int channel_index;
    int slice_index;
    int phase_index;

    ScanDimension()
    {
        channel_switch = -1;
        scan_id = -1;
        freq_count =  -1;
        phase_count =  -1;;
        slice_count =  -1;;
        phase_index = -1;
        slice_index = -1;
        channel_index = -1;
    }
     ScanDimension(const ScanDimension &rhs)
     {
         channel_switch = rhs.channel_switch;
         scan_id = rhs.scan_id;
         freq_count =  rhs.freq_count;
         phase_count =  rhs.phase_count;
         slice_count =  rhs.slice_count;;
         phase_index = rhs.phase_index;
         slice_index = rhs.slice_index;
         channel_index = rhs.channel_index;
     }
     ScanDimension& operator = (const ScanDimension& rhs){
         channel_switch = rhs.channel_switch;
         scan_id = rhs.scan_id;
         freq_count =  rhs.freq_count;
         phase_count =  rhs.phase_count;
         slice_count =  rhs.slice_count;;
         phase_index = rhs.phase_index;
         slice_index = rhs.slice_index;
         channel_index = rhs.channel_index;
     }
};

class QScanEvent : public QEvent
{
public:
    QScanEvent(QEvent::Type type, ScanSignal scan_signal, const ScanDimension& scan_dim, complex<float>* buffer);
    ScanSignal get_scan_signal() const{return _scan_signal;}
    ScanDimension get_scan_dimension() const {return _scan_dim;}
    complex<float>* get_buffer(){return _buffer;}
private:
    ScanSignal _scan_signal;
    ScanDimension _scan_dim;
    complex<float> *_buffer;

};

class IDataObserver
{
public:
    virtual ~IDataObserver(){}
    virtual void OnData(ScanSignal scan_signal, const ScanDimension& scan_dimension, shared_ptr<complex<float>> data) = 0;
};
}
#endif // CDATAEVENT_H
