#ifndef NIUMAGSLICEITERATOR_H_20170722
#define NIUMAGSLICEITERATOR_H_20170722

#include "Implement/ProcessorImpl.h"

class NiumagSliceIterator : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(NiumagSliceIterator)
public:
    explicit NiumagSliceIterator();
    NiumagSliceIterator(const NiumagSliceIterator& rhs);

private:
    virtual ~NiumagSliceIterator();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;
};

#endif // NIUMAGSLICEITERATOR_H
