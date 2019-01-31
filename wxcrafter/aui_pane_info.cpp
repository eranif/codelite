#include "aui_pane_info.h"
#include "json_node.h"
#include "wxgui_helpers.h"
#include <wx/propgrid/propgrid.h>

#define BOOL_TO_STR(b) (b ? "true" : "false")
#define XRC_BOOL_TO_STR(b) (b ? "1" : "0")

AuiPaneInfo::AuiPaneInfo() { Reset(); }

AuiPaneInfo::~AuiPaneInfo() {}

wxString AuiPaneInfo::ToCppCode() const
{
    wxString cppCode = "wxAuiPaneInfo()";

    if(m_name.IsEmpty() == false) { cppCode << ".Name(" << wxCrafter::WXT(m_name) << ")"; }

    if(m_caption.IsEmpty() == false) { cppCode << ".Caption(" << wxCrafter::UNDERSCORE(m_caption) << ")"; }

    cppCode << ".Direction(" << m_dockDirection << ")";
    cppCode << ".Layer(" << m_layer << ")";
    cppCode << ".Row(" << m_row << ")";
    cppCode << ".Position(" << m_position << ")";

    if(m_bestSize != wxDefaultSize) { cppCode << ".BestSize(" << wxCrafter::EncodeSize(m_bestSize) << ")"; }

    if(m_minSize != wxDefaultSize) { cppCode << ".MinSize(" << wxCrafter::EncodeSize(m_minSize) << ")"; }

    if(m_maxSize != wxDefaultSize) { cppCode << ".MaxSize(" << wxCrafter::EncodeSize(m_maxSize) << ")"; }

    if(!m_resizable) { cppCode << ".Fixed()"; }

    cppCode << ".CaptionVisible(" << BOOL_TO_STR(m_captionVisible) << ")";
    cppCode << ".MaximizeButton(" << BOOL_TO_STR(m_maxButton) << ")";
    cppCode << ".CloseButton(" << BOOL_TO_STR(m_closeButton) << ")";
    cppCode << ".MinimizeButton(" << BOOL_TO_STR(m_minButton) << ")";
    cppCode << ".PinButton(" << BOOL_TO_STR(m_pinButton) << ")";

    if(m_toolbarPane) { cppCode << ".ToolbarPane()"; }
    return cppCode;
}

void AuiPaneInfo::Construct(wxPropertyGrid* pg) const
{
    // Popuplate the sizer flags
    wxPGProperty* prop = NULL;
    wxPGProperty* general = pg->Append(new wxPropertyCategory(_("General")));
    pg->Expand(general);

    pg->Append(new wxStringProperty(_("Name"), wxPG_LABEL, m_name));
    pg->Append(new wxStringProperty(_("Caption"), wxPG_LABEL, m_caption));

    prop = pg->Append(new wxBoolProperty(_("Caption Visible"), wxPG_LABEL, m_captionVisible));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);

    prop = pg->Append(new wxBoolProperty(_("Resizable"), wxPG_LABEL, m_resizable));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);

    prop = pg->Append(new wxBoolProperty(_("ToolBar Pane"), wxPG_LABEL, m_toolbarPane));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);

    wxPGProperty* dockings = pg->Append(new wxPropertyCategory(_("Docking")));
    pg->Expand(dockings);

    wxArrayString directions;
    directions.Add("wxAUI_DOCK_TOP");
    directions.Add("wxAUI_DOCK_RIGHT");
    directions.Add("wxAUI_DOCK_BOTTOM");
    directions.Add("wxAUI_DOCK_LEFT");
    directions.Add("wxAUI_DOCK_CENTER");
    wxEnumProperty* dir = (wxEnumProperty*)pg->Append(new wxEnumProperty(_("Direction"), wxPG_LABEL, directions));
    pg->Expand(dir);
    dir->SetChoiceSelection(directions.Index(m_dockDirection));

    pg->Append(new wxIntProperty(_("Layer"), wxPG_LABEL, m_layer));
    pg->Append(new wxIntProperty(_("Row"), wxPG_LABEL, m_row));
    pg->Append(new wxIntProperty(_("Position"), wxPG_LABEL, m_position));

    pg->Append(new wxPropertyCategory(_("Size")));
    pg->Append(new wxStringProperty(_("Best Size"), wxPG_LABEL, wxCrafter::EncodeSize(m_bestSize)));
    pg->Append(new wxStringProperty(_("Min Size"), wxPG_LABEL, wxCrafter::EncodeSize(m_minSize)));
    pg->Append(new wxStringProperty(_("Max Size"), wxPG_LABEL, wxCrafter::EncodeSize(m_maxSize)));

    wxPGProperty* propButtons = pg->Append(new wxPropertyCategory(_("Buttons")));
    pg->Expand(propButtons);

    prop = pg->Append(new wxBoolProperty(_("Close Button"), wxPG_LABEL, m_closeButton));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);

    prop = pg->Append(new wxBoolProperty(_("Minimize Button"), wxPG_LABEL, m_minButton));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);

    prop = pg->Append(new wxBoolProperty(_("Maximize Button"), wxPG_LABEL, m_maxButton));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);

    prop = pg->Append(new wxBoolProperty(_("Pin Button"), wxPG_LABEL, m_pinButton));
    pg->SetPropertyAttribute(prop, wxPG_BOOL_USE_CHECKBOX, true);
}

