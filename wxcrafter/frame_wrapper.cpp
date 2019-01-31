#include "frame_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "file_ficker_property.h"
#include "menu_bar_wrapper.h"
#include "status_bar_wrapper.h"
#include "toolbar_base_wrapper.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_widget.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/docview.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/toplevel.h>

FrameWrapper::FrameWrapper()
    : TopLevelWinWrapper(ID_WXFRAME)
{
    PREPEND_STYLE(wxFRAME_FLOAT_ON_PARENT, false);
    PREPEND_STYLE(wxFRAME_NO_TASKBAR, false);
    PREPEND_STYLE(wxFRAME_TOOL_WINDOW, false);
    PREPEND_STYLE(wxDEFAULT_FRAME_STYLE, true);

    SetPropertyString(_("Common Settings"), "wxFrame");
    DoSetPropertyStringValue(PROP_TITLE, wxT("My Frame"));

    RegisterEvent(wxT("wxEVT_CLOSE_WINDOW"), wxT("wxCloseEvent"),
                  wxT("Process a close event. This event applies to wxFrame and wxDialog classes"));
    RegisterEvent(wxT("wxEVT_ACTIVATE"), wxT("wxActivateEvent"), wxT("Process a wxEVT_ACTIVATE event"));
    RegisterEvent(wxT("wxEVT_ACTIVATE_APP"), wxT("wxActivateEvent"), wxT("Process a wxEVT_ACTIVATE_APP event"));

    AddCategory("Frame Type");
    wxArrayString frameTypes;
    frameTypes.Add("wxFrame");
    frameTypes.Add("wxDocMDIParentFrame");
    frameTypes.Add("wxDocMDIChildFrame");
    frameTypes.Add("wxDocParentFrame");
    frameTypes.Add("wxDocChildFrame");
    frameTypes.Add("wxMiniFrame");
    AddProperty(new ChoiceProperty(PROP_FRAME_TYPE, frameTypes, 0, _("Select the wxFrame type you want")));

    AddCategory(_("Frame Icons"));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_16, wxT(""), _("Select a 16x16 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_32, wxT(""), _("Select a 32x32 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_64, wxT(""), _("Select a 64x64 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_128, wxT(""), _("Select a 128x128 bitmap")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH_256, wxT(""), _("Select a 256x256 bitmap")));

    m_namePattern = wxT("MyFrame");
    SetName(GenerateName());
}

FrameWrapper::~FrameWrapper() {}

void FrameWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type != XRC_LIVE) {
        text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>")
             << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" >");
    }

    wxString centred;
    if(!PropertyString(PROP_CENTRE_ON_SCREEN).empty()) {
        centred = wxT("<centered>1</centered>"); // In XRC centring is on/off; it doesn't discriminate between
                                                 // wxBOTH/wxVERTICAL/wxHORIZONTAL
    }
    
    wxString xrcPrefix = "wxFrame";
    if(PropertyString(PROP_FRAME_TYPE) ==  "wxMiniFrame") {
        xrcPrefix = "wxMiniFrame";
    }
    
    text << XRCPrefix("wxFrame") << wxT("<title>") << PropertyString(PROP_TITLE) << wxT("</title>") << centred
         << XRCStyle(type != wxcWidget::XRC_LIVE) // The parameter is to add the wxSTAY_ON_TOP, but not if we're 'live'
         << XRCSize() << XRCCommonAttributes();

    ChildrenXRC(text, type);
    text << wxT("</object>");

    if(type != wxcWidget::XRC_LIVE) { text << wxT("</resource>"); }
}

