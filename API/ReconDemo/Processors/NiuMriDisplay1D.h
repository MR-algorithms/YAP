#ifndef NIUMRIDISPLAY1D_H
#define NIUMRIDISPLAY1D_H

#include "Implement/ProcessorImpl.h"

class Plot1D;

class NiuMriDisplay1D :
        public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(NiuMriDisplay1D)
public:
    explicit NiuMriDisplay1D(Plot1D& _display_plot);
    NiuMriDisplay1D(const NiuMriDisplay1D& rhs);

    virtual bool Input(const wchar_t * port, Yap::IData * data) override;

private:
    ~NiuMriDisplay1D();
    Plot1D& _display_plot;
};

#endif // NIUMRIDISPLAY1D_H
