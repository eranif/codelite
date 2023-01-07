#include "tree_list_ctrl_column_wrapper.h"

#include "allocator_mgr.h"
#include "choice_property.h"
#include "col_header_flags_property.h"
#include "wxgui_helpers.h"

#include <wx/headercol.h>

TreeListCtrlColumnWrapper::TreeListCtrlColumnWrapper()
    : wxcWidget(ID_WXTREELISTCTRLCOL)
{
    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    wxArrayString alignment;
    alignment.Add("wxALIGN_LEFT");
    alignment.Add("wxALIGN_RIGHT");
    alignment.Add("wxALIGN_CENTER");

    // FIXME: add alignment + column flags here
    AddProperty(new CategoryProperty(_("wxTreeListCtrl Column")));
    AddProperty(new StringProperty(PROP_NAME, _("My Column"), _("Column caption")));
    AddProperty(new ChoiceProperty(PROP_DV_LISTCTRL_COL_ALIGN, alignment, 0,
                                   _("Alignment of both the column header and its items")));
    AddProperty(new StringProperty(
        PROP_WIDTH, "-2",
        _("The width of the column in pixels or the special wxCOL_WIDTH_AUTOSIZE(-2) value indicating that the column "
          "should adjust to its contents. Notice that the first column is special and will be always resized to fill "
          "all the space not taken by the other columns, i.e. the width specified here is ignored for it")));
    AddProperty(new ColHeaderFlagsProperty(
        PROP_COL_FLAGS, wxCOL_DEFAULT_FLAGS,
        _("Column flags, currently can include wxCOL_RESIZABLE to allow the user to resize the column and "
          "wxCOL_SORTABLE to allow the user to resort the control contents by clicking on this column")));
}

TreeListCtrlColumnWrapper::~TreeListCtrlColumnWrapper() {}

wxcWidget* TreeListCtrlColumnWrapper::Clone() const { return new TreeListCtrlColumnWrapper(); }

wxString TreeListCtrlColumnWrapper::CppCtorCode() const
{
    wxString cppCode;
    wxString alignstring = PropertyString(PROP_DV_LISTCTRL_COL_ALIGN);

    cppCode << GetWindowParent() << "->AppendColumn(" << wxCrafter::UNDERSCORE(GetName()) << ", " // Label
            << PropertyInt(PROP_WIDTH, -1) << ", "                                                // Width
            << alignstring << ", "                                                                // Alignment
            << PropertyString(PROP_COL_FLAGS, "0") << ");";                                       // Flags
    return cppCode;
}

void TreeListCtrlColumnWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString TreeListCtrlColumnWrapper::GetWxClassName() const { return ""; }

void TreeListCtrlColumnWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << "<object class=\"wxTreeListCtrlCol\">"
             << "<label>" << wxCrafter::CDATA(GetName()) << "</label>"
             << "<width>" << PropertyInt(PROP_WIDTH, -1) << "</width>"
             << "<align>"
             << "wxALIGN_LEFT"
             << "</align>"
             << "<flags>" << PropertyString(PROP_COL_FLAGS, "0") << "</flags>"
             << "</object>";
    }
}