void AuiPaneInfo::FromJSON(const JSONElement& json)
{
    Reset();
    if(json.isOk() == false) { return; }

    m_name = json.namedObject("m_name").toString();
    m_caption = json.namedObject("m_caption").toString();
    m_dockDirection = json.namedObject("m_dockDirection").toString();
    if(m_dockDirection.IsEmpty()) m_dockDirection = "wxAUI_DOCK_LEFT";

    m_layer = json.namedObject("m_layer").toInt();
    m_row = json.namedObject("m_row").toInt();
    m_position = json.namedObject("m_position").toInt();
    m_bestSize = json.namedObject("m_bestSize").toSize();
    m_minSize = json.namedObject("m_minSize").toSize();
    m_maxSize = json.namedObject("m_maxSize").toSize();
    m_resizable = json.namedObject("m_resizable").toBool();
    m_captionVisible = json.namedObject("m_captionVisible").toBool();
    m_closeButton = json.namedObject("m_closeButton").toBool();
    m_minButton = json.namedObject("m_minButton").toBool();
    m_maxButton = json.namedObject("m_maxButton").toBool();
    m_pinButton = json.namedObject("m_pinButton").toBool();
    m_toolbarPane = json.namedObject("m_toolbarPane").toBool();
}

JSONElement AuiPaneInfo::ToJSON() const
{
    JSONElement element = JSONElement::createObject("wxAuiPaneInfo");
    element.addProperty(wxT("m_name"), m_name);
    element.addProperty(wxT("m_caption"), m_caption);
    element.addProperty(wxT("m_dockDirection"), m_dockDirection);
    element.addProperty(wxT("m_layer"), m_layer);
    element.addProperty(wxT("m_row"), m_row);
    element.addProperty(wxT("m_position"), m_position);
    element.addProperty(wxT("m_bestSize"), m_bestSize);
    element.addProperty(wxT("m_minSize"), m_minSize);
    element.addProperty(wxT("m_maxSize"), m_maxSize);
    element.addProperty(wxT("m_resizable"), m_resizable);
    element.addProperty(wxT("m_captionVisible"), m_captionVisible);
    element.addProperty(wxT("m_closeButton"), m_closeButton);
    element.addProperty(wxT("m_minButton"), m_minButton);
    element.addProperty(wxT("m_maxButton"), m_maxButton);
    element.addProperty(wxT("m_pinButton"), m_pinButton);
    element.addProperty(wxT("m_toolbarPane"), m_toolbarPane);
    return element;
}
void AuiPaneInfo::Reset()
{
    m_dockDirection = "wxAUI_DOCK_LEFT";
    m_layer = 0;
    m_row = 0;
    m_position = 0;
    m_bestSize = wxSize(100, 100);
    m_minSize = wxSize(100, 100);
    m_maxSize = wxSize(100, 100);
    m_resizable = true;
    m_captionVisible = true;
    m_closeButton = false;
    m_minButton = false;
    m_maxButton = false;
    m_pinButton = false;
    m_toolbarPane = false;
}

