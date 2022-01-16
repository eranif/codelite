#ifndef ANIMATIONCTRLWRAPPER_H
#define ANIMATIONCTRLWRAPPER_H

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
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // ANIMATIONCTRLWRAPPER_H
