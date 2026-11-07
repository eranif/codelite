#ifndef POPUPWINDOWWRAPPER_H
#define POPUPWINDOWWRAPPER_H

#include "top_level_win_wrapper.h"
#include "wxc_widget.h" // Base class: wxcWidget

class PopupWindowWrapper : public TopLevelWinWrapper
{
public:
    PopupWindowWrapper();
    ~PopupWindowWrapper() override = default;

    wxString BaseCtorDecl() const override;
    wxString BaseCtorImplPrefix() const override;
    wxString DesignerXRC(bool forPreviewDialog) const override;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

protected:
    /// For the sub classes: the same form with another ID and another wxWidgets class
    PopupWindowWrapper(int id, const wxString& wxClassName, const wxString& namePattern);

    wxString m_wxClassName;
};

/// wxPopupTransientWindow: a popup that is dismissed when the user clicks outside of it. The code is the same as of
/// wxPopupWindow
class PopupTransientWindowWrapper : public PopupWindowWrapper
{
public:
    PopupTransientWindowWrapper();
    ~PopupTransientWindowWrapper() override = default;

    wxcWidget* Clone() const override;

protected:
    /// Generate the methods that wxPopupTransientWindow expects its sub classes to override (they call the base class)
    void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const override;
};

#endif // POPUPWINDOWWRAPPER_H