void AuiPaneInfo::OnChanged(wxPropertyGridEvent& e)
{
    wxPGProperty* prop = e.GetProperty();
    wxString label = prop->GetLabel();
    wxString value = prop->GetValueAsString();
    int iValue = prop->GetValue().GetLong();
    wxSize szValue = wxCrafter::DecodeSize(value);
    bool bValue = (value == "True");

    if(label == _("Name"))
        m_name = value;
    else if(label == _("Caption"))
        m_caption = value;
    else if(label == _("Caption Visible"))
        m_captionVisible = bValue;
    else if(label == _("Resizable"))
        m_resizable = bValue;
    else if(label == _("Direction"))
        m_dockDirection = value;
    else if(label == _("Layer"))
        m_layer = iValue;
    else if(label == _("Row"))
        m_row = iValue;
    else if(label == _("Position"))
        m_position = iValue;

    else if(label == _("Best Size"))
        m_bestSize = szValue;
    else if(label == _("Min Size"))
        m_minSize = szValue;
    else if(label == _("Max Size"))
        m_maxSize = szValue;

    else if(label == _("Close Button"))
        m_closeButton = bValue;
    else if(label == _("Minimize Button"))
        m_minButton = bValue;
    else if(label == _("Maximize Button"))
        m_maxButton = bValue;
    else if(label == _("Pin Button"))
        m_pinButton = bValue;
    else if(label == _("ToolBar Pane"))
        m_toolbarPane = bValue;
}

wxString AuiPaneInfo::ToXRC(const wxString& objXRC) const
{
    wxString xrc;
    xrc << "<object class=\"wxAuiPaneInfo\" name=\"" << m_name << "\">"
        << "<caption>" << m_caption << "</caption>"
        << "<caption_visible>" << XRC_BOOL_TO_STR(m_captionVisible)
        << "</caption_visible>"

        // Dockable Directions
        << "<top_dockable>1</top_dockable>"
        << "<left_dockable>1</left_dockable>"
        << "<bottom_dockable>1</bottom_dockable>"
        << "<right_dockable>1</right_dockable>"

        // Behaviours
        << "<resizable>" << (m_resizable ? "1" : "0") << "</resizable>"
        << "<movable>"
        << "1"
        << "</movable>"
        << "<floatable>"
        << "1"
        << "</floatable>"

        // Sizes
        << "<best_size>" << wxCrafter::EncodeSize(m_bestSize) << "</best_size>"
        << "<max_size>" << wxCrafter::EncodeSize(m_maxSize) << "</max_size>"
        << "<min_size>" << wxCrafter::EncodeSize(m_minSize) << "</min_size>";

    if(m_dockDirection == "wxAUI_DOCK_TOP")
        xrc << "<top>1</top>";
    else if(m_dockDirection == "wxAUI_DOCK_BOTTOM")
        xrc << "<bottom>1</bottom>";
    else if(m_dockDirection == "wxAUI_DOCK_LEFT")
        xrc << "<left>1</left>";
    else if(m_dockDirection == "wxAUI_DOCK_RIGHT")
        xrc << "<right>1</right>";
    else if(m_dockDirection == "wxAUI_DOCK_CENTER" || m_dockDirection == "wxAUI_DOCK_CENTRE")
        xrc << "<centre>1</centre>";
    else
        xrc << "<left>1</left>"; // Default

    xrc << "<layer>" << m_layer << "</layer>"
        << "<row>" << m_row << "</row>"
        << "<position>" << m_position
        << "</position>"

        // Buttons
        << "<close_button>" << XRC_BOOL_TO_STR(m_closeButton) << "</close_button>"
        << "<minimize_button>" << XRC_BOOL_TO_STR(m_minButton) << "</minimize_button>"
        << "<maximize_button>" << XRC_BOOL_TO_STR(m_maxButton) << "</maximize_button>"
        << "<pin_button>" << XRC_BOOL_TO_STR(m_pinButton) << "</pin_button>";

    if(m_toolbarPane) { xrc << "<toolbar_pane>" << XRC_BOOL_TO_STR(m_toolbarPane) << "</toolbar_pane>"; }
    xrc << objXRC << "</object>";
    return xrc;
}
