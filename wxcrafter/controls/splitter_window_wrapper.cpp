#include "splitter_window_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/menu.h>
#include <wx/splitter.h>

SplitterWindowWrapper::SplitterWindowWrapper()
    : wxcWidget(ID_WXSPLITTERWINDOW)
{
    wxArrayString options;
    options.Add(wxT("wxSPLIT_VERTICAL"));
    options.Add(wxT("wxSPLIT_HORIZONTAL"));

    SetPropertyString(_("Common Settings"), "wxSplitterWindow");
    AddProperty(new ChoiceProperty(PROP_SPLIT_MODE, options, 0, _("Sets the split mode")));

    wxString tip;
    tip << _("Gravity is real factor which controls position of sash while resizing wxSplitterWindow. \n")
        << _("Gravity tells wxSplitterWindow how much will left/top window grow while resizing.\n")
        << _("Example values:\n") << _("0.0 - only the bottom/right window is automatically resized\n")
        << _("0.5 - both windows grow by equal size\n") << _("1.0 - only left/top window grows\n")
        << _("(Gravity should be a real value between 0.0 and 1.0)");

    AddProperty(new StringProperty(PROP_SASH_GRAVITY, wxT("0.5"), tip));
    AddProperty(new StringProperty(PROP_MIN_PANE_SIZE, wxT("10"), _("Sets the minimum pane size")));
    AddProperty(new StringProperty(PROP_SASH_POS, wxT("0"), _("Sets the sash initial position")));

    PREPEND_STYLE_TRUE(wxSP_3D);
    PREPEND_STYLE_FALSE(wxSP_3DSASH);
    PREPEND_STYLE_FALSE(wxSP_3DBORDER);
    PREPEND_STYLE_FALSE(wxSP_BORDER);
    PREPEND_STYLE_FALSE(wxSP_NOBORDER);
    PREPEND_STYLE_FALSE(wxSP_NO_XP_THEME);
    PREPEND_STYLE_FALSE(wxSP_PERMIT_UNSPLIT);
    PREPEND_STYLE_FALSE(wxSP_LIVE_UPDATE);

    RegisterEvent(wxT("wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING"), wxT(" wxSplitterEvent"),
                  _("The sash position is in the process of being changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED"), wxT(" wxSplitterEvent"),
                  _("The sash position was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_SPLITTER_UNSPLIT"), wxT(" wxSplitterEvent"),
                  _("The splitter has been just unsplit"));
    RegisterEvent(wxT("wxEVT_COMMAND_SPLITTER_DOUBLECLICKED"), wxT(" wxSplitterEvent"),
                  _("The sash was double clicked.\nThe default behaviour is to unsplit the window when this "
                    "happens\n(unless the minimum pane size has been set to a value greater than zero)"));

    m_namePattern = wxT("m_splitter");
    SetName(GenerateName());
}

SplitterWindowWrapper::~SplitterWindowWrapper() {}

wxcWidget* SplitterWindowWrapper::Clone() const { return new SplitterWindowWrapper(); }

wxString SplitterWindowWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor(wxT("wxSP_3D"));
    code << GetName() << wxT("->SetSashGravity(")
         << wxCrafter::FloatToCString(wxCrafter::ToFloat(PropertyString(PROP_SASH_GRAVITY), 0.5)) << wxT(");\n");
    code << GetName() << wxT("->SetMinimumPaneSize(") << wxCrafter::ToNumber(PropertyString(PROP_MIN_PANE_SIZE), 10)
         << wxT(");\n");
    return code;
}

void SplitterWindowWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/splitter.h>"));
}

wxString SplitterWindowWrapper::GetWxClassName() const { return wxT("wxSplitterWindow"); }

void SplitterWindowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    float gravity = wxCrafter::ToFloat(PropertyString(PROP_SASH_GRAVITY), 0.5);
    int minsize = wxCrafter::ToNumber(PropertyString(PROP_MIN_PANE_SIZE), 0);
    int sashpos = wxCrafter::ToNumber(PropertyString(PROP_SASH_POS), 0);

    wxString orientation = "vertical";
    if(!IsSplitVertically()) { orientation = "horizontal"; }

    text << XRCPrefix() << XRCSize() << XRCCommonAttributes() << XRCStyle() << wxT("<gravity>")
         << wxCrafter::FloatToCString(gravity) << wxT("</gravity>") << wxT("<minsize>") << minsize << wxT("</minsize>")
         << wxT("<sashpos>") << sashpos << wxT("</sashpos>") << wxT("<orientation>") << orientation
         << wxT("</orientation>");

    ChildrenXRC(text, type);

    text << XRCSuffix();
}

void SplitterWindowWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("gravity"));
    if(propertynode) { SetPropertyString(PROP_SASH_GRAVITY, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("minsize"));
    if(propertynode) { SetPropertyString(PROP_MIN_PANE_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("sashpos"));
    if(propertynode) { SetPropertyString(PROP_SASH_POS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("orientation"));
    if(propertynode) {
        SetPropertyString(PROP_SPLIT_MODE, propertynode->GetNodeContent() == "vertical"
                                               ? wxString("wxSPLIT_VERTICAL")
                                               : wxString("wxSPLIT_HORIZONTAL"));
    }
}

void SplitterWindowWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("gravity"));
    if(propertynode) { SetPropertyString(PROP_SASH_GRAVITY, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("minsize"));
    if(propertynode) { SetPropertyString(PROP_MIN_PANE_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("sashpos"));
    if(propertynode) { SetPropertyString(PROP_SASH_POS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("orientation"));
    if(propertynode) { SetPropertyString(PROP_SPLIT_MODE, propertynode->GetNodeContent()); }
}

void SplitterWindowWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "sashgravity");
    if(propertynode) { SetPropertyString(PROP_SASH_GRAVITY, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "min_pane_size");
    if(propertynode) { SetPropertyString(PROP_MIN_PANE_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "sashpos");
    if(propertynode) { SetPropertyString(PROP_SASH_POS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "splitmode");
    if(propertynode) { SetPropertyString(PROP_SPLIT_MODE, propertynode->GetNodeContent()); }
}

bool SplitterWindowWrapper::IsSplitVertically() const
{
    wxString orientation = PropertyString(PROP_SPLIT_MODE);
    return orientation == wxT("wxSPLIT_VERTICAL");
}

int SplitterWindowWrapper::GetSashPos() const { return wxCrafter::ToNumber(PropertyString(PROP_SASH_POS), 0); }
