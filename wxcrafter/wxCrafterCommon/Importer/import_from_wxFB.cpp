#include "import_from_wxFB.h"

#include "wxgui_helpers.h"

namespace ImportFromwxFB
{

static std::map<wxString, wxString> sm_eventMap;

wxString ConvertFBOptionsString(const wxString& content, const wxString& separator /*= ";"*/)
{
    // wxFB stores the contents of e.g. a wxComboBox in the format:
    // <property name="choices">&quot;First string&quot; &quot;Second string&quot;</property>
    // We want: First string;Second string
    // (I thought the &quot;s would need decoding, but wx seems to cope)
    wxString options(content);
    options.Remove(0, 1).RemoveLast(1);  // Remove the unwanted first and last '"'
    options.Replace("\" \"", separator); // and replace " " with ;
    return options;
}

void ProcessBitmapProperty(const wxString& bitmapinfo,
                           wxcWidget* wrapper,
                           const wxString& property /*= PROP_BITMAP_PATH*/,
                           const wxString& client_hint /*=""*/)
{
    if (bitmapinfo.empty()) {
        return; // This isn't an error; the field may well be empty
    }

    // wxFB stores the contents of e.g. a wxComboBox in the format:
    //      <property name="bitmap">Load From Art Provider; wxART_GO_HOME; wxART_BUTTON</property>
    // or:  <property name="bitmap">Load From File; /full/path/to/foo.png</property>  *** Or vice versa!
    // or:  <property name="bitmap">Load From Embedded File; /full/path/to/foo.png</property>
    // or:  <property name="bitmap">Load From Resource; resourcename</property>
    // or:  <property name="bitmap">Load From Icon Resource; resourcename; [-1; -1]</property>
    wxArrayString arr =
        wxCrafter::Split(bitmapinfo, ";"); // ToDo: when we can load an icon resource, deal with the ';' in [-1; -1]!
    if (bitmapinfo.Contains("Load From File")) {
        wxString bitmappath;
        if (arr.Item(0).Contains("Load From File")) {
            bitmappath = arr.Item(1);
        } else if (arr.Item(1).Contains("Load From File")) {
            bitmappath = arr.Item(0);
        }
        if (!bitmappath.empty()) {
            PropertyBase* prop = wrapper->GetProperty(property); // Either PROP_BITMAP_PATH or PROP_DISABLED_BITMAP_PATH
            if (prop) {
                wxFileName fn(bitmappath.Trim(false)); // wxFB will probably have supplied a relative path
                if (fn.Normalize()) {
                    prop->SetValue(fn.GetFullPath());
                }
            }
        }
    }

    else if (bitmapinfo.Contains("Load From Art Provider")) {
        // "Load From Art Provider; wxART_GO_HOME; wxART_BUTTON"
        //  but we don't know for sure that the stock_client attribute will be present, or which order they'll be in.
        //  So:
        wxString idstring, clientstring, sizeHint;
        for (size_t n = 0; n < arr.GetCount(); ++n) {
            if (arr.Item(n).Contains("wxART_")) {
                // Could be e.g. wxART_GO_HOME, or the client hint e.g. wxART_BUTTON. Abuse IsArtProviderBitmap() to
                // find out which
                wxString artId, clientId, fake = arr.Item(n) + ",foo";
                if (wxCrafter::IsArtProviderBitmap(fake, artId, clientId, sizeHint)) {
                    idstring = arr.Item(n);
                } else {
                    clientstring = arr.Item(n);
                }
            }
        }
        if (!idstring.empty()) {
            PropertyBase* prop = wrapper->GetProperty(property);
            if (prop) {
                wxString artdata = idstring;
                if (clientstring.empty()) {
                    clientstring = client_hint; // Use any hint we were given
                }
                if (!clientstring.empty()) {
                    artdata << "," << clientstring;
                }
                prop->SetValue(artdata);
            }
        }
    }
    // More to follow when wxC does the other types
}

wxString GetEventtypeFromHandlerstub(const wxString& stub)
{
    // Despite the class name, this does XRC and wxSmith too. The 'duplicate' events are their way of labelling

    if (sm_eventMap.size() == 0) {
        // 'Common' eventtypes
        sm_eventMap.insert(std::pair<wxString, wxString>("OnKeyDown", "wxEVT_KEY_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_KEY_DOWN", "wxEVT_KEY_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnKeyUp", "wxEVT_KEY_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_KEY_UP", "wxEVT_KEY_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnChar", "wxEVT_CHAR"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CHAR", "wxEVT_CHAR"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnLeftDown", "wxEVT_LEFT_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LEFT_DOWN", "wxEVT_LEFT_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnLeftUp", "wxEVT_LEFT_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnLeftDClick", "wxEVT_LEFT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LEFT_DCLICK", "wxEVT_LEFT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnMiddleDown", "wxEVT_MIDDLE_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_MIDDLE_DOWN", "wxEVT_MIDDLE_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnMiddleUp", "wxEVT_MIDDLE_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnMiddleDClick", "wxEVT_MIDDLE_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_MIDDLE_DCLICK", "wxEVT_MIDDLE_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_RIGHT_DOWN", "wxEVT_RIGHT_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnRightDown", "wxEVT_RIGHT_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnRightUp", "wxEVT_RIGHT_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnRightDClick", "wxEVT_RIGHT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_RIGHT_DCLICK", "wxEVT_RIGHT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnMotion", "wxEVT_MOTION"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_MOTION", "wxEVT_MOTION"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnEnterWindow", "wxEVT_ENTER_WINDOW"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_ENTER_WINDOW", "wxEVT_ENTER_WINDOW"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnLeaveWindow", "wxEVT_LEAVE_WINDOW"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LEAVE_WINDOW", "wxEVT_LEAVE_WINDOW"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnMouseWheel", "wxEVT_MOUSEWHEEL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_MOUSEWHEEL", "wxEVT_MOUSEWHEEL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSetFocus", "wxEVT_SET_FOCUS"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SET_FOCUS", "wxEVT_SET_FOCUS"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnKillFocus", "wxEVT_KILL_FOCUS"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_KILL_FOCUS", "wxEVT_KILL_FOCUS"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnPaint", "wxEVT_PAINT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_PAINT", "wxEVT_PAINT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnEraseBackground", "wxEVT_ERASE_BACKGROUND"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_ERASE_BACKGROUND", "wxEVT_ERASE_BACKGROUND"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSize", "wxEVT_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SIZE", "wxEVT_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnUpdateUI", "wxEVT_UPDATE_UI"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_UPDATE_UI", "wxEVT_UPDATE_UI"));

        // Control-specific eventtypes
        sm_eventMap.insert(std::pair<wxString, wxString>("OnActivate", "wxEVT_ACTIVATE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnActivateApp", "wxEVT_ACTIVATE_APP"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnApplyButtonClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnAuiFindManager
                sm_eventMap.insert(std::pair<wxString, wxString>("OnAuiPaneButton
                sm_eventMap.insert(std::pair<wxString, wxString>("OnAuiPaneClose
                sm_eventMap.insert(std::pair<wxString, wxString>("OnAuiPaneMaximize
                sm_eventMap.insert(std::pair<wxString, wxString>("OnAuiPaneRestore
                sm_eventMap.insert(std::pair<wxString, wxString>("OnAuiRender*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_BUTTON", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCalendar", "wxEVT_CALENDAR_DOUBLECLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_DOUBLECLICKED", "wxEVT_CALENDAR_DOUBLECLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR", "wxEVT_CALENDAR_DOUBLECLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCalendarDay", "wxEVT_CALENDAR_DAY_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_DAY_CHANGED", "wxEVT_CALENDAR_DAY_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_DAY", "wxEVT_CALENDAR_DAY_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCalendarMonth", "wxEVT_CALENDAR_MONTH_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_MONTH_CHANGED", "wxEVT_CALENDAR_MONTH_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_MONTH", "wxEVT_CALENDAR_MONTH_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCalendarSelChanged", "wxEVT_CALENDAR_SEL_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_SEL_CHANGED", "wxEVT_CALENDAR_SEL_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCalendarWeekDayClicked", "wxEVT_CALENDAR_WEEKDAY_CLICKED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CALENDAR_WEEKDAY_CLICKED", "wxEVT_CALENDAR_WEEKDAY_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCalendarYear", "wxEVT_CALENDAR_YEAR_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_YEAR_CHANGED", "wxEVT_CALENDAR_YEAR_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CALENDAR_YEAR", "wxEVT_CALENDAR_YEAR_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCancelButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCheckBox", "wxEVT_COMMAND_CHECKBOX_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CHECKBOX", "wxEVT_COMMAND_CHECKBOX_CLICKED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnCheckListBox
                sm_eventMap.insert(std::pair<wxString, wxString>("OnCheckListBoxDClick*/
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnCheckListBoxToggled", "wxEVT_COMMAND_CHECKLISTBOX_TOGGLED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CHECKLISTBOX", "wxEVT_COMMAND_CHECKLISTBOX_TOGGLED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnChoice", "wxEVT_COMMAND_CHOICE_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CHOICE", "wxEVT_COMMAND_CHOICE_SELECTED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnChoicebookPageChanged", "wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnChoicebookPageChanging", "wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnClose", "wxEVT_CLOSE_WINDOW"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CLOSE", "wxEVT_CLOSE_WINDOW"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnColourChanged", "wxEVT_COMMAND_COLOURPICKER_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_COMMAND_COLOURPICKER_CHANGED", "wxEVT_COMMAND_COLOURPICKER_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_COLOURPICKER_CHANGED", "wxEVT_COMMAND_COLOURPICKER_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnFontChanged", "wxEVT_COMMAND_FONTPICKER_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_FONTPICKER_CHANGED", "wxEVT_COMMAND_FONTPICKER_CHANGED"));
        // sm_eventMap.insert(std::pair<wxString, wxString>("", "wxEVT_TIME_CHANGED")); wxTimePickerCtrl, which afaict
        // nobody implements atm
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCombobox", "wxEVT_COMMAND_COMBOBOX_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_COMBOBOX", "wxEVT_COMMAND_COMBOBOX_SELECTED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScroll*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollBottom", "wxEVT_SCROLL_BOTTOM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollChanged", "wxEVT_SCROLL_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollLineDown", "wxEVT_SCROLL_LINEDOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollLineUp", "wxEVT_SCROLL_LINEUP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollPageDown", "wxEVT_SCROLL_PAGEDOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollPageUp", "wxEVT_SCROLL_PAGEUP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollThumbRelease", "wxEVT_SCROLL_THUMBRELEASE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollThumbTrack", "wxEVT_SCROLL_THUMBTRACK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnCommandScrollTop", "wxEVT_SCROLL_TOP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnContextHelpButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnDateChanged", "wxEVT_DATE_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_DATE_CHANGED", "wxEVT_DATE_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCellChange", "wxEVT_GRID_CELL_CHANGE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_CELL_CHANGE", "wxEVT_GRID_CELL_CHANGE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_CELL_CHANGE", "wxEVT_GRID_CELL_CHANGE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCellLeftClick", "wxEVT_GRID_CELL_LEFT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_CELL_LEFT_CLICK", "wxEVT_GRID_CELL_LEFT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_CELL_LEFT_CLICK", "wxEVT_GRID_CELL_LEFT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCellLeftDClick", "wxEVT_GRID_CELL_LEFT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_CELL_LEFT_DCLICK", "wxEVT_GRID_CELL_LEFT_DCLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_CELL_LEFT_DCLICK", "wxEVT_GRID_CELL_LEFT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCellRightClick", "wxEVT_GRID_CELL_RIGHT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_CELL_RIGHT_CLICK", "wxEVT_GRID_CELL_RIGHT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_CELL_RIGHT_CLICK", "wxEVT_GRID_CELL_RIGHT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCellRightDClick", "wxEVT_GRID_CELL_RIGHT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_CELL_RIGHT_DCLICK", "wxEVT_GRID_CELL_RIGHT_DCLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_CELL_RIGHT_DCLICK", "wxEVT_GRID_CELL_RIGHT_DCLICK"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdCellChange
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdCellLeftClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdCellLeftDClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdCellRightClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdCellRightDClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdColSize*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdEditorCreated", "wxEVT_GRID_EDITOR_CREATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_EDITOR_CREATED", "wxEVT_GRID_EDITOR_CREATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_EDITOR_CREATED", "wxEVT_GRID_EDITOR_CREATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdEditorHidden", "wxEVT_GRID_EDITOR_HIDDEN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_EDITOR_HIDDEN", "wxEVT_GRID_EDITOR_HIDDEN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_EDITOR_HIDDEN", "wxEVT_GRID_EDITOR_HIDDEN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdEditorShown", "wxEVT_GRID_EDITOR_SHOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_EDITOR_SHOWN", "wxEVT_GRID_EDITOR_SHOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_EDITOR_SHOWN", "wxEVT_GRID_EDITOR_SHOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdLabelLeftClick", "wxEVT_GRID_LABEL_LEFT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_LABEL_LEFT_CLICK", "wxEVT_GRID_LABEL_LEFT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_LABEL_LEFT_CLICK", "wxEVT_GRID_LABEL_LEFT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdLabelLeftDClick", "wxEVT_GRID_LABEL_LEFT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_LABEL_LEFT_DCLICK", "wxEVT_GRID_LABEL_LEFT_DCLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_LABEL_LEFT_DCLICK", "wxEVT_GRID_LABEL_LEFT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdLabelRightClick", "wxEVT_GRID_LABEL_RIGHT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_LABEL_RIGHT_CLICK", "wxEVT_GRID_LABEL_RIGHT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_LABEL_RIGHT_CLICK", "wxEVT_GRID_LABEL_RIGHT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdLabelRightDClick", "wxEVT_GRID_LABEL_RIGHT_DCLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_GRID_LABEL_RIGHT_DCLICK", "wxEVT_GRID_LABEL_RIGHT_DCLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_CMD_GRID_LABEL_RIGHT_DCLICK", "wxEVT_GRID_LABEL_RIGHT_DCLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdRangeSelect", "wxEVT_GRID_RANGE_SELECT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_RANGE_SELECT", "wxEVT_GRID_RANGE_SELECT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_RANGE_SELECT", "wxEVT_GRID_RANGE_SELECT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdRowSize", "wxEVT_GRID_ROW_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_ROW_SIZE", "wxEVT_GRID_ROW_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_ROW_SIZE", "wxEVT_GRID_ROW_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridCmdSelectCell", "wxEVT_GRID_SELECT_CELL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_SELECT_CELL", "wxEVT_GRID_SELECT_CELL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_SELECT_CELL", "wxEVT_GRID_SELECT_CELL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridColSize", "wxEVT_GRID_COL_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_GRID_COL_SIZE", "wxEVT_GRID_COL_SIZE"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_CMD_GRID_COL_SIZE", "wxEVT_GRID_COL_SIZE"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnGridEditorCreated
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridEditorHidden
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridEditorShown
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridLabelLeftClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridLabelLeftDClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridLabelRightClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridLabelRightDClick
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridRangeSelect
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridRowSize
                sm_eventMap.insert(std::pair<wxString, wxString>("OnGridSelectCell*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnHelpButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnHibernate*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnHtmlCellClicked", "wxEVT_COMMAND_HTML_CELL_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_HTML_CELL_CLICKED", "wxEVT_COMMAND_HTML_CELL_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnHtmlCellHover", "wxEVT_COMMAND_HTML_CELL_HOVER"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_HTML_CELL_HOVER", "wxEVT_COMMAND_HTML_CELL_HOVER"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnHtmlLinkClicked", "wxEVT_COMMAND_HTML_LINK_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_HTML_LINK_CLICKED", "wxEVT_COMMAND_HTML_LINK_CLICKED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnIconize
                sm_eventMap.insert(std::pair<wxString, wxString>("OnIdle*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnInitDialog", "wxEVT_INIT_DIALOG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_INIT_DIALOG", "wxEVT_INIT_DIALOG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListBeginDrag", "wxEVT_COMMAND_LIST_BEGIN_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_BEGIN_DRAG", "wxEVT_COMMAND_LIST_BEGIN_DRAG"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnListBeginLabelEdit", "wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_BEGIN_LABEL_EDIT", "wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListBeginRDrag", "wxEVT_COMMAND_LIST_BEGIN_RDRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_BEGIN_RDRAG", "wxEVT_COMMAND_LIST_BEGIN_RDRAG"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnListbookPageChanged", "wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LISTBOOK_PAGE_CHANGED", "wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnListbookPageChanging", "wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LISTBOOK_PAGE_CHANGING", "wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListBox", "wxEVT_COMMAND_LISTBOX_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LISTBOX", "wxEVT_COMMAND_LISTBOX_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListBoxDClick", "wxEVT_COMMAND_LISTBOX_DOUBLECLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LISTBOX_DCLICK", "wxEVT_COMMAND_LISTBOX_DOUBLECLICKED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnListCacheHint
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_CACHE_HINT*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListColBeginDrag", "wxEVT_COMMAND_LIST_COL_BEGIN_DRAG"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_COL_BEGIN_DRAG", "wxEVT_COMMAND_LIST_COL_BEGIN_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListColClick", "wxEVT_COMMAND_LIST_COL_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_COL_CLICK", "wxEVT_COMMAND_LIST_COL_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListColDragging", "wxEVT_COMMAND_LIST_COL_DRAGGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_COL_DRAGGING", "wxEVT_COMMAND_LIST_COL_DRAGGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListColEndDrag", "wxEVT_COMMAND_LIST_COL_END_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_COL_END_DRAG", "wxEVT_COMMAND_LIST_COL_END_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListColRightClick", "wxEVT_COMMAND_LIST_COL_RIGHT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_COL_RIGHT_CLICK", "wxEVT_COMMAND_LIST_COL_RIGHT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnListDeleteAllItems", "wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_DELETE_ALL_ITEMS", "wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListDeleteItem", "wxEVT_COMMAND_LIST_DELETE_ITEM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_DELETE_ITEM", "wxEVT_COMMAND_LIST_DELETE_ITEM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListEndLabelEdit", "wxEVT_COMMAND_LIST_END_LABEL_EDIT"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_END_LABEL_EDIT", "wxEVT_COMMAND_LIST_END_LABEL_EDIT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListInsertItem", "wxEVT_COMMAND_LIST_INSERT_ITEM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_INSERT_ITEM", "wxEVT_COMMAND_LIST_INSERT_ITEM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListItemActivated", "wxEVT_COMMAND_LIST_ITEM_ACTIVATED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_ITEM_ACTIVATED", "wxEVT_COMMAND_LIST_ITEM_ACTIVATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListItemDeselected", "wxEVT_COMMAND_LIST_ITEM_DESELECTED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_ITEM_DESELECTED", "wxEVT_COMMAND_LIST_ITEM_DESELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListItemFocused", "wxEVT_COMMAND_LIST_ITEM_FOCUSED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_ITEM_FOCUSED", "wxEVT_COMMAND_LIST_ITEM_FOCUSED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnListItemMiddleClick", "wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_ITEM_MIDDLE_CLICK", "wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnListItemRightClick", "wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_LIST_ITEM_RIGHT_CLICK", "wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListItemSelected", "wxEVT_COMMAND_LIST_ITEM_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_ITEM_SELECTED", "wxEVT_COMMAND_LIST_ITEM_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnListKeyDown", "wxEVT_COMMAND_LIST_KEY_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_LIST_KEY_DOWN", "wxEVT_COMMAND_LIST_KEY_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnMenuSelection", "wxEVT_MENU"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_MENU", "wxEVT_MENU"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnNoButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnNotebookPageChanged", "wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_NOTEBOOK_PAGE_CHANGED", "wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnNotebookPageChanging", "wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_NOTEBOOK_PAGE_CHANGING", "wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnOKButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnRadioBox", "wxEVT_COMMAND_RADIOBOX_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_RADIOBOX", "wxEVT_COMMAND_RADIOBOX_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnRadioButton", "wxEVT_COMMAND_RADIOBUTTON_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_RADIOBUTTON", "wxEVT_COMMAND_RADIOBUTTON_SELECTED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSaveButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnScroll
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollBottom
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_BOTTOM
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollChanged
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_CHANGED
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollLineDown
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_LINEDOWN
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollLineUp
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_LINEUP
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollPageDown
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_PAGEDOWN
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollPageUp
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_PAGEUP
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollThumbRelease
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_THUMBRELEASE
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollThumbTrack
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_THUMBTRACK
                sm_eventMap.insert(std::pair<wxString, wxString>("OnScrollTop
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SCROLL_TOP*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSpin", "wxEVT_SPIN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SPIN", "wxEVT_SPIN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSpinCtrl", "wxEVT_COMMAND_SPINCTRL_UPDATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SPINCTRL", "wxEVT_COMMAND_SPINCTRL_UPDATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSpinCtrlText", "wxEVT_COMMAND_TEXT_UPDATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSpinDown", "wxEVT_SPIN_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SPIN_DOWN", "wxEVT_SPIN_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSpinUp", "wxEVT_SPIN_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SPIN_UP", "wxEVT_SPIN_UP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSplitterDClick", "wxEVT_COMMAND_SPLITTER_DOUBLECLICKED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_SPLITTER_DCLICK", "wxEVT_COMMAND_SPLITTER_DOUBLECLICKED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnSplitterSashPosChanged", "wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_SPLITTER_SASH_POS_CHANGED", "wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnSplitterSashPosChanging", "wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>(
            "EVT_SPLITTER_SASH_POS_CHANGING", "wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnSplitterUnsplit", "wxEVT_COMMAND_SPLITTER_UNSPLIT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_SPLITTER_UNSPLIT", "wxEVT_COMMAND_SPLITTER_UNSPLIT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnText", "wxEVT_COMMAND_TEXT_UPDATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TEXT", "wxEVT_COMMAND_TEXT_UPDATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTextEnter", "wxEVT_COMMAND_TEXT_ENTER"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TEXT_ENTER", "wxEVT_COMMAND_TEXT_ENTER"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTextMaxLen", "wxEVT_COMMAND_TEXT_MAXLEN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TEXT_MAXLEN", "wxEVT_COMMAND_TEXT_MAXLEN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTextURL", "wxEVT_COMMAND_TEXT_URL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TEXT_URL", "wxEVT_COMMAND_TEXT_URL"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnToggleButton", "wxEVT_COMMAND_TOGGLEBUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TOGGLEBUTTON", "wxEVT_COMMAND_TOGGLEBUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnToolClicked", "wxEVT_COMMAND_TOOL_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TOOL", "wxEVT_COMMAND_TOOL_CLICKED"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnToolEnter
                sm_eventMap.insert(std::pair<wxString, wxString>("OnToolRClicked*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeBeginDrag", "wxEVT_COMMAND_TREE_BEGIN_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_BEGIN_DRAG", "wxEVT_COMMAND_TREE_BEGIN_DRAG"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnTreeBeginLabelEdit", "wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_BEGIN_LABEL_EDIT", "wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeBeginRDrag", "wxEVT_COMMAND_TREE_BEGIN_RDRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_BEGIN_RDRAG", "wxEVT_COMMAND_TREE_BEGIN_RDRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeDeleteItem", "wxEVT_COMMAND_TREE_DELETE_ITEM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_DELETE_ITEM", "wxEVT_COMMAND_TREE_DELETE_ITEM"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeEndDrag", "wxEVT_COMMAND_TREE_END_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_END_DRAG", "wxEVT_COMMAND_TREE_END_DRAG"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeEndLabelEdit", "wxEVT_COMMAND_TREE_END_LABEL_EDIT"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_END_LABEL_EDIT", "wxEVT_COMMAND_TREE_END_LABEL_EDIT"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeGetInfo
                sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_GET_INFO*/
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemActivated", "wxEVT_COMMAND_TREE_ITEM_ACTIVATED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_ACTIVATED", "wxEVT_COMMAND_TREE_ITEM_ACTIVATED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemCollapsed", "wxEVT_COMMAND_TREE_ITEM_COLLAPSED"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_COLLAPSED", "wxEVT_COMMAND_TREE_ITEM_COLLAPSED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemCollapsing", "wxEVT_COMMAND_TREE_ITEM_COLLAPSING"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_COLLAPSING", "wxEVT_COMMAND_TREE_ITEM_COLLAPSING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemExpanded", "wxEVT_COMMAND_TREE_ITEM_EXPANDED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_ITEM_EXPANDED", "wxEVT_COMMAND_TREE_ITEM_EXPANDED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemExpanding", "wxEVT_COMMAND_TREE_ITEM_EXPANDING"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_EXPANDING", "wxEVT_COMMAND_TREE_ITEM_EXPANDING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemGetTooltip", "wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_GETTOOLTIP", "wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeItemMenu", "wxEVT_COMMAND_TREE_ITEM_MENU"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_ITEM_MENU", "wxEVT_COMMAND_TREE_ITEM_MENU"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnTreeItemMiddleClick", "wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_MIDDLE_CLICK", "wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnTreeItemRightClick", "wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_ITEM_RIGHT_CLICK", "wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeKeyDown", "wxEVT_COMMAND_TREE_KEY_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_KEY_DOWN", "wxEVT_COMMAND_TREE_KEY_DOWN"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeSelChanged", "wxEVT_COMMAND_TREE_SEL_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_SEL_CHANGED", "wxEVT_COMMAND_TREE_SEL_CHANGED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeSelChanging", "wxEVT_COMMAND_TREE_SEL_CHANGING"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_SEL_CHANGING", "wxEVT_COMMAND_TREE_SEL_CHANGING"));
        /*        sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeSetInfo
                sm_eventMap.insert(std::pair<wxString, wxString>("OnTreeSetInfo*/
        sm_eventMap.insert(
            std::pair<wxString, wxString>("OnTreeStateImageClick", "wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK"));
        sm_eventMap.insert(
            std::pair<wxString, wxString>("EVT_TREE_STATE_IMAGE_CLICK", "wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_TREE_SET_INFO", "wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnYesButtonClick", "wxEVT_COMMAND_BUTTON_CLICKED"));
        sm_eventMap.insert(std::pair<wxString, wxString>("OnHyperlink", "wxEVT_COMMAND_HYPERLINK"));
        sm_eventMap.insert(std::pair<wxString, wxString>("EVT_HYPERLINK", "wxEVT_COMMAND_HYPERLINK"));
    }

    std::map<wxString, wxString>::iterator iter = sm_eventMap.find(stub);

    return (iter != sm_eventMap.end()) ? iter->second : "";
}

} // namespace ImportFromwxFB
