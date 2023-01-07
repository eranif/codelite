#include "grid_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "choice_property.h"
#include "grid_column_wrapper.h"
#include "grid_row_wrapper.h"
#include "multi_strings_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

GridWrapper::GridWrapper()
    : wxcWidget(ID_WXGRID)
{
    EnableStyle(wxT("wxWANTS_CHARS"), true);

    wxArrayString vOpts, hOpts;
    vOpts.Add("wxALIGN_TOP");
    vOpts.Add("wxALIGN_CENTRE");
    vOpts.Add("wxALIGN_BOTTOM");

    hOpts.Add("wxALIGN_LEFT");
    hOpts.Add("wxALIGN_CENTRE");
    hOpts.Add("wxALIGN_RIGHT");

    SetPropertyString(_("Common Settings"), "wxGrid");
    AddProperty(new CategoryProperty(_("wxGrid Header")));
    AddProperty(new BoolProperty(PROP_AUTOSIZE_COL, true, _("Auto size column content to fit the column header")));
    AddProperty(
        new BoolProperty(PROP_GRID_NATIVE_LOOK, true, _("Enable the use of native header window for column labels")));
    // AddProperty(new BoolProperty(PROP_GRID_NATIVE_COL_LABELS, true, _("Call this in order to make the column labels
    // use a native look")));

    AddProperty(new CategoryProperty(_("wxGrid Columns Labels")));
    AddInteger(PROP_HEIGHT, _("Sets the height of the column label"), -1);
    AddProperty(new ChoiceProperty(PROP_COL_LABEL_H_ALIGN, hOpts, 1, _("Sets the horizontal alignment of the label")));
    AddProperty(new ChoiceProperty(PROP_COL_LABEL_V_ALIGN, vOpts, 1, _("Sets the vertical alignment of the label")));

    AddProperty(new CategoryProperty(_("wxGrid Rows Labels")));
    AddInteger(PROP_WIDTH, _("Sets the width of the row labels"), -1);
    AddProperty(new ChoiceProperty(PROP_ROW_LABEL_H_ALIGN, hOpts, 2, _("Sets the horizontal alignment of the label")));
    AddProperty(new ChoiceProperty(PROP_ROW_LABEL_V_ALIGN, vOpts, 1, _("Sets the vertical alignment of the label")));

    AddProperty(new CategoryProperty(_("wxGrid Cells")));
    AddProperty(new BoolProperty(PROP_ALLOW_EDITING, true, _("Allow editing grid content")));

    RegisterEvent(wxT("wxEVT_GRID_CELL_CHANGING"), wxT("wxGridEvent"),
                  _("The user is about to change the data in a cell. The new cell value as string is available from "
                    "GetString() event object method. This event can be vetoed if the change is not allowed. "
                    "Processes a wxEVT_GRID_CELL_CHANGING event type"));
    RegisterEvent(wxT("wxEVT_GRID_CELL_CHANGED"), wxT("wxGridEvent"),
                  _("The user changed the data in a cell. The old cell value as string is available from GetString() "
                    "event object method. Notice that vetoing this event still works for backwards compatibility "
                    "reasons but any new code should only veto EVT_GRID_CELL_CHANGING event and not this one. "
                    "Processes a wxEVT_GRID_CELL_CHANGED event type."));
    RegisterEvent(wxT("wxEVT_GRID_CELL_LEFT_CLICK"), wxT("wxGridEvent"),
                  _("The user clicked a cell with the left mouse button. Processes a wxEVT_GRID_CELL_LEFT_CLICK."));
    RegisterEvent(
        wxT("wxEVT_GRID_CELL_LEFT_DCLICK"), wxT("wxGridEvent"),
        _("The user double-clicked a cell with the left mouse button. Processes a wxEVT_GRID_CELL_LEFT_DCLICK"));
    RegisterEvent(wxT("wxEVT_GRID_CELL_RIGHT_CLICK"), wxT("wxGridEvent"),
                  _("The user clicked a cell with the right mouse button. Processes a wxEVT_GRID_CELL_RIGHT_CLICK."));
    RegisterEvent(
        wxT("wxEVT_GRID_CELL_RIGHT_DCLICK"), wxT("wxGridEvent"),
        _("The user double-clicked a cell with the right mouse button. Processes a wxEVT_GRID_CELL_RIGHT_DCLICK."));
    RegisterEvent(wxT("wxEVT_GRID_EDITOR_HIDDEN"), wxT("wxGridEvent"),
                  _("The editor for a cell was hidden. Processes a wxEVT_GRID_EDITOR_HIDDEN event type."));
    RegisterEvent(wxT("wxEVT_GRID_EDITOR_SHOWN"), wxT("wxGridEvent"),
                  _("The editor for a cell was shown. Processes a wxEVT_GRID_EDITOR_SHOWN event type"));
    RegisterEvent(wxT("wxEVT_GRID_LABEL_LEFT_CLICK"), wxT("wxGridEvent"),
                  _("The user clicked a label with the left mouse button. Processes a wxEVT_GRID_LABEL_LEFT_CLICK."));
    RegisterEvent(
        wxT("wxEVT_GRID_LABEL_LEFT_DCLICK"), wxT("wxGridEvent"),
        _("The user double-clicked a label with the left mouse button. Processes a wxEVT_GRID_LABEL_LEFT_DCLICK."));
    RegisterEvent(
        wxT("wxEVT_GRID_LABEL_RIGHT_CLICK"), wxT("wxGridEvent"),
        _("The user clicked a label with the right mouse button. Processes a wxEVT_GRID_LABEL_RIGHT_CLICK."));
    RegisterEvent(
        wxT("wxEVT_GRID_LABEL_RIGHT_DCLICK"), wxT("wxGridEvent"),
        _("The user double-clicked a label with the right mouse button. Processes a wxEVT_GRID_LABEL_RIGHT_DCLICK."));
    RegisterEvent(wxT("wxEVT_GRID_SELECT_CELL"), wxT("wxGridEvent"),
                  _("The user moved to, and selected a cell. Processes a wxEVT_GRID_SELECT_CELL."));
    RegisterEvent(
        wxT("wxEVT_GRID_COL_MOVE"), wxT("wxGridEvent"),
        _("The user tries to change the order of the columns in the grid by dragging the column specified by GetCol(). "
          "This event can be vetoed to either prevent the user from reordering the column change completely (but notice "
          "that if you don't want to allow it at all, you simply shouldn't call wxGrid::EnableDragColMove() in the first "
          "place), "
          "vetoed but handled in some way in the handler, e.g. by really moving the column to the new position at the "
          "associated table level, "
          "or allowed to proceed in which case wxGrid::SetColPos() is used to reorder the columns display order without "
          "affecting the use of "
          "the column indices otherwise. This event macro corresponds to wxEVT_GRID_COL_MOVE event type."));
    RegisterEvent(wxT("wxEVT_GRID_COL_SORT"), wxT("wxGridEvent"),
                  _("This event is generated when a column is clicked by the user and its name is explained by the fact "
                    "that the custom reaction to a "
                    "click on a column is to sort the grid contents by this column. However the grid itself has no "
                    "special support for sorting and it's "
                    "up to the handler of this event to update the associated table. But if the event is handled (and "
                    "not vetoed) the grid supposes that "
                    "the table was indeed resorted and updates the column to indicate the new sort order and refreshes "
                    "itself. This event macro corresponds "
                    "to wxEVT_GRID_COL_SORT event type."));
    RegisterEvent(wxT("wxEVT_GRID_TABBING"), wxT("wxGridEvent"),
                  _("This event is generated when the user presses TAB or Shift-TAB in the grid. It can be used to "
                    "customize the simple default TAB handling "
                    "logic, e.g. to go to the next non-empty cell instead of just the next cell. See also "
                    "wxGrid::SetTabBehaviour()."
                    " This event is new since wxWidgets 2.9.5."));
    m_namePattern = wxT("m_grid");
    SetName(GenerateName());
}

