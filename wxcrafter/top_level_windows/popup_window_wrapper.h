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
};

#endif // POPUPWINDOWWRAPPER_H
