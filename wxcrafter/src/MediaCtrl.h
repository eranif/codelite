#ifndef MEDIACTRL_H
#define MEDIACTRL_H
#include "wxcrafter_gui.h"

class MediaCtrl : public MediaCtrlBase
{
public:
    explicit MediaCtrl(wxWindow* parent);
    ~MediaCtrl() override = default;

    wxStaticBitmap* Bitmap() const { return m_staticBitmap667; }
};
#endif // MEDIACTRL_H
