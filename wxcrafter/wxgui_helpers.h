#ifndef _WXGUI_HELPERS_H_
#define _WXGUI_HELPERS_H_

#include <event_notifier.h>
#include <macros.h>
#include <set>
#include <vector>
#include <wx/colour.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/frame.h>

#define ITEM_NORMAL wxT("normal")
#define ITEM_RADIO wxT("radio")
#define ITEM_CHECK wxT("checkable")
#define ITEM_SPACE wxT("space")
#define ITEM_SEPARATOR wxT("separator")
#define ITEM_DROPDOWN wxT("dropdown")
//#define ITEM_AUILABEL  wxT("label") Not currently needed

class wxXmlNode;

namespace wxCrafter
{
enum TOOL_TYPE {
    TOOL_TYPE_NORMAL,
    TOOL_TYPE_RADIO,
    TOOL_TYPE_CHECK,
    TOOL_TYPE_SEPARATOR,
    TOOL_TYPE_SPACE,
    TOOL_TYPE_DROPDOWN,        // Toolbar only
    TOOL_TYPE_NONSTRETCHSPACE, // AuiToolbar only
    TOOL_TYPE_AUILABEL         // AuiToolbar only
};

wxSize DecodeSize(const wxString& strSize);
wxString EncodeSize(const wxSize& size);
wxString GetSizeAsDlgUnits(const wxSize& size, const wxString& parent);
wxFileName LoadXRC(const wxString& xrcString, const wxString& filename, wxString& caption, wxString& style,
                   wxString& bmppath);
int ToNumber(const wxString& str, int defaultValue);
float ToFloat(const wxString& str, float defaultValue);
wxString FloatToCString(float f);
wxString ToString(int n);
/**
 * @brief split string of numbers separated by comma
 */
wxArrayString SplitByString(const wxString& str, const wxString& delim, bool keepEmptyLines = false);
wxArrayString Split(const wxString& str, const wxString& delim, wxStringTokenizerMode mode = wxTOKEN_STRTOK);
wxString Join(const wxArrayString& arr, const wxString& delim);
/**
 * @brief attempt to make a 'CamelCase' string form str
 */
wxString CamelCase(const wxString& str);

wxString WXT(const wxString& s);
wxString ESCAPE(const wxString& s);
wxString UNDERSCORE(const wxString& s);
wxString FontToString(const wxFont& font);
wxFont StringToFont(const wxString& font);
bool IsSystemFont(const wxString& font);
wxString FontToXRC(const wxString& font);
wxString XRCToFontstring(const wxXmlNode* propertynode);
wxString FBToFontstring(const wxString& FBstr);
wxString FontToCpp(const wxString& font, const wxString& fontMemberName);
wxString XMLEncode(const wxString& text, bool decode = false);
wxString ToBool(const wxString& text);
wxString CDATA(const wxString& text);
wxArrayString MakeUnique(const wxArrayString& arr);
wxString AddQuotes(const wxString& str);
void MakeAbsToProject(wxFileName& fn);

/**
 * @brief convert windows slashes to unix paths
 */
wxString ToUnixPath(const wxString& path);

// ------------------ colors -------------------------------

wxString ColourToCpp(const wxString& guiname);
/**
 * @brief convert color name to XRC valid color
 */
wxString GetColourForXRC(const wxString& name);

wxColour NameToColour(const wxString& name);

/**
 * @brief convert color string (of unknown format) to a string suitable for ColorProperty
 */
wxString ValueToColourString(const wxString& value);

/**
 * @brief convert color name to system index, return -1 if not found
 */
int GetColourSysIndex(const wxString& name);

// ------------------ colors end -------------------------------

void WriteFile(const wxFileName& filename, const wxString& content, bool overwriteContent);
/**
 * @brief compare two text files
 */
bool IsTheSame(const wxFileName& f1, const wxFileName& f2);
/**
 * @brief return true of file content is same as the one on disk
 */
bool IsTheSame(const wxString& fileContent, const wxFileName& f);

std::set<wxString> VectorToSet(const std::vector<wxFileName>& v);

wxArrayString GetToolTypes(bool forToolbar = false);

TOOL_TYPE GetToolType(const wxString& name);

/**
 * @brief convert TOOL_TYPE to wx TYPE
 * @param type
 * @return
 */
wxString ToolTypeToWX(TOOL_TYPE type);

wxString GetUserDataDir();

wxString GetConfigFile();

/**
 * @brief set status bar message at column 0
 */
void SetStatusMessage(const wxString& msg);

/**
 * @brief print wx29 preprocessor start condition
 * @return
 */
wxString WX294_BLOCK_START();

/**
 *
 * @brief print wx3.0 preprocessor start condition
 * @return
 */
wxString WX30_BLOCK_START();

/**
 *
 * @brief print wx3.1 preprocessor start condition
 * @return
 */
wxString WX31_BLOCK_START();

/**
 *
 * @brief print wx2.9 preprocessor start condition
 * @return
 */
wxString WX29_BLOCK_START();

/**
 * @brief print wx29 preprocessor end condition
 * @return
 */
wxString WXVER_CHECK_BLOCK_END();

int ToAligment(const wxString& aligment);

/**
 * @brief convert column flag from string to int
 * enum  {
 *  wxCOL_RESIZABLE = 1,
 *  wxCOL_SORTABLE = 2,
 *  wxCOL_REORDERABLE = 4,
 *  wxCOL_HIDDEN = 8,
 *  wxCOL_DEFAULT_FLAGS = wxCOL_RESIZABLE | wxCOL_REORDERABLE
 * }
 */
int ColumnFlagsFromString(const wxString& col_flags);

bool IsArtProviderBitmap(const wxString& bmpString, wxString& artId, wxString& clientID, wxString& sizeHint);

/**
 * @brief convert 'size' into a string that can be used in C++ code.
 * For example:
 * '16' -> wxSize(16, 16)
 * '16,15' -> wxSize(16, 15)
 * wxDefaultSize -> wxDefaultSize
 */
wxString MakeWxSizeStr(const wxString& size);

/*
 * wxSHOW_EFFECT_NONE
 * wxSHOW_EFFECT_ROLL_TO_LEFT
 * wxSHOW_EFFECT_ROLL_TO_RIGHT
 * wxSHOW_EFFECT_ROLL_TO_TOP
 * wxSHOW_EFFECT_ROLL_TO_BOTTOM
 * wxSHOW_EFFECT_SLIDE_TO_LEFT
 * wxSHOW_EFFECT_SLIDE_TO_RIGHT
 * wxSHOW_EFFECT_SLIDE_TO_TOP
 * wxSHOW_EFFECT_SLIDE_TO_BOTTOM
 * wxSHOW_EFFECT_BLEND
 * wxSHOW_EFFECT_EXPAND
 */
wxShowEffect ShowEffectFromString(const wxString& effect);

/**
 * @brief notify codelite about file save
 * @param fn the file that was saved/generated
 */
void NotifyFileSaved(const wxFileName& fn);

/**
 * @brief wrap block of code with #if <...> #endif
 * @param condname
 * @param codeblock
 */
void WrapInIfBlock(const wxString& condname, wxString& codeblock);

/**
 * @brief return the workspace files as set
 */
void GetWorkspaceFiles(wxStringSet_t& files);
/**
 * @brief return the workspace files as set
 */
void GetProjectFiles(const wxString& projectName, wxStringSet_t& files);

/**
 * @brief format source code string
 */
void FormatString(wxString& content, const wxFileName& filename);

/**
 * @brief format 
 * @param content
 * @param filename
 */
void FormatFile(const wxFileName& filename);

/**
 * @brief return the top frame
 */
wxWindow* TopFrame();

/**
 * @brief set the top frame
 */
void SetTopFrame(wxWindow* frame);
} // namespace wxCrafter

#endif // _WXGUI_HELPERS_H_