wxString FrameWrapper::DesignerXRC(bool forPreviewDialog) const
{
    wxUnusedVar(forPreviewDialog);
    wxString text;
    text << wxT("<object class=\"wxPanel\" name=\"PreviewPanel\">") << "<caption-title>"
         << wxCrafter::CDATA(PropertyString(PROP_TITLE)) << "</caption-title>"
         << "<tlw-style>" << StyleFlags() << "</tlw-style>"
         << "<tlw-icon>" << wxCrafter::CDATA(PropertyString(PROP_BITMAP_PATH_16)) << "</tlw-icon>"
         << wxT("<hidden>1</hidden>") << XRCSize() << XRCCommonAttributes();

    ChildrenXRC(text, XRC_DESIGNER);
    text << wxT("</object>");

    // frame can have a wxMenuBar /  wxToolBar / wxStatusBar as a direct child, search for them
    bool bMenuBar = false;
    bool bToolBar = false;
    bool bStatusBar = false;

    wxcWidget::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        if(!bMenuBar && (*iter)->GetType() == ID_WXMENUBAR) {
            wxString menuBarXRC;
            MenuBarWrapper* mb = dynamic_cast<MenuBarWrapper*>(*iter);
            if(mb) {
                menuBarXRC = mb->DesignerXRC();
                text.Prepend(menuBarXRC);
                bMenuBar = true;
            }

        } else if(!bToolBar && (*iter)->GetType() == ID_WXTOOLBAR) {

            wxString toolbarXRC;
            ToolbarWrapper* tb = dynamic_cast<ToolbarWrapper*>(*iter);
            if(tb) {
                toolbarXRC = tb->DesignerXRC();
                text.Prepend(toolbarXRC);
                bToolBar = true;
            }

        } else if(!bStatusBar && (*iter)->GetType() == ID_WXSTATUSBAR) {

            wxString statusBarXRC;
            StatusBarWrapper* sb = dynamic_cast<StatusBarWrapper*>(*iter);
            if(sb) {
                statusBarXRC = sb->DesignerXRC();
                text.Prepend(statusBarXRC);
                bStatusBar = true;
            }

        } else if(bToolBar && bMenuBar && bStatusBar) {
            break;
        }
    }

    WrapXRC(text);
    return text;
}

wxString FrameWrapper::CppCtorCode() const
{
    // Add the dialog icons to the generator helper
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_16));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_32));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_64));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_128));
    wxcCodeGeneratorHelper::Get().AddIcon(PropertyFile(PROP_BITMAP_PATH_256));
    return TopLevelWinWrapper::CppCtorCode();
}

void FrameWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/frame.h>"));
    headers.Add(wxT("#include <wx/iconbndl.h>"));
    headers.Add(wxT("#include <wx/artprov.h>"));
    headers.Add(wxT("#include <wx/sizer.h>")); // needed for (GetSizer()->...)

    if(PropertyString(PROP_FRAME_TYPE) == "wxMiniFrame") {
        headers.Add("#include <wx/minifram.h>");
    } else if(PropertyString(PROP_FRAME_TYPE) != "wxFrame") {
        headers.Add("#include <wx/docview.h>");
        headers.Add("#include <wx/docmdi.h>");
    }
}

wxString FrameWrapper::BaseCtorImplPrefix() const
{
    wxString code;
    wxString frameType = PropertyString(PROP_FRAME_TYPE);
    if(frameType == "wxDocMDIParentFrame") {
        code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
             << "(wxDocManager *manager, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, "
                "const wxSize& size, long style)\n";
        code << "    : " << GetRealClassName() << "(manager, parent, id, title, pos, size, style)\n";

    } else if(frameType == "wxDocMDIChildFrame") {
        code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
             << "(wxDocument *doc, wxView *view, wxMDIParentFrame *parent, wxWindowID id, const wxString& title, const "
                "wxPoint& pos, const wxSize& size, long style)\n";
        code << "    : " << GetRealClassName() << "(doc, view, parent, id, title, pos, size, style)\n";

    } else if(frameType == "wxDocChildFrame") {
        code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
             << "(wxDocument *doc, wxView *view, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& "
                "pos, const wxSize& size, long style)\n";
        code << "    : " << GetRealClassName() << "(doc, view, parent, id, title, pos, size, style)\n";

    } else if(frameType == "wxDocParentFrame") {
        code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
             << "(wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, "
                "const wxSize& size, long style)\n";
        code << "    : " << GetRealClassName() << "(manager, parent, id, title, pos, size, style)\n";

    } else { // wxFrame or wxMiniFrame
        code << CreateBaseclassName() << wxT("::") << CreateBaseclassName()
             << wxT("(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, "
                    "long style)\n");
        code << "    : " << GetRealClassName() << "(parent, id, title, pos, size, style)\n";
    }
    return code;
}

