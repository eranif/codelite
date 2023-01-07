#include "dialog_wrapper.h"
#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/dialog.h>

DialogWrapper::DialogWrapper()
    : TopLevelWinWrapper(ID_WXDIALOG)
{
    DoClearFlags(m_styles);
    PREPEND_STYLE(wxDIALOG_NO_PARENT, false);
    PREPEND_STYLE(wxDIALOG_EX_CONTEXTHELP, false);
    PREPEND_STYLE(wxSTAY_ON_TOP, false);
    PREPEND_STYLE(wxDEFAULT_DIALOG_STYLE, true);

    RegisterEvent(wxT("wxEVT_INIT_DIALOG"), wxT("wxInitDialogEvent"),
                  _("A wxInitDialogEvent is sent as a dialog or panel is being initialised. Handlers for this event "
                    "can transfer data to the window.\nThe default handler calls wxWindow::TransferDataToWindow"));
    RegisterEvent(wxT("wxEVT_CLOSE_WINDOW"), wxT("wxCloseEvent"),
                  _("Process a close event. This event applies to wxFrame and wxDialog classes"));
    RegisterEvent(wxT("wxEVT_ACTIVATE"), wxT("wxActivateEvent"), _("Process a wxEVT_ACTIVATE event"));
    RegisterEvent(wxT("wxEVT_ACTIVATE_APP"), wxT("wxActivateEvent"), _("Process a wxEVT_ACTIVATE_APP event"));

    SetPropertyString(_("Common Settings"), "wxDialog");
    DoSetPropertyStringValue(PROP_TITLE, _("My Dialog"));
    m_namePattern = wxT("MyDialog");

    AddCategory(_("Dialog Icons"));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_16, wxT(""), _("Select a 16x16 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_32, wxT(""), _("Select a 32x32 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_64, wxT(""), _("Select a 64x64 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_128, wxT(""), _("Select a 128x128 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_256, wxT(""), _("Select a 256x256 bitmap")));
    SetName(GenerateName());
}

DialogWrapper::~DialogWrapper() {}

wxString DialogWrapper::DesignerXRC(bool forPreviewDialog) const
{
    wxUnusedVar(forPreviewDialog);
    wxString text;
    text << wxT("<object class=\"wxPanel\" name=\"PreviewPanel\">") << "<caption-title>"
         << wxCrafter::CDATA(PropertyString(PROP_TITLE)) << "</caption-title>"
         << "<tlw-style>" << StyleFlags() << "</tlw-style>"
         << "<tlw-icon>" << wxCrafter::CDATA(PropertyString(PROP_BITMAP_PATH_16)) << "</tlw-icon>"
         << wxT("<hidden>1</hidden>") << XRCSize();

    ChildrenXRC(text, XRC_DESIGNER);
    text << wxT("</object>");

    WrapXRC(text);
    return text;
}

wxString DialogWrapper::BaseCtorDecl() const
{
    wxString code;
    wxString title = wxCrafter::UNDERSCORE(PropertyString(PROP_TITLE));
    code << wxT("    ") << CreateBaseclassName()
         << wxT("(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = ") << title << wxT(", ")
         << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ") << SizeAsString() << wxT(", ")
         << wxT("long style = ") << StyleFlags(wxT("0")) << wxT(");\n");
    return code;
}

wxString DialogWrapper::BaseCtorImplPrefix() const
{
    wxString code;
    code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
         << wxT("(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long "
                "style)\n");
    code << "    : " << GetRealClassName() << "(parent, id, title, pos, size, style)\n";
    return code;
}

wxcWidget* DialogWrapper::Clone() const { return new DialogWrapper(); }

wxString DialogWrapper::CppCtorCode() const
{
    // Add the dialog icons to the generator helper
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_16));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_32));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_64));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_128));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_256));
    return TopLevelWinWrapper::CppCtorCode();
}

void DialogWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/dialog.h>"));
    headers.Add(wxT("#include <wx/iconbndl.h>"));
    headers.Add(wxT("#include <wx/artprov.h>"));
}

wxString DialogWrapper::GetWxClassName() const { return wxT("wxDialog"); }

void DialogWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type != wxcWidget::XRC_LIVE) {
        text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>")
             << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\">");
    }

    wxString centred;
    if(!PropertyString(PROP_CENTRE_ON_SCREEN).empty()) {
        centred = wxT("<centered>1</centered>"); // In XRC centring is on/off; it doesn't discriminate between
                                                 // wxBOTH/wxVERTICAL/wxHORIZONTAL
    }

    text << XRCPrefix() << wxT("<title>") << wxCrafter::CDATA(PropertyString(PROP_TITLE)) << wxT("</title>") << centred
         << XRCStyle(type != wxcWidget::XRC_LIVE) // The parameter is to add the wxSTAY_ON_TOP, but not if we're 'live'
         << XRCCommonAttributes() << XRCSize();

    ChildrenXRC(text, type);
    text << wxT("</object>");

    if(type != wxcWidget::XRC_LIVE) { text << wxT("</resource>"); }
}

void DialogWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("centered"));
    if(propertynode) { SetPropertyString(PROP_CENTRE_ON_SCREEN, propertynode->GetNodeContent()); }

    // We've already done 'size' in the baseclass, but do it again for TLWindows
    // That's because if no size was specified, wxC inflicts 500,300. This isn't what the user expected
    // So, if there is *not* a specified size, overwrite the wxC default with wxDefaultSize
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

void DialogWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("centered"));
    if(propertynode) { SetPropertyString(PROP_CENTRE_ON_SCREEN, propertynode->GetNodeContent()); }

    // We've already done 'size' in the baseclass, but do it again for TLWindows
    // That's because if no size was specified, wxC inflicts 500,300. This isn't what the user expected
    // So, if there is *not* a specified size, overwrite the wxC default with wxDefaultSize
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

void DialogWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "center");
    if(propertynode) { SetPropertyString(PROP_CENTRE_ON_SCREEN, propertynode->GetNodeContent()); }
    // See the comment in LoadPropertiesFromXRC()
    propertynode = XmlUtils::FindNodeByName(node, "property", wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

bool DialogWrapper::HasIcon() const { return true; }
