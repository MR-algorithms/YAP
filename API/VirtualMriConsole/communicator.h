#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H


struct DataPackage;

class Communicator
{
public:
    Communicator();

    bool SetRemoteHost(const wchar_t * const ip_address, unsigned short port);
    bool Connect();
    bool Disconnect();
    bool Send(const DataPakage& data);
};

#endif // COMMUNICATOR_H
