#include "data_view_list_ctrl_column.h"

#include "allocator_mgr.h"
#include "bool_property.h"
#include "choice_property.h"
#include "col_header_flags_property.h"
#include "multi_strings_property.h"
#include "wxgui_helpers.h"

#include <wx/dataview.h>

DataViewListCtrlColumn::DataViewListCtrlColumn()
    : wxcWidget(ID_WXDATAVIEWCOL)
{
    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    wxArrayString coltype;
    coltype.Add("bitmap");
    coltype.Add("check");
    coltype.Add("text");
    coltype.Add("icontext");
    coltype.Add("progress");
    coltype.Add("choice");

    wxArrayString alignment;
    alignment.Add("wxALIGN_LEFT");
    alignment.Add("wxALIGN_RIGHT");
    alignment.Add("wxALIGN_CENTER");

    wxArrayString cellType;
    cellType.Add("wxDATAVIEW_CELL_INERT");
    cellType.Add("wxDATAVIEW_CELL_ACTIVATABLE");
    cellType.Add("wxDATAVIEW_CELL_EDITABLE");

    AddProperty(new CategoryProperty(_("wxDataViewListCtrl Column")));
    AddProperty(new StringProperty(PROP_NAME, _("My Column"), _("Column Caption")));
    AddProperty(new StringProperty(PROP_WIDTH, wxT("-2"),
                                   _("Column Width (in pixels)\n-1 - special value for column width meaning "
                                     "unspecified/default\n-2 - size the column automatically to fit all values")));
    AddProperty(new ChoiceProperty(PROP_DV_LISTCTRL_COL_TYPES, coltype, 2, _("Column Type")));
    AddProperty(new MultiStringsProperty(PROP_OPTIONS, _("Choices strings\nRelevant only for column of type 'choice'"),
                                         ";", "Enter choices"));
    AddProperty(new ChoiceProperty(PROP_DV_LISTCTRL_COL_ALIGN, alignment, 0, _("Cell Alignment")));
    AddProperty(
        new ChoiceProperty(PROP_DV_CELLMODE, cellType, 0, _("Cell mode (can be editable, activatable or inert)")));
    AddProperty(new ColHeaderFlagsProperty(
        PROP_DV_COLFLAGS, 0, _("One or more flags of the wxDataViewColumnFlags enumeration"), eColumnKind::kDataView));
}

DataViewListCtrlColumn::~DataViewListCtrlColumn() {}

wxcWidget* DataViewListCtrlColumn::Clone() const { return new DataViewListCtrlColumn(); }

wxString DataViewListCtrlColumn::CppCtorCode() const
{
    wxString cppCode;
    wxString parentName = GetParent()->GetName();
    wxString alignstring = PropertyString(PROP_DV_LISTCTRL_COL_ALIGN);
    wxString label = wxCrafter::UNDERSCORE(GetName());
    wxString coltype = PropertyString(PROP_DV_LISTCTRL_COL_TYPES);

    bool childOfDataViewListCtrl = GetParent()->GetType() == ID_WXDATAVIEWLISTCTRL;
    wxString cellMode = PropertyString(PROP_DV_CELLMODE);
    wxString colFlag = PropertyString(PROP_COL_FLAGS, "0");
    wxString columnWidth;
    columnWidth << "WXC_FROM_DIP(" << PropertyString(PROP_WIDTH) << ")";
    if(coltype == "bitmap") {
        cppCode << parentName << "->AppendBitmapColumn(" << label << ", " << parentName << "->GetColumnCount(), "
                << cellMode << ", " << columnWidth << ", " << alignstring << ", " << colFlag << ");";

    } else if(coltype == "check") {
        cppCode << parentName << "->AppendToggleColumn(" << label << ", ";
        if(!childOfDataViewListCtrl)
            cppCode << parentName << "->GetColumnCount(), ";
        cppCode << cellMode << ", " << columnWidth << ", " << alignstring << ", " << colFlag << ");";

    } else if(coltype == "text") {
        cppCode << parentName << "->AppendTextColumn(" << label << ", ";
        if(!childOfDataViewListCtrl)
            cppCode << parentName << "->GetColumnCount(), ";
        cppCode << cellMode << ", " << columnWidth << ", " << alignstring << ", " << colFlag << ");";

    } else if(coltype == "icontext") {
        cppCode << parentName << "->AppendIconTextColumn(" << label << ", ";
        if(!childOfDataViewListCtrl)
            cppCode << parentName << "->GetColumnCount(), ";
        cppCode << cellMode << ", " << columnWidth << ", " << alignstring << ", " << colFlag << ");";

    } else if(coltype == "progress") {
        cppCode << parentName << "->AppendProgressColumn(" << label << ", ";
        if(!childOfDataViewListCtrl)
            cppCode << parentName << "->GetColumnCount(), ";
        cppCode << cellMode << ", " << columnWidth << ", " << alignstring << ", " << colFlag << ");";
    } else if(coltype == "choice") {
        cppCode << "{\n";
        cppCode << "    wxArrayString choices;\n";

        wxArrayString choices = ::wxStringTokenize(PropertyString(PROP_OPTIONS), ";", wxTOKEN_STRTOK);
        for(size_t i = 0; i < choices.GetCount(); ++i) {
            cppCode << "    choices.Add(" << wxCrafter::UNDERSCORE(choices.Item(i)) << ");\n";
        }
        cppCode << "    " << parentName << "->AppendColumn( new wxDataViewColumn( " << label
                << ", new wxDataViewChoiceRenderer( choices, " << cellMode << ", wxDVR_DEFAULT_ALIGNMENT), "
                << parentName << "->GetColumnCount(), " << PropertyString(PROP_WIDTH) << ", " << alignstring << ", "
                << colFlag << "));";
        cppCode << "}\n";
    }
    return cppCode;
}

void DataViewListCtrlColumn::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString DataViewListCtrlColumn::GetWxClassName() const { return ""; }

void DataViewListCtrlColumn::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == wxcWidget::XRC_LIVE) {
        text << XRCUnknown();
    } else {
        text << wxT("<object class=\"wxDataViewColumn\">") << wxT("<coltype>")
             << PropertyString(PROP_DV_LISTCTRL_COL_TYPES) << wxT("</coltype>") << wxT("<width>")
             << PropertyString(PROP_WIDTH) << wxT("</width>") << wxT("<label>") << wxCrafter::CDATA(GetName())
             << wxT("</label>") << wxT("<align>") << PropertyString(PROP_DV_LISTCTRL_COL_ALIGN) << wxT("</align>")
             << wxT("<cellmode>") << PropertyString(PROP_DV_CELLMODE) << wxT("</cellmode>") << wxT("<choices>")
             << wxCrafter::XMLEncode(PropertyString(PROP_OPTIONS)) << wxT("</choices>") << wxT("</object>");
    }
}
