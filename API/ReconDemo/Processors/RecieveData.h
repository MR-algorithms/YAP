#ifndef RECIEVEDATA_H
#define RECIEVEDATA_H

#include "Implement/ProcessorImpl.h"


class RecieveData : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(RecieveData)
public:
    explicit RecieveData();
    RecieveData(const RecieveData &rhs);

private:
    virtual ~RecieveData();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;
};

#endif // RECIEVEDATA_H
