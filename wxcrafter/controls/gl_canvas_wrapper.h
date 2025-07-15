#ifndef GLCANVASWRAPPER_H
#define GLCANVASWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class GLCanvasWrapper : public wxcWidget
{
    wxArrayString m_attrs;

public:
    GLCanvasWrapper();
    ~GLCanvasWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // GLCANVASWRAPPER_H
