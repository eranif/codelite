#ifndef WXGUI_DEFS_H
#define WXGUI_DEFS_H

#include "json_node.h"

#include <wx/string.h>

class wxcWidget;
#define CHECK_POINTER(d) \
    if(!d)               \
        return;
#define CHECK_POINTER_RET_NULL(d) \
    if(!d)                        \
        return NULL;
#define CHECK_POINTER_RET_FALSE(d) \
    if(!d)                         \
        return false;
#define CHECK_TREEITEM(item) \
    if(item.IsOk() == false) \
        return;

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

#define REMOVE_STYLE(styleBit)                           \
    {                                                    \
        wxString name = wxString(#styleBit, wxConvUTF8); \
        if(m_styles.Contains(name))                      \
            m_styles.Remove(name);                       \
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
#define PROP_TASKBAR_ICONTYPE wxTRANSLATE("Icon Type:")
#define PROP_BG wxTRANSLATE("Bg Colour:")
#define PROP_FG wxTRANSLATE("Fg Colour:")
#define PROP_FONT wxTRANSLATE("Font:")
#define PROP_WINDOW_ID wxTRANSLATE("ID:")
#define PROP_NAME wxTRANSLATE("Name:")
#define PROP_SUBCLASS_NAME wxTRANSLATE("Class Name:")
#define PROP_SUBCLASS_INCLUDE wxTRANSLATE("Include File:")
#define PROP_SUBCLASS_STYLE wxTRANSLATE("Style:")
#define PROP_STATE_HIDDEN wxTRANSLATE("Hidden")
#define PROP_STATE_DISABLED wxTRANSLATE("Disabled")
#define PROP_HAS_FOCUS wxTRANSLATE("Focused")
#define PROP_TOOLTIP wxTRANSLATE("Tooltip:")
#define PROP_LABEL wxTRANSLATE("Label:")
#define PROP_CENTRE_ON_SCREEN wxTRANSLATE("Centre:")
#define PROP_ACCELERATOR wxTRANSLATE("Shortcut:")
#define PROP_DEFAULT_BUTTON wxTRANSLATE("Default Button")
#define PROP_WRAP wxTRANSLATE("Wrap:")
#define PROP_COLS wxTRANSLATE("# Columns:")
#define PROP_COLS_LIST wxTRANSLATE("Columns:")
#define PROP_COL_LABEL_V_ALIGN wxTRANSLATE("Col Label Vertical Align")
#define PROP_COL_LABEL_H_ALIGN wxTRANSLATE("Col Label Horizontal Align")
#define PROP_ROWS_LIST wxTRANSLATE("Rows:")
#define PROP_ROW_LABEL_V_ALIGN wxTRANSLATE("Row Label Vertical Align")
#define PROP_ROW_LABEL_H_ALIGN wxTRANSLATE("Row Label Horizontal Align")
#define PROP_GRID_NATIVE_LOOK wxTRANSLATE("Use Native Header")
#define PROP_GRID_NATIVE_COL_LABELS wxTRANSLATE("Use Native Column Labels")
#define PROP_ROWS wxTRANSLATE("# Rows:")
#define PROP_GROW_COLS wxTRANSLATE("Growable columns:")
#define PROP_GROW_ROWS wxTRANSLATE("Growable rows:")
#define PROP_HGAP wxTRANSLATE("Horizontal gap:")
#define PROP_VGAP wxTRANSLATE("Vertical gap:")
#define PROP_VALUE wxTRANSLATE("Value:")
#define PROP_MAXLENGTH wxTRANSLATE("Max Length:")
#define PROP_AUTO_COMPLETE_DIRS wxTRANSLATE("Auto Complete Directories:")
#define PROP_AUTO_COMPLETE_FILES wxTRANSLATE("Auto Complete Files:")
#define PROP_ORIENTATION wxTRANSLATE("Orientation:")
#define PROP_MINSIZE wxTRANSLATE("Minimum Size:")
#define PROP_BITMAP_PATH wxTRANSLATE("Bitmap File:")
#define PROP_BITMAP_PATH_16 wxTRANSLATE("Bitmap File (16x16)  :")
#define PROP_BITMAP_PATH_32 wxTRANSLATE("Bitmap File (32x32)  :")
#define PROP_BITMAP_PATH_64 wxTRANSLATE("Bitmap File (64x64)  :")
#define PROP_BITMAP_PATH_128 wxTRANSLATE("Bitmap File (128x128):")
#define PROP_BITMAP_PATH_256 wxTRANSLATE("Bitmap File (256x256):")
#define PROP_DISABLED_BITMAP_PATH wxTRANSLATE("Disabled-Bitmap File")
#define PROP_OPTIONS wxTRANSLATE("Choices:")
#define PROP_CB_CHOICES wxTRANSLATE("ComboBox Choices:")
#define PROP_SELECTION wxTRANSLATE("Selection:")
#define PROP_SIZE wxTRANSLATE("Size:")
#define PROP_TITLE wxTRANSLATE("Title:")
#define PROP_PERSISTENT wxTRANSLATE("Enable Window Persistency:")
#define PROP_FILE wxTRANSLATE("File:")
#define PROP_VIRTUAL_FOLDER wxTRANSLATE("Virtual Folder:")
#define PROP_WIDTH wxTRANSLATE("Width:")
#define PROP_HEIGHT wxTRANSLATE("Height:")
#define PROP_MAJORDIM wxTRANSLATE("Major Dimension:")
#define PROP_MINVALUE wxTRANSLATE("Min value:")
#define PROP_MAXVALUE wxTRANSLATE("Max value:")
#define PROP_RANGE wxTRANSLATE("Range:")
#define PROP_URL wxTRANSLATE("URL:")
#define PROP_HTMLCODE wxTRANSLATE("HTML Code:")
#define PROP_AUTOSIZE_COL wxTRANSLATE("Fit Content to Columns")
#define PROP_ALLOW_EDITING wxTRANSLATE("Allow Cell Editing")
#define PROP_CHECKED wxTRANSLATE("Checked")
#define PROP_SHOW_CANCEL_BTN wxTRANSLATE("Show Cancel Button")
#define PROP_SHOW_SEARCH_BTN wxTRANSLATE("Show Search Button")
#define PROP_MESSAGE wxTRANSLATE("Message:")
#define PROP_WILDCARD wxTRANSLATE("Wildcard:")
#define PROP_THUMBSIZE wxTRANSLATE("Thumbsize:")
#define PROP_PAGESIZE wxTRANSLATE("Page Size:")
#define PROP_NORMAL_COLOR wxTRANSLATE("Normal Colour:")
#define PROP_VISITED_COLOR wxTRANSLATE("Visited Colour:")
#define PROP_HOVER_COLOR wxTRANSLATE("Hover Colour:")
#define PROP_DEFAULT_FOLDER wxTRANSLATE("Default Path:")
#define PROP_DEFAULT_FILTER wxTRANSLATE("Filter Index:")
#define PROP_FILTER wxTRANSLATE("Filter:")
#define PROP_SHOW_HIDDEN wxTRANSLATE("Show Hidden Files:")
#define PROP_SCROLL_RATE_X wxTRANSLATE("Scroll Rate X:")
#define PROP_SCROLL_RATE_Y wxTRANSLATE("Scroll Rate Y:")
#define PROP_SELECTED wxTRANSLATE("Selected")
#define PROP_EXPANDED wxTRANSLATE("Expand Node")
#define PROP_NULL_BOOK_PAGE wxTRANSLATE("Null Page")
#define PROP_SPLIT_MODE wxTRANSLATE("Split Mode:")
#define PROP_SASH_GRAVITY wxTRANSLATE("Sash Gravity:")
#define PROP_MIN_PANE_SIZE wxTRANSLATE("Minimum Pane Size:")
#define PROP_SASH_POS wxTRANSLATE("Sash Position:")
#define PROP_SPLITTER_LEFT wxTRANSLATE("Set Splitter Left:")
#define PROP_CELLPOS wxTRANSLATE("Cell Position:")
#define PROP_CELLSPAN wxTRANSLATE("Cell Span:")
#define PROP_KIND wxTRANSLATE("Kind:")
#define PROP_HELP wxTRANSLATE("Help String:")
#define PROP_MARGINS wxTRANSLATE("Margins:")
#define PROP_BITMAP_SIZE wxTRANSLATE("Bitmap Size:")
#define PROP_FIELD_COUNT wxTRANSLATE("Field Count:")
#define PROP_PADDING wxTRANSLATE("Padding:")
#define PROP_SEPARATOR_SIZE wxTRANSLATE("Separator Size:")
#define PROP_STC_MARGIN_LINE_NUMBERS wxTRANSLATE("Line Number Margin")
#define PROP_STC_MARGIN_SYMBOL wxTRANSLATE("Symbol Margin")
#define PROP_STC_MARGIN_FOLD wxTRANSLATE("Fold Margin")
#define PROP_STC_MARGIN_SEPARATOR wxTRANSLATE("Separator Margin")
#define PROP_STC_LEXER wxTRANSLATE("Lexer")
#define PROP_STC_WRAP wxTRANSLATE("Wrap Text")
#define PROP_STC_INDENT_GUIDES wxTRANSLATE("Indentation Guides")
#define PROP_STC_VIEW_EOL wxTRANSLATE("Display EOL Markers")
#define PROP_STC_EOL_MODE wxTRANSLATE("EOL Mode")
#define PROP_CLASS_DECORATOR wxTRANSLATE("Class Decorator")
#define PROP_DV_LISTCTRL_COL_TYPES wxTRANSLATE("Column Type")
#define PROP_DV_LISTCTRL_COL_ALIGN wxTRANSLATE("Alignment")
#define PROP_DV_MODEL_CLASS_NAME wxTRANSLATE("Generated Model Class Name")
#define PROP_DV_CELLMODE wxTRANSLATE("Cell Mode")
#define PROP_DV_COLFLAGS wxTRANSLATE("Column Flags")
#define PROP_DV_CONTAINER_ITEM_HAS_COLUMNS wxTRANSLATE("Container Item Has Columns")
#define PROP_COLOR_GRADIENT_START wxTRANSLATE("Gradient Start")
#define PROP_COLOR_GRADIENT_END wxTRANSLATE("Gradient End")
#define PROP_NOTE wxTRANSLATE("Note")
#define PROP_COLLAPSED wxTRANSLATE("Collapsed")
#define PROP_INFOBAR_ICON_ID wxTRANSLATE("Icon ID")
#define PROP_INFOBAR_BTN_ID wxTRANSLATE("Button ID")
#define PROP_AUI_SASH_SIZE wxTRANSLATE("Sash Size")
#define PROP_AUI_PANE_BORDER_SIZE wxTRANSLATE("Pane Border Size")
#define PROP_AUI_SASH_COLOUR wxTRANSLATE("Sash Colour")
#define PROP_AUI_CAPTION_COLOUR wxTRANSLATE("Caption Colour")
#define PROP_AUI_CAPTION_COLOUR_GRADIENT wxTRANSLATE("Caption Gradient Colour")
#define PROP_AUI_INACTIVE_CAPTION_COLOUR wxTRANSLATE("Inactive Caption Colour")
#define PROP_AUI_INACTIVE_CAPTION_COLOUR_GRADIENT wxTRANSLATE("Inactive Caption Gradient Colour")
#define PROP_AUI_ACTIVE_CAPTION_TEXT_COLOUR wxTRANSLATE("Active Caption Text Colour")
#define PROP_AUI_INACTIVE_CAPTION_TEXT_COLOUR wxTRANSLATE("Inactive Caption Text Colour")
#define PROP_AUI_GRADIENT_TYPE wxTRANSLATE("Gradient Type")
#define PROP_IMGLIST_MASK wxTRANSLATE("Bitmap Mask")
#define PROP_BASE_CLASS_SUFFIX wxTRANSLATE("Base Class Suffix")
#define PROP_INTERVAL wxTRANSLATE("Interval")
#define PROP_START_TIMER wxTRANSLATE("Start the timer")
#define PROP_ONE_SHOT_TIMER wxTRANSLATE("One Shot Timer")
#define PROP_OUTPUT_FILE_NAME wxTRANSLATE("Output File Name")
#define PROP_INHERITED_CLASS wxTRANSLATE("Inherited Class")
#define PROP_KEYWORDS_SET_1 wxTRANSLATE("Keywords Set 1")
#define PROP_KEYWORDS_SET_2 wxTRANSLATE("Keywords Set 2")
#define PROP_KEYWORDS_SET_3 wxTRANSLATE("Keywords Set 3")
#define PROP_KEYWORDS_SET_4 wxTRANSLATE("Keywords Set 4")
#define PROP_KEYWORDS_SET_5 wxTRANSLATE("Keywords Set 5")
#define PROP_CONTROL_SPECIFIC_SETTINGS wxTRANSLATE("Control Specific Settings")
#define PROP_RIBBON_THEME wxTRANSLATE("Ribbon Theme")
#define PROP_RIBBON_TOOLBAR_MIN_ROWS wxTRANSLATE("Minimum #Rows")
#define PROP_RIBBON_TOOLBAR_MAX_ROWS wxTRANSLATE("Maximum #Rows")
#define PROP_MC_BACKENDNAME wxTRANSLATE("Backend")
#define PROP_MC_CONTROLS wxTRANSLATE("wxMediaCtrl Controls")
#define PROP_MC_NO_CONTROLS "wxMEDIACTRLPLAYERCONTROLS_NONE"
#define PROP_MC_DEFAULT_CONTROLS "wxMEDIACTRLPLAYERCONTROLS_DEFAULT"
#define PROP_FRAME_TYPE wxTRANSLATE("wxFrame Type")
#define PROP_COL_FLAGS wxTRANSLATE("Column Flags")
#define PROP_EFFECT wxTRANSLATE("Show Effect")
#define PROP_CUSTOM_EDITOR wxTRANSLATE("Property Editor Control")
#define PROP_DROPDOWN_MENU wxTRANSLATE("Construct the Dropdown Menu:")
#define PROP_HINT wxTRANSLATE("Text Hint")
#define PROP_DIRECTION wxTRANSLATE("Direction")
#define PROP_ANIM_AUTO_PLAY wxTRANSLATE("Load and play")
#define PROP_PROPORTION wxTRANSLATE("Proportion:")
#define PROP_SPELLCHECK wxTRANSLATE("Enable Spell Checking")
#define PROP_KEEP_CLASS_MEMBER wxTRANSLATE("Keep as a class member")
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