GridWrapper::~GridWrapper() {}

wxcWidget* GridWrapper::Clone() const { return new GridWrapper(); }

wxString GridWrapper::CppCtorCode() const
{
    wxString cppCode, cppRowColCode;
    cppCode << CPPStandardWxCtor(wxT("wxWANTS_CHARS"));

    // We need to count the number of children of type Col/Row we have
    GridColumnWrapper::Vector_t cols;
    GridRowWrapper::Vector_t rows;

    GetRowsCols(rows, cols);

    for(size_t i = 0; i < rows.size(); ++i) {
        cppRowColCode << rows.at(i)->Code(i);
    }

    for(size_t i = 0; i < cols.size(); ++i) {
        cppRowColCode << cols.at(i)->Code(i);
    }

    cppCode << GetName() << wxT("->CreateGrid(" << rows.size() << ", ") << cols.size() << wxT(");\n");
    cppCode << cppRowColCode;

    // set the aligment
    cppCode << GetName() << "->SetRowLabelAlignment(" << PropertyString(PROP_ROW_LABEL_H_ALIGN) << ", "
            << PropertyString(PROP_ROW_LABEL_V_ALIGN) << ");\n";
    cppCode << GetName() << "->SetColLabelAlignment(" << PropertyString(PROP_COL_LABEL_H_ALIGN) << ", "
            << PropertyString(PROP_COL_LABEL_V_ALIGN) << ");\n";
    cppCode << wxCrafter::WX294_BLOCK_START();
    cppCode << GetName() << "->UseNativeColHeader(" << PropertyBool(PROP_GRID_NATIVE_LOOK) << ");\n";
    cppCode << wxCrafter::WXVER_CHECK_BLOCK_END();
    cppCode << GetName() << wxT("->EnableEditing(") << PropertyBool(PROP_ALLOW_EDITING) << ");\n";

    int colHeight = PropertyInt(PROP_HEIGHT, -1);
    if(colHeight != -1) { cppCode << GetName() << "->SetColLabelSize(" << colHeight << ");\n"; }

    int rowLabelWidth = PropertyInt(PROP_WIDTH, -1);
    if(rowLabelWidth != -1) { cppCode << GetName() << "->SetRowLabelSize(" << rowLabelWidth << ");\n"; }
    return cppCode;
}

void GridWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/grid.h>")); }

wxString GridWrapper::GetWxClassName() const { return wxT("wxGrid"); }

void GridWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        // Native wx do not support what we do...
        text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << XRCSuffix();

    } else {

        GridColumnWrapper::Vector_t cols;
        GridRowWrapper::Vector_t rows;
        GetRowsCols(rows, cols);

        // wxBORDER_THEME seems to be breaking the preview when wxGrid is placed inside
        // a wxDialog. So we disable it for the preview only
#ifdef __WXGTK__
        wxString style = StyleFlags();

        wxArrayString styleArr = wxCrafter::Split(style, "|");
        if(styleArr.Index("wxBORDER_THEME") != wxNOT_FOUND) { styleArr.Remove("wxBORDER_THEME"); }
        style = wxCrafter::Join(styleArr, "|");
        style.Prepend("<style>").Append("</style>");
#else
        wxString style = XRCStyle();
#endif

        text << XRCPrefix() << XRCSize() << style << XRCCommonAttributes();
        text << wxT("<autosizecol>") << PropertyString(PROP_AUTOSIZE_COL) << wxT("</autosizecol>");
        text << wxT("<editable>") << PropertyString(PROP_ALLOW_EDITING) << wxT("</editable>");

        // Columns
        text << wxT("<columns>");
        for(size_t i = 0; i < cols.size(); ++i) {
            cols.at(i)->ToXRC(text, type);
        }
        text << wxT("</columns>");

        // Rows
        text << wxT("<rows>");
        for(size_t i = 0; i < rows.size(); ++i) {
            rows.at(i)->ToXRC(text, type);
        }
        text << wxT("</rows>");

        // alignment
        text << "<row-label-v-alignment>" << PropertyString(PROP_ROW_LABEL_V_ALIGN) << "</row-label-v-alignment>";
        text << "<row-label-h-alignment>" << PropertyString(PROP_ROW_LABEL_H_ALIGN) << "</row-label-h-alignment>";

        text << "<col-label-v-alignment>" << PropertyString(PROP_COL_LABEL_V_ALIGN) << "</col-label-v-alignment>";
        text << "<col-label-h-alignment>" << PropertyString(PROP_COL_LABEL_H_ALIGN) << "</col-label-h-alignment>";
        text << "<col-label-size>" << PropertyInt(PROP_HEIGHT) << "</col-label-size>";
        text << "<row-label-size>" << PropertyInt(PROP_WIDTH) << "</row-label-size>";

        text << "<native-header>" << PropertyString(PROP_GRID_NATIVE_LOOK) << "</native-header>";
        // text << "<native-col-labels>" << PropertyString(PROP_GRID_NATIVE_COL_LABELS) << "</native-col-labels>";
        text << XRCSuffix();
    }
}

