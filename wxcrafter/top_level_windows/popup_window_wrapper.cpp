#include "popup_window_wrapper.h"
#include "allocator_mgr.h"

PopupWindowWrapper::PopupWindowWrapper()
    : TopLevelWinWrapper(ID_WXPOPUPWINDOW)
{
    SetPropertyString(_("Common Settings"), "wxPopupWindow");
    REMOVE_STYLE(wxCAPTION);
    REMOVE_STYLE(wxSYSTEM_MENU);
    REMOVE_STYLE(wxMAXIMIZE_BOX);
    REMOVE_STYLE(wxMINIMIZE_BOX);
    REMOVE_STYLE(wxRESIZE_BORDER);
    REMOVE_STYLE(wxSTAY_ON_TOP);
    REMOVE_STYLE(wxCLOSE_BOX);

    m_namePattern = wxT("MyPopupWindow");
    SetName(GenerateName());
}

PopupWindowWrapper::~PopupWindowWrapper() {}

wxcWidget* PopupWindowWrapper::Clone() const { return new PopupWindowWrapper(); }

wxString PopupWindowWrapper::CppCtorCode() const { return ""; }

void PopupWindowWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/popupwin.h>");
    headers.Add(wxT("#include <wx/artprov.h>"));
}

wxString PopupWindowWrapper::GetWxClassName() const { return "wxPopupWindow"; }

void PopupWindowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type != wxcWidget::XRC_LIVE) {
        text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>")
             << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" >");
    }

    text << "<object class=\"wxPanel\" name=\"" << GetName() << "\" >"
         << XRCStyle(type != wxcWidget::XRC_LIVE) // The parameter is to add the wxSTAY_ON_TOP, but not if we're 'live'
         << XRCCommonAttributes() << XRCSize();

    ChildrenXRC(text, type);
    text << wxT("</object>");

    if(type != wxcWidget::XRC_LIVE) { text << wxT("</resource>"); }
}

wxString PopupWindowWrapper::BaseCtorDecl() const
{
    wxString code;
    code << wxT("    ") << CreateBaseclassName() << wxT("(wxWindow* parent, long style = ")
         << StyleFlags("wxBORDER_NONE") << ");\n";
    return code;
}

wxString PopupWindowWrapper::BaseCtorImplPrefix() const
{
    wxString code;
    code << CreateBaseclassName() << wxT("::") << CreateBaseclassName() << wxT("(wxWindow* parent,long style)\n");
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