wxString FrameWrapper::BaseCtorDecl() const
{
    wxString code;
    wxString frameType = PropertyString(PROP_FRAME_TYPE);
    wxString title = wxCrafter::UNDERSCORE(PropertyString(PROP_TITLE));

    if(frameType == "wxDocMDIParentFrame") {
        // wxDocMDIParentFrame
        code << wxT("    ") << CreateBaseclassName()
             << wxT("(wxDocManager *manager, wxFrame* parent, wxWindowID id = wxID_ANY, const wxString& title = ")
             << title << wxT(", ") << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ")
             << SizeAsString() << wxT(", ") << wxT("long style = ") << StyleFlags(wxT("0")) << wxT(");\n");

    } else if(frameType == "wxDocMDIChildFrame") {
        code << wxT("    ") << CreateBaseclassName()
             << wxT("(wxDocument *doc, wxView *view, wxMDIParentFrame *parent, "
                    "wxWindowID id = wxID_ANY, const wxString& title = ")
             << title << wxT(", ") << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ")
             << SizeAsString() << wxT(", ") << wxT("long style = ") << StyleFlags(wxT("0")) << wxT(");\n");

    } else if(frameType == "wxDocChildFrame") {
        code << wxT("    ") << CreateBaseclassName()
             << wxT("(wxDocument *doc, wxView *view, wxFrame *parent, "
                    "wxWindowID id = wxID_ANY, const wxString& title = ")
             << title << wxT(", ") << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ")
             << SizeAsString() << wxT(", ") << wxT("long style = ") << StyleFlags(wxT("0")) << wxT(");\n");

    } else if(frameType == "wxDocParentFrame") {
        code << wxT("    ") << CreateBaseclassName()
             << wxT("(wxDocManager *manager, wxFrame *parent, wxWindowID id = wxID_ANY, const wxString& title = ")
             << title << wxT(", ") << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ")
             << SizeAsString() << wxT(", ") << wxT("long style = ") << StyleFlags(wxT("0")) << wxT(");\n");

    } else {
        // wxFrame
        code << wxT("    ") << CreateBaseclassName()
             << wxT("(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = ") << title << wxT(", ")
             << wxT("const wxPoint& pos = wxDefaultPosition, const wxSize& size = ") << SizeAsString() << wxT(", ")
             << wxT("long style = ") << StyleFlags(wxT("0")) << wxT(");\n");
    }
    return code;
}

void FrameWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
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

void FrameWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
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

void FrameWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "center");
    if(propertynode) { SetPropertyString(PROP_CENTRE_ON_SCREEN, propertynode->GetNodeContent()); }
    // See the comment in LoadPropertiesFromXRC()
    propertynode = XmlUtils::FindNodeByName(node, "property", wxT("size"));
    if(!propertynode) { SetPropertyString(PROP_SIZE, "-1,-1"); }
}

bool FrameWrapper::HasIcon() const { return true; }

wxString FrameWrapper::GetWxClassName() const { return PropertyString(PROP_FRAME_TYPE); }

wxString FrameWrapper::GetDerivedClassCtorSignature() const
{
    wxString code;
    wxString frameType = PropertyString(PROP_FRAME_TYPE);
    if(frameType == "wxDocMDIParentFrame") {
        code << "(wxDocManager *manager, wxFrame* parent)";

    } else if(frameType == "wxDocMDIChildFrame") {
        code << "(wxDocument *doc, wxView *view, wxMDIParentFrame *parent)";

    } else if(frameType == "wxDocChildFrame") {
        code << "(wxDocument *doc, wxView *view, wxFrame *parent)";

    } else if(frameType == "wxDocParentFrame") {
        code << "(wxDocManager *manager, wxFrame *parent)";

    } else {
        // default
        code << TopLevelWinWrapper::GetDerivedClassCtorSignature();
    }
    return code;
}

wxString FrameWrapper::GetParentCtorInitArgumentList() const
{
    wxString code;
    wxString frameType = PropertyString(PROP_FRAME_TYPE);
    if(frameType == "wxDocMDIParentFrame") {
        code << "(manager, parent)";

    } else if(frameType == "wxDocMDIChildFrame") {
        code << "(doc, view, parent)";

    } else if(frameType == "wxDocChildFrame") {
        code << "(doc, view, parent)";

    } else if(frameType == "wxDocParentFrame") {
        code << "(manager, parent)";

    } else {
        // default
        code << TopLevelWinWrapper::GetParentCtorInitArgumentList();
    }
    return code;
}
