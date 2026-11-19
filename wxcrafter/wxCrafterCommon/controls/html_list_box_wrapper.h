#pragma once

#include "vlistbox_wrapper.h"

/// Wrapper for wxHtmlListBox.
///
/// Like its base class wxVListBox, wxHtmlListBox is abstract: it needs OnGetItem(). Without a "Subclass", the
/// generated form class gets a small nested class with a default implementation (see VListBoxWrapper).
/// wxHtmlListBox has no wxXmlResource handler at all, so it is always a placeholder in the XRC output.
class HtmlListBoxWrapper : public VListBoxWrapper
{
public:
    HtmlListBoxWrapper();
    ~HtmlListBoxWrapper() override = default;

    wxcWidget* Clone() const override { return new HtmlListBoxWrapper(); }
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;

protected:
    bool HasXRCHandler() const override { return false; }
    void DefaultImplMethods(wxString& decl, wxString& impl, const wxString& qualifiedName) const override;
};
