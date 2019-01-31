#ifndef ANIMATIONCTRLWRAPPER_H
#define ANIMATIONCTRLWRAPPER_H

#include "wxc_settings.h"
#include "wxc_widget.h" // Base class: wxcWidget

class AnimationCtrlWrapper : public wxcWidget
{
public:
    AnimationCtrlWrapper();
    virtual ~AnimationCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;

    bool IsLicensed() const { return wxcSettings::Get().IsLicensed2(); }
};

#endif // ANIMATIONCTRLWRAPPER_H
