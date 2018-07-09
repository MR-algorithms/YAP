#ifndef PSLICEITERATOR_H_20170722
#define PSLICEITERATOR_H_20170722

#include "Implement/ProcessorImpl.h"

class PSliceIterator : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(PSliceIterator)
public:
    explicit PSliceIterator();
    PSliceIterator(const PSliceIterator& rhs);

private:
    virtual ~PSliceIterator();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;
};

#endif // PSLICEITERATOR_H
