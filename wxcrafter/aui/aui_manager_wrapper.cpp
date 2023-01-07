#include "aui_manager_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "color_property.h"
#include "int_property.h"
#include "wxgui_helpers.h"
#include <wx/aui/framemanager.h>

AuiManagerWrapper::AuiManagerWrapper()
    : wxcWidget(ID_WXAUIMANAGER)
{
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    // wxAUI_DOCKART_PANE_BORDER_SIZE
    // wxAUI_DOCKART_BACKGROUND_COLOUR
    // wxAUI_DOCKART_SASH_COLOUR
    // wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR
    // wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR
    // wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR
    // wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR
    // wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR
    // wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR
    // wxAUI_DOCKART_BORDER_COLOUR
    // wxAUI_DOCKART_GRIPPER_COLOUR
    // wxAUI_DOCKART_GRADIENT_TYPE

    SetPropertyString(_("Common Settings"), "wxAuiManager");
    AddProperty(new CategoryProperty(_("General")));
    AddProperty(new StringProperty(PROP_NAME, "", _("wxAuiManager member name")));
    AddProperty(new ColorProperty(PROP_BG, "<Default>", _("Background Colour")));
    AddProperty(new IntProperty(PROP_AUI_PANE_BORDER_SIZE, -1, _("Pane border size")));

    AddProperty(new CategoryProperty(_("Sash")));
    AddProperty(new ColorProperty(PROP_AUI_SASH_COLOUR, "<Default>", _("Sash colour")));
    AddProperty(new IntProperty(PROP_AUI_SASH_SIZE, -1, _("Set the wxAUI sash size")));

    AddProperty(new CategoryProperty(_("Caption")));
    wxArrayString gradientTypes;
    gradientTypes.Add("wxAUI_GRADIENT_NONE");
    gradientTypes.Add("wxAUI_GRADIENT_VERTICAL");
    gradientTypes.Add("wxAUI_GRADIENT_HORIZONTAL");
    AddProperty(new ChoiceProperty(PROP_AUI_GRADIENT_TYPE, gradientTypes, 0, _("Gradient type")));

    AddProperty(new ColorProperty(PROP_AUI_CAPTION_COLOUR, "<Default>", _("Active caption colour")));
    AddProperty(new ColorProperty(PROP_AUI_CAPTION_COLOUR_GRADIENT, "<Default>", _("Active caption gradient colour")));
    AddProperty(new ColorProperty(PROP_AUI_INACTIVE_CAPTION_COLOUR, "<Default>", _("Inactive caption colour")));
    AddProperty(new ColorProperty(PROP_AUI_INACTIVE_CAPTION_COLOUR_GRADIENT, "<Default>",
                                  _("Inactive caption gradient colour")));
    AddProperty(new ColorProperty(PROP_AUI_ACTIVE_CAPTION_TEXT_COLOUR, "<Default>", _("Active caption text colour")));
    AddProperty(
        new ColorProperty(PROP_AUI_INACTIVE_CAPTION_TEXT_COLOUR, "<Default>", _("Inactive caption text colour")));

    m_styles.Clear();

    PREPEND_STYLE(wxAUI_MGR_ALLOW_FLOATING, true);
    PREPEND_STYLE(wxAUI_MGR_ALLOW_ACTIVE_PANE, true);
    PREPEND_STYLE(wxAUI_MGR_TRANSPARENT_DRAG, true);
    PREPEND_STYLE(wxAUI_MGR_TRANSPARENT_DRAG, true);
    PREPEND_STYLE(wxAUI_MGR_TRANSPARENT_HINT, true);
    PREPEND_STYLE(wxAUI_MGR_VENETIAN_BLINDS_HINT, false);
    PREPEND_STYLE(wxAUI_MGR_RECTANGLE_HINT, false);
    PREPEND_STYLE(wxAUI_MGR_HINT_FADE, false);
    PREPEND_STYLE(wxAUI_MGR_NO_VENETIAN_BLINDS_FADE, false);
    PREPEND_STYLE(wxAUI_MGR_LIVE_RESIZE, true);

    RegisterEvent("wxEVT_AUI_PANE_BUTTON", "wxAuiManagerEvent",
                  _("Triggered when any button is pressed for any docked panes"));
    RegisterEvent("wxEVT_AUI_PANE_CLOSE", "wxAuiManagerEvent",
                  _("Triggered when a docked or floating pane is closed."));
    RegisterEvent("wxEVT_AUI_PANE_MAXIMIZE", "wxAuiManagerEvent", _("Triggered when a pane is maximized."));
    RegisterEvent("wxEVT_AUI_PANE_RESTORE", "wxAuiManagerEvent", _("Triggered when a pane is restored."));
    RegisterEvent("wxEVT_AUI_PANE_ACTIVATED", "wxAuiManagerEvent",
                  _("Triggered when a pane is made 'active'. This event is new since wxWidgets 2.9.4."));
    RegisterEvent("wxEVT_AUI_RENDER", "wxAuiManagerEvent",
                  _("This event can be caught to override the default renderer in order "
                    "to custom draw your wxAuiManager window (not recommended)."));

    m_namePattern = "m_auimgr";
    SetName(GenerateName());
}

