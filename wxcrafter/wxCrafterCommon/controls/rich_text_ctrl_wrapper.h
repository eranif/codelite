#ifndef RICHTEXTCTRLWRAPPER_H
#define RICHTEXTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class RichTextCtrlWrapper : public wxcWidget
{

public:
    RichTextCtrlWrapper();
    ~RichTextCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // RICHTEXTCTRLWRAPPER_H
