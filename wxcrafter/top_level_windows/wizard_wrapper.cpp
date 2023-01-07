#include "wizard_wrapper.h"
#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/wizard.h>

WizardWrapper::WizardWrapper()
    : TopLevelWinWrapper(ID_WXWIZARD)
{
    SetPropertyString(_("Common Settings"), "wxWizard");
    DoSetPropertyStringValue(PROP_TITLE, _("My Wizard"));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, wxT(""),
                                         _("The default bitmap used in the left side of the wizard.")));

    PREPEND_STYLE(wxDEFAULT_DIALOG_STYLE, true);
    EnableStyle(wxT("wxCLOSE_BOX"), false);
    EnableStyle(wxT("wxSYSTEM_MENU"), false);
    EnableStyle(wxT("wxMINIMIZE_BOX"), false);
    EnableStyle(wxT("wxMAXIMIZE_BOX"), false);
    EnableStyle(wxT("wxRESIZE_BORDER"), false);
    EnableStyle(wxT("wxCAPTION"), false);

    // Wizard related events
    RegisterEvent(wxT("wxEVT_WIZARD_PAGE_CHANGED"), wxT("wxWizardEvent"),
                  _("The page has just been changed (this event cannot be vetoed)"));
    RegisterEvent(wxT("wxEVT_WIZARD_PAGE_CHANGING"), wxT("wxWizardEvent"),
                  _("The page is being changed (this event can be vetoed)."));
    RegisterEvent(wxT("wxEVT_WIZARD_PAGE_SHOWN"), wxT("wxWizardEvent"), _("The page was shown and laid out."));
    RegisterEvent(wxT("wxEVT_WIZARD_CANCEL"), wxT("wxWizardEvent"),
                  _("The user attempted to cancel the wizard (this event may also be vetoed)."));
    RegisterEvent(wxT("wxEVT_WIZARD_HELP"), wxT("wxWizardEvent"), _("The wizard help button was pressed."));
    RegisterEvent(wxT("wxEVT_WIZARD_FINISHED"), wxT("wxWizardEvent"), _("The wizard finished button was pressed."));

    // Dialog related events
    RegisterEvent(wxT("wxEVT_INIT_DIALOG"), wxT("wxInitDialogEvent"),
                  _("A wxInitDialogEvent is sent as a dialog or panel is being initialised. Handlers for this event "
                    "can transfer data to the window.\nThe default handler calls wxWindow::TransferDataToWindow"));
    RegisterEvent(wxT("wxEVT_CLOSE_WINDOW"), wxT("wxCloseEvent"),
                  _("Process a close event. This event applies to wxFrame and wxDialog classes"));
    RegisterEvent(wxT("wxEVT_ACTIVATE"), wxT("wxActivateEvent"), _("Process a wxEVT_ACTIVATE event"));
    RegisterEvent(wxT("wxEVT_ACTIVATE_APP"), wxT("wxActivateEvent"), _("Process a wxEVT_ACTIVATE_APP event"));

    AddCategory(_("Wizard Icons"));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_16, wxT(""), _("Select a 16x16 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_32, wxT(""), _("Select a 32x32 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_64, wxT(""), _("Select a 64x64 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_128, wxT(""), _("Select a 128x128 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_256, wxT(""), _("Select a 256x256 bitmap")));

    m_namePattern = wxT("MyWizard");
    SetName(GenerateName());
}

WizardWrapper::~WizardWrapper() {}

wxString WizardWrapper::BaseCtorDecl() const
{
    wxString code;
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    code << wxT("    ") << CreateBaseclassName()
         << wxT("(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = ")
         << wxCrafter::UNDERSCORE(PropertyString(PROP_TITLE)) << wxT(", ")
         << wxT("const wxBitmap& bmp = wxNullBitmap, ") << wxT("const wxPoint& pos = wxDefaultPosition, ")
         << wxT("long style = ") << StyleFlags(wxT("wxDEFAULT_DIALOG_STYLE")) << wxT(");\n") << wxT("    ")
         << wxT("wxWizardPageSimple* GetFirstPage() const { if(!m_pages.empty()) return m_pages.at(0); return NULL; "
                "}\n");
    return code;
}

wxString WizardWrapper::BaseCtorImplPrefix() const
{
    wxString code;
    code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
         << wxT("(wxWindow* parent, wxWindowID id, const wxString& title, const wxBitmap& bmp, const wxPoint& pos, "
                "long style)\n");
    return code;
}

wxString WizardWrapper::DesignerXRC(bool forPreviewDialog) const
{
    wxUnusedVar(forPreviewDialog);
    wxString text;
    text << wxT("<object class=\"wxPanel\" name=\"PreviewPanel\">") << XRCSize();

    ChildrenXRC(text, XRC_DESIGNER);
    text << wxT("</object>");

    WrapXRC(text);
    return text;
}

void WizardWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type != wxcWidget::XRC_LIVE) {
        text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>")
             << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" >");
    }

    wxString centred;
    if(!PropertyString(PROP_CENTRE_ON_SCREEN).empty()) {
        centred = wxT("<centered>1</centered>"); // In XRC centring is on/off; it doesn't discriminate between
                                                 // wxBOTH/wxVERTICAL/wxHORIZONTAL
    }

    text << XRCPrefix() << wxT("<title>") << wxCrafter::CDATA(PropertyString(PROP_TITLE)) << wxT("</title>") << centred
         << XRCBitmap()
         << XRCStyle(type != wxcWidget::XRC_LIVE) // The parameter is to add the wxSTAY_ON_TOP, but not if we're 'live'
         << XRCCommonAttributes();

    ChildrenXRC(text, type);
    text << wxT("</object>");

    if(type != wxcWidget::XRC_LIVE) { text << wxT("</resource>"); }
}

void WizardWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/wizard.h>"));
    headers.Add(wxT("#include <vector>"));
    headers.Add(wxT("#include <wx/artprov.h>"));
}

wxString WizardWrapper::CppCtorCode() const
{
    wxString code;

    // Add the dialog icons to the generator helper
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_16));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_32));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_64));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_128));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_256));

    code << wxT("Create(parent, id, title, ")
         << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(", pos, style);\n");
    return code;
}

wxString WizardWrapper::DoGenerateClassMember() const
{
    wxString memberCode;
    memberCode << wxT("    std::vector<wxWizardPageSimple*> m_pages;\n");
    return memberCode;
}

void WizardWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("centered"));
    if(propertynode) { SetPropertyString(PROP_CENTRE_ON_SCREEN, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_OTHER"); }

    // We've already done 'size' in the baseclass, but do it again for TLWindows
    // That's because if no size was specified, wxC inflicts 500,300. This isn't what the user expected
    // So, if there is *not* a specified size, overwrite the wxC default with wxDefaultSize
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

void WizardWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
    if(propertynode) {
        ImportFromwxFB::ProcessBitmapProperty(propertynode->GetNodeContent(), this, PROP_BITMAP_PATH, "wxART_OTHER");
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "center");
    if(propertynode) { SetPropertyString(PROP_CENTRE_ON_SCREEN, propertynode->GetNodeContent()); }
    // See the comment in LoadPropertiesFromXRC()
    propertynode = XmlUtils::FindNodeByName(node, "property", wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

bool WizardWrapper::HasIcon() const { return true; }