void GridWrapper::UnSerialize(const JSONElement& json)
{
    m_sizerItem.SetBorder(json.namedObject(wxT("border")).toInt(5));
    m_sizerItem.SetProportion(json.namedObject(wxT("proportion")).toInt(0));
    m_gbSpan = json.namedObject(wxT("gbSpan")).toString();
    m_gbPos = json.namedObject(wxT("gbPosition")).toString();

    m_auiPaneInfo.FromJSON(json.namedObject("wxAuiPaneInfo"));

    // Unserialize the styles
    DoClearFlags(m_styles);
    DoClearFlags(m_sizerFlags);

    m_connectedEvents.Clear();

    JSONElement styles = json.namedObject(wxT("m_styles"));
    int nCount = styles.arraySize();
    for(int i = 0; i < nCount; i++) {
        wxString styleName = styles.arrayItem(i).toString();
        EnableStyle(styleName, true);
    }

    JSONElement sizerFlags = json.namedObject(wxT("m_sizerFlags"));
    nCount = sizerFlags.arraySize();
    for(int i = 0; i < nCount; i++) {
        wxString styleName = sizerFlags.arrayItem(i).toString();
        EnableSizerFlag(styleName, true);
    }

    // Unserialize the properties
    JSONElement properties = json.namedObject(wxT("m_properties"));
    nCount = properties.arraySize();

    for(int i = 0; i < nCount; i++) {
        JSONElement jsonProp = properties.arrayItem(i);
        wxString propLabel = jsonProp.namedObject(wxT("m_label")).toString();

        if(propLabel == PROP_COLS_LIST) {
            wxString cols_string = jsonProp.namedObject(wxT("m_value")).toString();
            wxArrayString colsArr = wxCrafter::Split(cols_string, ";");

            for(size_t i = 0; i < colsArr.GetCount(); ++i) {
                // The old way of creating rows - create new children and add them here
                wxcWidget* col = Allocator::Instance()->Create(ID_WXGRIDCOL);
                col->SetName(colsArr.Item(i));
                AddChild(col);
            }

        } else if(propLabel == PROP_ROWS_LIST) {
            wxString rows_string = jsonProp.namedObject(wxT("m_value")).toString();
            wxArrayString rowsArr = wxCrafter::Split(rows_string, ";");

            for(size_t i = 0; i < rowsArr.GetCount(); ++i) {
                // The old way of creating rows - create new children and add them here
                wxcWidget* row = Allocator::Instance()->Create(ID_WXGRIDROW);
                row->SetName(rowsArr.Item(i));
                AddChild(row);
            }

        } else if(m_properties.Contains(propLabel)) {
            m_properties.Item(propLabel)->UnSerialize(jsonProp);
        }
    }

    // Unserialize the events
    JSONElement events = json.namedObject(wxT("m_events"));
    nCount = events.arraySize();
    for(int i = 0; i < nCount; i++) {
        JSONElement jsonEvent = events.arrayItem(i);
        ConnectDetails details;
        details.FromJSON(jsonEvent);
        m_connectedEvents.PushBack(details.GetEventName(), details);
    }

    JSONElement children = json.namedObject(wxT("m_children"));
    int nChildren = children.arraySize();
    for(int i = 0; i < nChildren; i++) {
        JSONElement child = children.arrayItem(i);
        wxcWidget* wrapper = Allocator::Instance()->CreateWrapperFromJSON(child);
        if(wrapper) { AddChild(wrapper); }
    }
}

void GridWrapper::GetRowsCols(GridRowWrapper::Vector_t& rows, GridColumnWrapper::Vector_t& cols) const
{
    wxcWidget::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        if((*iter)->GetType() == ID_WXGRIDCOL) {
            cols.push_back(static_cast<GridColumnWrapper*>((*iter)));
        } else {
            rows.push_back(static_cast<GridRowWrapper*>((*iter)));
        }
    }
}
