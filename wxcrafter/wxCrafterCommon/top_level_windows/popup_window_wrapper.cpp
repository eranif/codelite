#include "popup_window_wrapper.h"

#include <wx/popupwin.h>

PopupWindowWrapper::PopupWindowWrapper()
    : PopupWindowWrapper(ID_WXPOPUPWINDOW, "wxPopupWindow", "MyPopupWindow")
{
}

PopupWindowWrapper::PopupWindowWrapper(int id, const wxString& wxClassName, const wxString& namePattern)
    : TopLevelWinWrapper(id)
    , m_wxClassName(wxClassName)
{
    SetPropertyString(_("Common Settings"), wxClassName);
    REMOVE_STYLE(wxCAPTION);
    REMOVE_STYLE(wxSYSTEM_MENU);
    REMOVE_STYLE(wxMAXIMIZE_BOX);
    REMOVE_STYLE(wxMINIMIZE_BOX);
    REMOVE_STYLE(wxRESIZE_BORDER);
    REMOVE_STYLE(wxSTAY_ON_TOP);
    REMOVE_STYLE(wxCLOSE_BOX);

    PREPEND_STYLE(wxPU_CONTAINS_CONTROLS, true);
    m_namePattern = namePattern;
    SetName(GenerateName());
}

wxcWidget* PopupWindowWrapper::Clone() const { return new PopupWindowWrapper(); }

PopupTransientWindowWrapper::PopupTransientWindowWrapper()
    : PopupWindowWrapper(ID_WXPOPUPTRANSIENTWINDOW, "wxPopupTransientWindow", "MyPopupTransientWindow")
{
}

wxcWidget* PopupTransientWindowWrapper::Clone() const { return new PopupTransientWindowWrapper(); }

wxString PopupWindowWrapper::CppCtorCode() const { return ""; }

void PopupWindowWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/popupwin.h>");
    headers.Add(wxT("#include <wx/artprov.h>"));
}

wxString PopupWindowWrapper::GetWxClassName() const { return m_wxClassName; }

void PopupWindowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if (type != wxcWidget::XRC_LIVE) {
        text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>")
             << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" >");
    }

    text << "<object class=\"wxPanel\" name=\"" << GetName() << "\" >"
         << XRCStyle(type != wxcWidget::XRC_LIVE) // The parameter is to add the wxSTAY_ON_TOP, but not if we're 'live'
         << XRCCommonAttributes() << XRCSize();

    ChildrenXRC(text, type);
    text << wxT("</object>");

    if (type != wxcWidget::XRC_LIVE) {
        text << wxT("</resource>");
    }
}

wxString PopupWindowWrapper::BaseCtorDecl() const
{
    wxString code;
    code << wxT("    ") << CreateBaseClassName() << wxT("(wxWindow* parent, long style = ")
         << StyleFlags("wxBORDER_NONE") << ");\n";
    return code;
}

wxString PopupWindowWrapper::BaseCtorImplPrefix() const
{
    wxString code;
    code << CreateBaseClassName() << wxT("::") << CreateBaseClassName() << wxT("(wxWindow* parent,long style)\n");
    code << "    : " << GetRealClassName() << "(parent, style)\n";
    return code;
}

wxString PopupWindowWrapper::DesignerXRC(bool forPreviewDialog) const
{
    wxString text;
    text << wxT("<object class=\"wxPanel\" name=\"PreviewPanel\">") << wxT("<hidden>1</hidden>") << XRCSize();

    ChildrenXRC(text, XRC_DESIGNER);
    text << wxT("</object>");

    WrapXRC(text);
    return text;
}

void PopupTransientWindowWrapper::DoGenerateExtraFunctions(wxString& decl, wxString& impl) const
{
    // Nothing to do for a window that was not created as a top level one
    if (GetParent()) {
        return;
    }

    const wxString cls = CreateBaseClassName();
    const wxString parent = GetRealClassName();

    decl << "    virtual void Popup(wxWindow* focus = nullptr) override;\n"
         << "    virtual void OnDismiss() override;\n"
         << "    virtual bool ProcessLeftDown(wxMouseEvent& event) override;\n"
         << "    virtual bool Show(bool show = true) override;\n";

    impl << "void " << cls << "::Popup(wxWindow* focus)\n"
         << "{\n"
         << "    " << parent << "::Popup(focus);\n"
         << "}\n\n"
         << "void " << cls << "::OnDismiss()\n"
         << "{\n"
         << "    " << parent << "::OnDismiss();\n"
         << "}\n\n"
         << "bool " << cls << "::ProcessLeftDown(wxMouseEvent& event)\n"
         << "{\n"
         << "    return " << parent << "::ProcessLeftDown(event);\n"
         << "}\n\n"
         << "bool " << cls << "::Show(bool show)\n"
         << "{\n"
         << "    return " << parent << "::Show(show);\n"
         << "}\n";
}