AuiManagerWrapper::~AuiManagerWrapper() {}

wxcWidget* AuiManagerWrapper::Clone() const { return new AuiManagerWrapper(); }

wxString AuiManagerWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetName() << " = new wxAuiManager;\n";

    if(GetParent()->IsTopWindow()) {
        cppCode << GetName() << "->SetManagedWindow( this );\n";
    } else {
        cppCode << GetName() << "->SetManagedWindow( " << GetParent()->GetName() << " );\n";
    }
    cppCode << GetName() << "->SetFlags( " << StyleFlags("0") << ");\n";
    if(PropertyInt(PROP_AUI_SASH_SIZE) != -1)
        cppCode << GetName() << "->GetArtProvider()->SetMetric( wxAUI_DOCKART_SASH_SIZE, "
                << PropertyInt(PROP_AUI_SASH_SIZE) << ");\n";

    if(PropertyInt(PROP_AUI_PANE_BORDER_SIZE) != -1)
        cppCode << GetName() << "->GetArtProvider()->SetMetric( wxAUI_DOCKART_PANE_BORDER_SIZE, "
                << PropertyInt(PROP_AUI_PANE_BORDER_SIZE) << ");\n";

    wxString col;

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_SASH_COLOUR));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_SASH_COLOUR, " << col << ");\n";

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_CAPTION_COLOUR));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, " << col << ");\n";

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_CAPTION_COLOUR_GRADIENT));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, " << col
                << ");\n";

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_INACTIVE_CAPTION_COLOUR));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, " << col
                << ");\n";

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_INACTIVE_CAPTION_COLOUR_GRADIENT));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, " << col
                << ");\n";

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_ACTIVE_CAPTION_TEXT_COLOUR));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, " << col
                << ");\n";

    col = wxCrafter::ColourToCpp(PropertyString(PROP_AUI_INACTIVE_CAPTION_TEXT_COLOUR));
    if(!col.IsEmpty())
        cppCode << GetName() << "->GetArtProvider()->SetColor( wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, " << col
                << ");\n";
    cppCode << GetName() << "->GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, "
            << PropertyString(PROP_AUI_GRADIENT_TYPE) << ");\n";

    return cppCode;
}

void AuiManagerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/aui/framemanager.h>");
    headers.Add("#include <wx/aui/dockart.h>");
}

wxString AuiManagerWrapper::GetWxClassName() const { return "wxAuiManager"; }

void AuiManagerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << "<object class=\"wxAuiManager\">";
    text << "<style>" << StyleFlags("wxAUI_MGR_DEFAULT") << "</style>";
    text << "<sashsize>" << PropertyInt(PROP_AUI_SASH_SIZE) << "</sashsize>";
    text << "<pane-border-size>" << PropertyInt(PROP_AUI_PANE_BORDER_SIZE) << "</pane-border-size>";
    text << "<sash-colour>" << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_SASH_COLOUR)) << "</sash-colour>";
    text << "<caption-colour>" << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_CAPTION_COLOUR));
    text << "</caption-colour>";
    text << "<caption-colour-gradient>" << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_CAPTION_COLOUR_GRADIENT));
    text << "</caption-colour-gradient>";
    text << "<inactive-caption-colour>" << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_INACTIVE_CAPTION_COLOUR));
    text << "</inactive-caption-colour>";
    text << "<inactive-caption-colour-gradient>";
    text << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_INACTIVE_CAPTION_COLOUR_GRADIENT));
    text << "</inactive-caption-colour-gradient>";
    text << "<caption-text-colour>" << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_ACTIVE_CAPTION_TEXT_COLOUR));
    text << "</caption-text-colour>";
    text << "<inactive-caption-text-colour>";
    text << wxCrafter::GetColourForXRC(PropertyString(PROP_AUI_ACTIVE_CAPTION_TEXT_COLOUR));
    text << "</inactive-caption-text-colour>";
    text << "<gradient-type>" << PropertyString(PROP_AUI_GRADIENT_TYPE) << "</gradient-type>";

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString AuiManagerWrapper::CppDtorCode() const
{
    wxString cppCode;
    cppCode << "    " << GetName() << "->UnInit();\n";
    cppCode << "    delete " << GetName() << ";\n";
    return cppCode;
}
