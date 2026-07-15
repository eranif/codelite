#ifndef ANIMATIONCTRLWRAPPER_H
#define ANIMATIONCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class AnimationCtrlWrapper : public wxcWidget
{
public:
    AnimationCtrlWrapper();
    ~AnimationCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // ANIMATIONCTRLWRAPPER_H
