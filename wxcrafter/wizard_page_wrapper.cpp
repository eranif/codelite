#include "wizard_page_wrapper.h"
#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "wizard_wrapper.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include "xmlutils.h"

WizardPageWrapper::WizardPageWrapper()
{
    SetPropertyString(_("Common Settings"), "wxWizardPage");
    m_type = ID_WXWIZARDPAGE;
    m_namePattern = wxT("m_wizardPage");
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, wxT(""),
                                         _("The page-specific bitmap if different from the global one")));
    DoSetPropertyStringValue(PROP_SIZE, wxT("500,300"));
    SetName(GenerateName());
}

WizardPageWrapper::~WizardPageWrapper() {}

wxcWidget* WizardPageWrapper::Clone() const { return new WizardPageWrapper(); }

wxString WizardPageWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));

    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ")
         << wxT("NULL, ") << wxT("NULL, ") << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH))
         << wxT(");\n");
    code << wxT("m_pages.push_back(") << GetName() << wxT(");\n");

    WizardWrapper* wizard = dynamic_cast<WizardWrapper*>(GetParent());
    if(!wizard) { return code; }

    // If this is the last page, perform the 'Chain' call
    // We use |@@| as a sepcial delimiter here (to avoid indenting this code)
    // it will be replaced later by the TopLevelWindowWrapper to an empty string
    const wxcWidget::List_t& siblings = wizard->GetChildren();
    const wxcWidget* lastChild = siblings.back();
    if(lastChild == this) {
        code << wxT("|@@|if (m_pages.size() > 1) {\n") << wxT("|@@|    for(size_t i=1; i<m_pages.size(); i++) {\n")
             << wxT("|@@|        wxWizardPageSimple::Chain(m_pages.at(i-1), m_pages.at(i));\n") << wxT("|@@|    }\n")
             << wxT("|@@|}\n");
        code << wxT("GetPageAreaSizer()->Add(m_pages.at(0));\n");
    }
    return code;
}

wxString WizardPageWrapper::GetWxClassName() const { return wxT("wxWizardPageSimple"); }

void WizardPageWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    bool isFirstPage = (GetParent()->GetChildren().front() == this);

    if(type == XRC_DESIGNER) {
        // HACK: Only display the active page
        if(GUICraftMainPanel::m_MainPanel->GetActiveWizardPage() != this) return;

        text << XRCCommonAttributes() << XRCSize() << XRCStyle() << XRCBitmap();

        text << wxT("<object class=\"wxBoxSizer\">");
        text << wxT("<object class=\"sizeritem\">");
        text << wxT("   <flag>wxEXPAND</flag>");
        text << wxT("   <border>5</border>");
        text << wxT("   <option>1</option>");

        ChildrenXRC(text, type);

        text << wxT("</object>");
        text << wxT("</object>");

    } else if(type == XRC_PREVIEW) {
        if(isFirstPage) {
            text << wxT("<object class=\"") << GetWxClassName() << wxT("\" name=\"WIZARD_PAGE_ONE\">");
        } else {
            text << XRCPrefix();
        }

        text << XRCCommonAttributes() << XRCSize() << XRCStyle() << XRCBitmap();
        ChildrenXRC(text, type);
        text << XRCSuffix();
    } else if(type == XRC_LIVE) {
        text << "<object class=\"" << GetWxClassName() << "\" name=\"" << GetName() << "\">";
        text << XRCCommonAttributes() << XRCSize() << XRCStyle() << XRCBitmap();
        ChildrenXRC(text, type);
        text << XRCSuffix();
    }
}

void WizardPageWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_OTHER"); }
}

void WizardPageWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
    if(propertynode) {
        ImportFromwxFB::ProcessBitmapProperty(propertynode->GetNodeContent(), this, PROP_BITMAP_PATH, "wxART_OTHER");
    }
}
