#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include "Implement/ProcessorImpl.h"

class ImageLayoutWidget;

class NiuMriDisplay2D :
        public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(NiuMriDisplay2D)
public:
    explicit NiuMriDisplay2D(ImageLayoutWidget& display_window);
    NiuMriDisplay2D(const NiuMriDisplay2D& rhs);

    virtual bool Input(const wchar_t * port, Yap::IData * data) override;

protected:
    ~NiuMriDisplay2D();
    ImageLayoutWidget& _display_window;
};

#endif // DISPLAY_H
