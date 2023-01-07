#ifndef POPUPWINDOWWRAPPER_H
#define POPUPWINDOWWRAPPER_H

#include "top_level_win_wrapper.h"
#include "wxc_widget.h" // Base class: wxcWidget

class PopupWindowWrapper : public TopLevelWinWrapper
{
public:
    virtual wxString BaseCtorDecl() const;
    virtual wxString BaseCtorImplPrefix() const;
    virtual wxString DesignerXRC(bool forPreviewDialog) const;
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;

    PopupWindowWrapper();
    virtual ~PopupWindowWrapper();
};

#endif // POPUPWINDOWWRAPPER_H
