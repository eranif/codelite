#include "list_ctrl_column_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

ListCtrlColumnWrapper::ListCtrlColumnWrapper()
    : wxcWidget(ID_WXLISTCTRL_COL)
{
    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    SetPropertyString(_("Common Settings"), "wxListCtrlColumn");
    AddProperty(new CategoryProperty(_("wxListCtrl Column")));
    AddProperty(new StringProperty(PROP_NAME, _("My Column"), _("Column caption")));
    AddProperty(new StringProperty(
        PROP_WIDTH, wxT("-1"),
        _("Column Width\nSet it to -1 for auto sizing.\nOr set it to -2 to fit the column width to heading or to "
          "extend to fill all the remaining space for the last column\nValue > 0 will set the width in pixels")));
}

ListCtrlColumnWrapper::~ListCtrlColumnWrapper() {}

wxcWidget* ListCtrlColumnWrapper::Clone() const { return new ListCtrlColumnWrapper(); }

wxString ListCtrlColumnWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetWindowParent() << wxT("->InsertColumn(") << GetWindowParent() << wxT("->GetColumnCount(), ")
            << wxCrafter::UNDERSCORE(GetName()) << wxT(", ") << wxT("wxLIST_FORMAT_LEFT, ") << PropertyString(PROP_WIDTH) << wxT(");");
    return cppCode;
}

void ListCtrlColumnWrapper::GetIncludeFile(wxArrayString& headers) const
{
    // No need to add anything, we can only be added under wxListCtrl which
    // provides all the needed include files
}

wxString ListCtrlColumnWrapper::GetWxClassName() const { return wxT(""); }

void ListCtrlColumnWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"listcol\">") << wxT("<text>") << wxCrafter::CDATA(GetName()) << wxT("</text>")
         << wxT("<width>") << PropertyString(PROP_WIDTH) << wxT("</width>") << wxT("</object>");
}

void ListCtrlColumnWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("text"));
    if(propertynode) { SetName(propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("width"));
    if(propertynode) { SetPropertyString(PROP_WIDTH, propertynode->GetNodeContent()); }
}

// AFAICT wxSmith & wxFB don't do ListCtrlColumns
