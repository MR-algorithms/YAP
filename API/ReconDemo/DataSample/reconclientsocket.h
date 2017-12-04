#ifndef RECONCLINETSOCKET_H
#define RECONCLINETSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QObject>
#include "SampleDataProtocol.h"

class ReconClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ReconClientSocket(QObject *parent = 0);

signals:
    void signalDataReceived(QByteArray, int);
    void signalDisconnected(int);

public slots:
protected slots:
    void slotDataReceived();
    void slotDisconnected();
private:
    QByteArray Read(uint32_t &cmd_id);
    SampleDataStart _startStruct;
    SampleDataData  _dataStruct;
    SampleDataEnd   _endStruct;



};

#endif // RECONCLINETSOCKET_H
