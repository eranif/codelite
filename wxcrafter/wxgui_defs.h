#ifndef WXGUI_DEFS_H
#define WXGUI_DEFS_H

#include "json_node.h"
#include <wx/string.h>

class wxcWidget;
#define CHECK_POINTER(d) \
    if(!d) return;
#define CHECK_POINTER_RET_NULL(d) \
    if(!d) return NULL;
#define CHECK_POINTER_RET_FALSE(d) \
    if(!d) return false;
#define CHECK_TREEITEM(item) \
    if(item.IsOk() == false) return;

#define STRINGFY(flag) wxString(#flag, wxConvUTF8)

#define ADD_STYLE(styleBit, isSet)                          \
    {                                                       \
        WxStyleInfo style;                                  \
        style.is_set = isSet;                               \
        style.style_bit = styleBit;                         \
        style.style_name = wxString(#styleBit, wxConvUTF8); \
        m_styles.PushBack(style.style_name, style);         \
    }

#define PREPEND_STYLE(styleBit, isSet)                      \
    {                                                       \
        WxStyleInfo style;                                  \
        style.is_set = isSet;                               \
        style.style_bit = styleBit;                         \
        style.style_name = wxString(#styleBit, wxConvUTF8); \
        m_styles.PushFront(style.style_name, style);        \
    }

#define PREPEND_STYLE_STR(styleName, styleBit, isSet) \
    {                                                 \
        WxStyleInfo style;                            \
        style.is_set = isSet;                         \
        style.style_bit = styleBit;                   \
        style.style_name = styleName;                 \
        m_styles.PushFront(style.style_name, style);  \
    }

#define PREPEND_STYLE_FALSE(styleBit) PREPEND_STYLE_STR(wxT(#styleBit), styleBit, false)
#define PREPEND_STYLE_TRUE(styleBit) PREPEND_STYLE_STR(wxT(#styleBit), styleBit, true)

#define HAS_STYLE(styleBit) m_styles.Contains(wxString(#styleBit, wxConvUTF8))

#define REMOVE_STYLE(styleBit)                             \
    {                                                      \
        wxString name = wxString(#styleBit, wxConvUTF8);   \
        if(m_styles.Contains(name)) m_styles.Remove(name); \
    }

#define ADD_SIZER_FLAG(styleBit, isSet)                     \
    {                                                       \
        WxStyleInfo style;                                  \
        style.is_set = isSet;                               \
        style.style_bit = styleBit;                         \
        style.style_name = wxString(#styleBit, wxConvUTF8); \
        DoAddSizerFlag(style.style_name, style);            \
    }

#define GENERAL_PROPERTIES_NODE wxT("General Properties")

// Default values
#define DEFAULT_SIZER_PROPORTION 0
#define DEFAULT_SIZER_BORDER 5

#define DEFAULT_SIZER_PROPORTION_STR wxT("0")
#define DEFAULT_SIZER_BORDER_STR wxT("5")

/////////////////////////
// Toolbar IDs
/////////////////////////

#define ID_TOOL_ALIGN_LEFT 3800
#define ID_TOOL_ALIGN_HCENTER 3801
#define ID_TOOL_ALIGN_RIGHT 3802
#define ID_TOOL_ALIGN_TOP 3803
#define ID_TOOL_ALIGN_VCENTER 3804
#define ID_TOOL_ALIGN_BOTTOM 3805
#define ID_TOOL_BORDER_LEFT 3806
#define ID_TOOL_BORDER_TOP 3807
#define ID_TOOL_BORDER_RIGHT 3808
#define ID_TOOL_BORDER_BOTTOM 3809
#define ID_TOOL_BORDER_ALL 3810
#define ID_TOOL_WXEXPAND 3811
#define ID_TOOL_PROP1 3812

#define ID_OPEN_WXGUI_PROJECT 3850
#define ID_SAVE_WXGUI_PROJECT 3851

#define ID_MOVE_NODE_UP 3852
#define ID_MOVE_NODE_DOWN 3853
#define ID_MOVE_NODE_INTO_SIZER 3854
#define ID_MOVE_NODE_INTO_SIBLING 3855
#define ID_DELETE_NODE 3856
#define ID_WXGUI_PROJECT_SETTINGS 3857
#define ID_CUT 3858
#define ID_COPY 3859
#define ID_PASTE 3860
#define ID_RENAME 3861
#define ID_DUPLICATE 3862
#define ID_SHOW_PREVIEW 3863
#define ID_CANCEL_PREVIEW 3864

#define BASE_CLASS_SUFFIX wxT("BaseClass")

/////////////////////////
// Properties
/////////////////////////
#define PROP_TASKBAR_ICONTYPE _("Icon Type:")
#define PROP_BG _("Bg Colour:")
#define PROP_FG _("Fg Colour:")
#define PROP_FONT _("Font:")
#define PROP_WINDOW_ID _("ID:")
#define PROP_NAME _("Name:")
#define PROP_SUBCLASS_NAME _("Class Name:")
#define PROP_SUBCLASS_INCLUDE _("Include File:")
#define PROP_SUBCLASS_STYLE _("Style:")
#define PROP_STATE_HIDDEN _("Hidden")
#define PROP_STATE_DISABLED _("Disabled")
#define PROP_HAS_FOCUS _("Focused")
#define PROP_TOOLTIP _("Tooltip:")
#define PROP_LABEL _("Label:")
#define PROP_CENTRE_ON_SCREEN _("Centre:")
#define PROP_ACCELERATOR _("Shortcut:")
#define PROP_DEFAULT_BUTTON _("Default Button")
#define PROP_WRAP _("Wrap:")
#define PROP_COLS _("# Columns:")
#define PROP_COLS_LIST _("Columns:")
#define PROP_COL_LABEL_V_ALIGN _("Col Label Vertical Align")
#define PROP_COL_LABEL_H_ALIGN _("Col Label Horizontal Align")
#define PROP_ROWS_LIST _("Rows:")
#define PROP_ROW_LABEL_V_ALIGN _("Row Label Vertical Align")
#define PROP_ROW_LABEL_H_ALIGN _("Row Label Horizontal Align")
#define PROP_GRID_NATIVE_LOOK _("Use Native Header")
#define PROP_GRID_NATIVE_COL_LABELS _("Use Native Column Labels")
#define PROP_ROWS _("# Rows:")
#define PROP_GROW_COLS _("Growable columns:")
#define PROP_GROW_ROWS _("Growable rows:")
#define PROP_HGAP _("Horizontal gap:")
#define PROP_VGAP _("Vertical gap:")
#define PROP_VALUE _("Value:")
#define PROP_MAXLENGTH _("Max Length:")
#define PROP_AUTO_COMPLETE_DIRS _("Auto Complete Directories:")
#define PROP_AUTO_COMPLETE_FILES _("Auto Complete Files:")
#define PROP_ORIENTATION _("Orientation:")
#define PROP_MINSIZE _("Minimum Size:")
#define PROP_BITMAP_PATH _("Bitmap File:")
#define PROP_BITMAP_PATH_16 _("Bitmap File (16x16)  :")
#define PROP_BITMAP_PATH_32 _("Bitmap File (32x32)  :")
#define PROP_BITMAP_PATH_64 _("Bitmap File (64x64)  :")
#define PROP_BITMAP_PATH_128 _("Bitmap File (128x128):")
#define PROP_BITMAP_PATH_256 _("Bitmap File (256x256):")
#define PROP_DISABLED_BITMAP_PATH _("Disabled-Bitmap File")
#define PROP_OPTIONS _("Choices:")
#define PROP_CB_CHOICES _("ComboBox Choices:")
#define PROP_SELECTION _("Selection:")
#define PROP_SIZE _("Size:")
#define PROP_TITLE _("Title:")
#define PROP_PERSISTENT _("Enable Window Persistency:")
#define PROP_FILE _("File:")
#define PROP_VIRTUAL_FOLDER _("Virtual Folder:")
#define PROP_WIDTH _("Width:")
#define PROP_HEIGHT _("Height:")
#define PROP_MAJORDIM _("Major Dimension:")
#define PROP_MINVALUE _("Min value:")
#define PROP_MAXVALUE _("Max value:")
#define PROP_RANGE _("Range:")
#define PROP_URL _("URL:")
#define PROP_HTMLCODE _("HTML Code:")
#define PROP_AUTOSIZE_COL _("Fit Content to Columns")
#define PROP_ALLOW_EDITING _("Allow Cell Editing")
#define PROP_CHECKED _("Checked")
#define PROP_SHOW_CANCEL_BTN _("Show Cancel Button")
#define PROP_SHOW_SEARCH_BTN _("Show Search Button")
#define PROP_MESSAGE _("Message:")
#define PROP_WILDCARD _("Wildcard:")
#define PROP_THUMBSIZE _("Thumbsize:")
#define PROP_PAGESIZE _("Page Size:")
#define PROP_NORMAL_COLOR _("Normal Colour:")
#define PROP_VISITED_COLOR _("Visited Colour:")
#define PROP_HOVER_COLOR _("Hover Colour:")
#define PROP_DEFAULT_FOLDER _("Default Path:")
#define PROP_DEFAULT_FILTER _("Filter Index:")
#define PROP_FILTER _("Filter:")
#define PROP_SHOW_HIDDEN _("Show Hidden Files:")
#define PROP_SCROLL_RATE_X _("Scroll Rate X:")
#define PROP_SCROLL_RATE_Y _("Scroll Rate Y:")
#define PROP_SELECTED _("Selected")
#define PROP_EXPANDED _("Expand Node")
#define PROP_NULL_BOOK_PAGE _("Null Page")
#define PROP_SPLIT_MODE _("Split Mode:")
#define PROP_SASH_GRAVITY _("Sash Gravity:")
#define PROP_MIN_PANE_SIZE _("Minimum Pane Size:")
#define PROP_SASH_POS _("Sash Position:")
#define PROP_SPLITTER_LEFT _("Set Splitter Left:")
#define PROP_CELLPOS _("Cell Position:")
#define PROP_CELLSPAN _("Cell Span:")
#define PROP_KIND _("Kind:")
#define PROP_HELP _("Help String:")
#define PROP_MARGINS _("Margins:")
#define PROP_BITMAP_SIZE _("Bitmap Size:")
#define PROP_FIELD_COUNT _("Field Count:")
#define PROP_PADDING _("Padding:")
#define PROP_SEPARATOR_SIZE _("Separator Size:")
#define PROP_STC_MARGIN_LINE_NUMBERS _("Line Number Margin")
#define PROP_STC_MARGIN_SYMBOL _("Symbol Margin")
#define PROP_STC_MARGIN_FOLD _("Fold Margin")
#define PROP_STC_MARGIN_SEPARATOR _("Separator Margin")
#define PROP_STC_LEXER _("Lexer")
#define PROP_STC_WRAP _("Wrap Text")
#define PROP_STC_INDENT_GUIDES _("Indentation Guides")
#define PROP_STC_VIEW_EOL _("Display EOL Markers")
#define PROP_STC_EOL_MODE _("EOL Mode")
#define PROP_CLASS_DECORATOR _("Class Decorator")
#define PROP_DV_LISTCTRL_COL_TYPES _("Column Type")
#define PROP_DV_LISTCTRL_COL_ALIGN _("Alignment")
#define PROP_DV_MODEL_CLASS_NAME _("Generated Model Class Name")
#define PROP_DV_CELLMODE _("Cell Mode")
#define PROP_DV_COLFLAGS _("Column Flags")
#define PROP_DV_CONTAINER_ITEM_HAS_COLUMNS _("Container Item Has Columns")
#define PROP_COLOR_GRADIENT_START _("Gradient Start")
#define PROP_COLOR_GRADIENT_END _("Gradient End")
#define PROP_NOTE _("Note")
#define PROP_COLLAPSED _("Collapsed")
#define PROP_INFOBAR_ICON_ID _("Icon ID")
#define PROP_INFOBAR_BTN_ID _("Button ID")
#define PROP_AUI_SASH_SIZE _("Sash Size")
#define PROP_AUI_PANE_BORDER_SIZE _("Pane Border Size")
#define PROP_AUI_SASH_COLOUR _("Sash Colour")
#define PROP_AUI_CAPTION_COLOUR _("Caption Colour")
#define PROP_AUI_CAPTION_COLOUR_GRADIENT _("Caption Gradient Colour")
#define PROP_AUI_INACTIVE_CAPTION_COLOUR _("Inactive Caption Colour")
#define PROP_AUI_INACTIVE_CAPTION_COLOUR_GRADIENT _("Inactive Caption Gradient Colour")
#define PROP_AUI_ACTIVE_CAPTION_TEXT_COLOUR _("Active Caption Text Colour")
#define PROP_AUI_INACTIVE_CAPTION_TEXT_COLOUR _("Inactive Caption Text Colour")
#define PROP_AUI_GRADIENT_TYPE _("Gradient Type")
#define PROP_IMGLIST_MASK _("Bitmap Mask")
#define PROP_BASE_CLASS_SUFFIX _("Base Class Suffix")
#define PROP_INTERVAL _("Interval")
#define PROP_START_TIMER _("Start the timer")
#define PROP_ONE_SHOT_TIMER _("One Shot Timer")
#define PROP_OUTPUT_FILE_NAME _("Output File Name")
#define PROP_INHERITED_CLASS _("Inherited Class")
#define PROP_KEYWORDS_SET_1 _("Keywords Set 1")
#define PROP_KEYWORDS_SET_2 _("Keywords Set 2")
#define PROP_KEYWORDS_SET_3 _("Keywords Set 3")
#define PROP_KEYWORDS_SET_4 _("Keywords Set 4")
#define PROP_KEYWORDS_SET_5 _("Keywords Set 5")
#define PROP_CONTROL_SPECIFIC_SETTINGS _("Control Specific Settings")
#define PROP_RIBBON_THEME _("Ribbon Theme")
#define PROP_RIBBON_TOOLBAR_MIN_ROWS _("Minimum #Rows")
#define PROP_RIBBON_TOOLBAR_MAX_ROWS _("Maximum #Rows")
#define PROP_MC_BACKENDNAME _("Backend")
#define PROP_MC_CONTROLS _("wxMediaCtrl Controls")
#define PROP_MC_NO_CONTROLS "wxMEDIACTRLPLAYERCONTROLS_NONE"
#define PROP_MC_DEFAULT_CONTROLS "wxMEDIACTRLPLAYERCONTROLS_DEFAULT"
#define PROP_FRAME_TYPE "wxFrame Type"
#define PROP_COL_FLAGS "Column Flags"
#define PROP_EFFECT "Show Effect"
#define PROP_CUSTOM_EDITOR _("Property Editor Control")
#define PROP_DROPDOWN_MENU _("Construct the Dropdown Menu:")
#define PROP_HINT _("Text Hint")
#define PROP_DIRECTION _("Direction")
#define PROP_ANIM_AUTO_PLAY _("Load and play")
///////////////////////////////////////////////////////////////////////////////////////

struct WxStyleInfo {
    wxString style_name;
    int style_bit;
    bool is_set;
    wxArrayString style_group;

    bool operator==(const WxStyleInfo& src) const { return style_bit == src.style_bit; }

    bool operator<(const WxStyleInfo& src) const { return style_bit < src.style_bit; }

    /**
     * @brief check 'styleAsString' and update it depending on the style_group
     * @param styleAsString comma delimited styles
     */
    void UpdateStyleString(wxString& styleAsString) const;
    bool IsGroupConditionMet(wxcWidget* widget) const;
    JSONElement ToJSON() const
    {
        JSONElement json = JSONElement::createObject();
        json.addProperty(wxT("style_name"), style_name);
        json.addProperty(wxT("style_bit"), style_bit);
        json.addProperty(wxT("is_set"), is_set);
        return json;
    }

    void FromJSON(const JSONElement& json)
    {
        style_name = json.namedObject(wxT("style_name")).toString();
        style_bit = json.namedObject(wxT("style_bit")).toInt();
        is_set = json.namedObject(wxT("is_set")).toBool();
    }
};
/////////////////////////////////////////////////////////////////////////////////////////

#endif // WXGUI_DEFS_H
