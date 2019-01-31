#include "panel_wrapper_top_level.h"
#include "allocator_mgr.h"
#include "xmlutils.h"
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

PanelWrapperTopLevel::PanelWrapperTopLevel()
    : TopLevelWinWrapper(ID_WXPANEL_TOPLEVEL)
{
    SetPropertyString(_("Common Settings"), "wxPanel");
    // Enable the 'wxTAB_TRAVERSAL' style
    EnableStyle(wxT("wxTAB_TRAVERSAL"), true);

    REMOVE_STYLE(wxCAPTION);
    REMOVE_STYLE(wxSYSTEM_MENU);
    REMOVE_STYLE(wxMAXIMIZE_BOX);
    REMOVE_STYLE(wxMINIMIZE_BOX);
    REMOVE_STYLE(wxRESIZE_BORDER);
    REMOVE_STYLE(wxSTAY_ON_TOP);
    REMOVE_STYLE(wxCLOSE_BOX);

    m_namePattern = wxT("MyPanel");
    SetName(GenerateName());
}

PanelWrapperTopLevel::~PanelWrapperTopLevel() {}

void PanelWrapperTopLevel::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/panel.h>"));
    headers.Add(wxT("#include <wx/artprov.h>"));
}

wxString PanelWrapperTopLevel::GetWxClassName() const { return wxT("wxPanel"); }

wxString PanelWrapperTopLevel::CppCtorCode() const { return TopLevelWinWrapper::CppCtorCode(); }

void PanelWrapperTopLevel::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type != wxcWidget::XRC_LIVE) {
        text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>")
             << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" >");
    }

    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes();

    ChildrenXRC(text, type);
    text << wxT("</object>");

    if(type != wxcWidget::XRC_LIVE) { text << wxT("</resource>"); }
}

wxString PanelWrapperTopLevel::DesignerXRC(bool forPreviewDialog) const
{
    wxUnusedVar(forPreviewDialog);
    wxString text;
    text << wxT("<object class=\"wxPanel\" name=\"PreviewPanel\">") << wxT("<hidden>1</hidden>") << XRCSize()
         << XRCStyle() << XRCCommonAttributes();

    ChildrenXRC(text, XRC_DESIGNER);
    text << wxT("</object>");

    WrapXRC(text);
    return text;
}

bool PanelWrapperTopLevel::IsTopWindow() const { return GetParent() == NULL; }

wxString PanelWrapperTopLevel::BaseCtorDecl() const
{
    wxString code;
    code << wxT("    ") << CreateBaseclassName() << wxT("(wxWindow* parent, wxWindowID id = wxID_ANY, ")
         << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ") << SizeAsString() << wxT(", ")
         << wxT("long style = ") << StyleFlags(wxT("wxTAB_TRAVERSAL")) << wxT(");\n");
    return code;
}

wxString PanelWrapperTopLevel::BaseCtorImplPrefix() const
{
    wxString code;
    code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
         << wxT("(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)\n");
    code << "    : " << GetRealClassName() << "(parent, id, pos, size, style)\n";
    return code;
}

void PanelWrapperTopLevel::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // We've already done 'size' in the baseclass, but do it again for TLWindows
    // That's because if no size was specified, wxC inflicts 500,300. This isn't what the user expected
    // So, if there is *not* a specified size, overwrite the wxC default with wxDefaultSize
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

void PanelWrapperTopLevel::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // We've already done 'size' in the baseclass, but do it again for TLWindows
    // That's because if no size was specified, wxC inflicts 500,300. This isn't what the user expected
    // So, if there is *not* a specified size, overwrite the wxC default with wxDefaultSize
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

void PanelWrapperTopLevel::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // See the comment in LoadPropertiesFromXRC()
    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}
