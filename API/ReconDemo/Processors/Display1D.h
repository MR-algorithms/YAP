#ifndef DISPLAY1D_H
#define DISPLAY1D_H

#include "Implement/ProcessorImpl.h"

class Plot1D;

class Display1D :
        public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(Display1D)
public:
    explicit Display1D(Plot1D& _display_plot);
    Display1D(const Display1D& rhs);

    virtual bool Input(const wchar_t * port, Yap::IData * data) override;

private:
    ~Display1D();
    Plot1D& _display_plot;
};

#endif // DISPLAY1D_H
