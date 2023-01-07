#ifndef MEDIACTRL_H
#define MEDIACTRL_H
#include "wxcrafter_gui.h"

class MediaCtrl : public MediaCtrlBase
{
public:
    MediaCtrl(wxWindow* parent);
    virtual ~MediaCtrl();

    wxStaticBitmap* Bitmap() const { return m_staticBitmap667; }
};
#endif // MEDIACTRL_H
