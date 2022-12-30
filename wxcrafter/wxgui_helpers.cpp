#include "wxgui_helpers.h"

#include "file_logger.h"
#include "main.h"
#include "wxc_project_metadata.h"
#include "xmlutils.h"

#include <algorithm>
#include <cl_command_event.h>
#include <event_notifier.h>
#include <macros.h>
#include <map>
#include <plugin.h>
#include <project.h>
#include <workspace.h>
#include <wx/app.h>
#include <wx/arrstr.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/headercol.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/propgrid.h>
#include <wx/regex.h>
#include <wx/settings.h>
#include <wx/sstream.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

namespace
{
wxFont GetSystemFont(const wxString& name)
{
    // name is in the format of name,style,weight,underlined
    if(!name.IsEmpty()) {
        wxArrayString parts = ::wxStringTokenize(name, wxT(","), wxTOKEN_STRTOK);
        if(parts.IsEmpty())
            return wxNullFont;

        wxFont font;
        wxString name, style, weight, underlined;
        name = parts.Item(0);

        if(parts.GetCount() > 1)
            style = parts.Item(1);

        if(parts.GetCount() > 2)
            weight = parts.Item(2);

        if(parts.GetCount() > 3)
            underlined = parts.Item(3);

        if(name == wxT("wxSYS_OEM_FIXED_FONT"))
            font = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);

        else if(name == wxT("wxSYS_ANSI_FIXED_FONT")) {
            font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            font.SetFamily(wxFONTFAMILY_TELETYPE);
        }

        else if(name == wxT("wxSYS_ANSI_VAR_FONT"))
            font = wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT);
        else if(name == wxT("wxSYS_SYSTEM_FONT"))
            font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
        else if(name == wxT("wxSYS_DEVICE_DEFAULT_FONT"))
            font = wxSystemSettings::GetFont(wxSYS_DEVICE_DEFAULT_FONT);
        else if(name == wxT("wxSYS_SYSTEM_FIXED_FONT"))
            font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT);
        else if(name == wxT("wxSYS_DEFAULT_GUI_FONT"))
            font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        else
            return wxNullFont;

        if(style == wxT("italic"))
            font.SetStyle(wxFONTSTYLE_ITALIC);
        else
            font.SetStyle(wxFONTSTYLE_NORMAL);

        if(weight == wxT("bold"))
            font.SetWeight(wxFONTWEIGHT_BOLD);
        else
            font.SetWeight(wxFONTWEIGHT_NORMAL);

        if(underlined == wxT("underlined"))
            font.SetUnderlined(true);
        else
            font.SetUnderlined(false);
        return font;
    }
    return wxNullFont;
}

wxFontFamily StringToFontFamily(const wxString& str)
{
    wxFontFamily family = wxFONTFAMILY_DEFAULT;
    if(str == wxT("decorative"))
        family = wxFONTFAMILY_DECORATIVE;

    if(str == wxT("roman"))
        family = wxFONTFAMILY_ROMAN;

    if(str == wxT("script"))
        family = wxFONTFAMILY_SCRIPT;

    if(str == wxT("swiss"))
        family = wxFONTFAMILY_SWISS;

    if(str == wxT("modern"))
        family = wxFONTFAMILY_MODERN;

    if(str == wxT("teletype"))
        family = wxFONTFAMILY_TELETYPE;

    return family;
}

wxFontStyle StringToFontStyle(const wxString& str)
{
    wxFontStyle style = wxFONTSTYLE_NORMAL;
    if((str == wxT("italic")) || (wxCrafter::ToNumber(str, -1) == wxFONTSTYLE_ITALIC)) {
        style = wxFONTSTYLE_ITALIC;
    } else if((str == wxT("slant")) || (wxCrafter::ToNumber(str, -1) == wxFONTSTYLE_SLANT)) {
        style = wxFONTSTYLE_SLANT;
    }

    return style;
}

wxFontWeight StringToFontWeight(const wxString& str)
{
    wxFontWeight weight = wxFONTWEIGHT_NORMAL;
    if((str == wxT("bold")) || (wxCrafter::ToNumber(str, -1) == wxFONTWEIGHT_BOLD)) {
        weight = wxFONTWEIGHT_BOLD;
    } else if((str == wxT("light")) || (wxCrafter::ToNumber(str, -1) == wxFONTWEIGHT_LIGHT)) {
        weight = wxFONTWEIGHT_LIGHT;
    }

    return weight;
}
} // namespace

wxBorder wxCrafter::GetControlBorder()
{
#if wxVERSION_NUMBER >= 3300 && defined(__WXMSW__)
    return wxSystemSettings::GetAppearance().IsDark() ? wxBORDER_SIMPLE : wxBORDER_STATIC;
#else
    return wxBORDER_DEFAULT;
#endif
}

wxSize wxCrafter::DecodeSize(const wxString& strSize)
{
    // Strip any braces
    wxString s = strSize;
    s.Replace(wxT("("), wxT(""));
    s.Replace(wxT(")"), wxT(""));
    s.Replace(wxT("wxSize"), wxT(""));

    wxString strWidth = s.BeforeFirst(wxT(',')).Trim().Trim(false);
    wxString strHeight = s.AfterFirst(wxT(',')).Trim().Trim(false);

    long x(-1), y(-1);
    strWidth.ToLong(&x);
    strHeight.ToLong(&y);
    return wxSize(x, y);
}

wxString wxCrafter::ToUnixPath(const wxString& path)
{
    wxString p(path);
    p.Replace("\\", "/");
    return p;
}

wxString wxCrafter::EncodeSize(const wxSize& size)
{
    wxString str;
    str << size.x << wxT(",") << size.y;
    return str;
}

wxString wxCrafter::GetSizeAsDlgUnits(const wxSize& size, const wxString& parent)
{
    wxString str;
    str << "wxDLG_UNIT(" << parent << ", wxSize(" << size.x << wxT(",") << size.y << "))";
    return str;
}

wxFileName wxCrafter::LoadXRC(const wxString& xrcString, const wxString& filename, wxString& caption, wxString& style,
                              wxString& bmppath)
{
    {
        wxXmlDocument doc;
        wxStringInputStream inStr(xrcString);
        if(doc.Load(inStr)) {
            wxXmlNode* root = doc.GetRoot();
            if(root) {
                wxXmlNode* child = root->GetChildren();
                while(child) {
                    if(child->GetName() == "object" && child->GetAttribute("name") == "PreviewPanel") {
                        // we got the main view
                        wxXmlNode* captionNode = XmlUtils::FindFirstByTagName(child, "caption-title");
                        if(captionNode) {
                            caption = captionNode->GetNodeContent();
                        }

                        wxXmlNode* tlwStyle = XmlUtils::FindFirstByTagName(child, "tlw-style");
                        if(tlwStyle) {
                            style = tlwStyle->GetNodeContent();
                        }

                        wxXmlNode* tlwIcon = XmlUtils::FindFirstByTagName(child, "tlw-icon");
                        if(tlwIcon) {
                            bmppath = tlwIcon->GetNodeContent();
                        }

                        break;
                    }
                    child = child->GetNext();
                }
            }
        }
    }

    wxString fmtStr;
#ifndef NDEBUG
    wxXmlDocument doc;
    wxStringOutputStream str(&fmtStr);
    wxStringInputStream inStr(xrcString);

    if(!doc.Load(inStr)) {
        ::wxMessageBox(_("Invalid XRC! could not save DesignerPreview.xrc"), wxT("wxCrafter"), wxOK | wxICON_WARNING);
        return wxFileName(GetUserDataDir(), filename);
    }
    doc.Save(str);
#else
    fmtStr = xrcString;
#endif

    wxFFile fp(wxFileName(GetUserDataDir(), filename).GetFullPath(), wxT("w+b"));
    if(fp.IsOpened()) {
        fp.Write(fmtStr, wxConvUTF8);
        fp.Close();
    }

    wxFileName xrcFile = wxFileName(GetUserDataDir(), filename);
    wxXmlResource::Get()->Load(xrcFile.GetFullPath());
    return xrcFile;
}

int wxCrafter::ToNumber(const wxString& str, int defaultValue)
{
    long num(defaultValue);
    if(str.ToLong(&num)) {
        return num;
    } else {
        return defaultValue;
    }
}

wxString wxCrafter::ToString(int n)
{
    wxString str;
    str << n;
    return str;
}

wxArrayString wxCrafter::SplitByString(const wxString& str, const wxString& delim, bool keepEmptyLines)
{
    wxArrayString arr;
    wxString tmp = str;
    int where = tmp.Find(delim);

    while(where != wxNOT_FOUND) {
        wxString token = tmp.Mid(0, where);
        if(!keepEmptyLines) {
            token.Trim().Trim(false);
            if(!token.IsEmpty())
                arr.Add(token);
        } else {
            arr.Add(token);
        }

        tmp = tmp.Mid(where + delim.length()); // Skip the delimiter
        where = tmp.Find(delim);
    }

    if(tmp.IsEmpty() == false)
        arr.Add(tmp);

    return arr;
}

wxString wxCrafter::Join(const wxArrayString& arr, const wxString& delim)
{
    wxString res;
    for(size_t i = 0; i < arr.GetCount(); i++) {
        wxString item = arr.Item(i);
        item.Trim().Trim(false);
        res << item << delim;
    }

    if(res.IsEmpty() == false) {
        res.RemoveLast(delim.Len());
    }
    return res;
}

wxString wxCrafter::CamelCase(const wxString& str)
{
#ifndef __WXMAC__
    int re_flags = wxRE_ADVANCED;
#else
    int re_flags = wxRE_DEFAULT;
#endif

    wxString tmpstr = str;
    // Convert CamelCase to '_' separated string
    static wxRegEx re(wxT("([a-z])([A-Z])"), re_flags);
    while(re.IsValid() && re.Matches(tmpstr)) {
        re.Replace(&tmpstr, wxT("\\1_\\2"));
    }

    wxArrayString parts = wxCrafter::Split(tmpstr, wxT("_"));
    wxString fooname;
    for(size_t i = 0; i < parts.GetCount(); i++) {
        parts.Item(i).MakeLower();
        wxString firstChar = parts.Item(i).at(0);
        firstChar.MakeUpper();
        parts.Item(i).at(0) = firstChar.at(0);
        fooname << parts.Item(i);
    }
    return fooname;
}

static wxArrayString s_colorNames;
static wxArrayString s_colorSysNames;
static wxArrayInt s_colorIdx;

static void init_color_indexes()
{
    if(s_colorNames.IsEmpty()) {
        s_colorNames.Add("AppWorkspace");
        s_colorNames.Add("ActiveBorder");
        s_colorNames.Add("ActiveCaption");
        s_colorNames.Add("ButtonFace");
        s_colorNames.Add("ButtonHighlight");
        s_colorNames.Add("ButtonShadow");
        s_colorNames.Add("ButtonText");
        s_colorNames.Add("CaptionText");
        s_colorNames.Add("ControlDark");
        s_colorNames.Add("ControlLight");
        s_colorNames.Add("Desktop");
        s_colorNames.Add("GrayText");
        s_colorNames.Add("Highlight");
        s_colorNames.Add("HighlightText");
        s_colorNames.Add("InactiveBorder");
        s_colorNames.Add("InactiveCaption");
        s_colorNames.Add("InactiveCaptionText");
        s_colorNames.Add("Menu");
        s_colorNames.Add("Scrollbar");
        s_colorNames.Add("Tooltip");
        s_colorNames.Add("TooltipText");
        s_colorNames.Add("Window");
        s_colorNames.Add("WindowFrame");
        s_colorNames.Add("WindowText");
        s_colorNames.Add("Custom");
    }

    if(s_colorIdx.IsEmpty()) {
        s_colorIdx.Add(wxSYS_COLOUR_APPWORKSPACE);
        s_colorIdx.Add(wxSYS_COLOUR_ACTIVEBORDER);
        s_colorIdx.Add(wxSYS_COLOUR_ACTIVECAPTION);
        s_colorIdx.Add(wxSYS_COLOUR_BTNFACE);
        s_colorIdx.Add(wxSYS_COLOUR_BTNHIGHLIGHT);
        s_colorIdx.Add(wxSYS_COLOUR_BTNSHADOW);
        s_colorIdx.Add(wxSYS_COLOUR_BTNTEXT);
        s_colorIdx.Add(wxSYS_COLOUR_CAPTIONTEXT);
        s_colorIdx.Add(wxSYS_COLOUR_3DDKSHADOW);
        s_colorIdx.Add(wxSYS_COLOUR_3DLIGHT);
        s_colorIdx.Add(wxSYS_COLOUR_BACKGROUND);
        s_colorIdx.Add(wxSYS_COLOUR_GRAYTEXT);
        s_colorIdx.Add(wxSYS_COLOUR_HIGHLIGHT);
        s_colorIdx.Add(wxSYS_COLOUR_HIGHLIGHTTEXT);
        s_colorIdx.Add(wxSYS_COLOUR_INACTIVEBORDER);
        s_colorIdx.Add(wxSYS_COLOUR_INACTIVECAPTION);
        s_colorIdx.Add(wxSYS_COLOUR_INACTIVECAPTIONTEXT);
        s_colorIdx.Add(wxSYS_COLOUR_MENU);
        s_colorIdx.Add(wxSYS_COLOUR_SCROLLBAR);
        s_colorIdx.Add(wxSYS_COLOUR_INFOBK);
        s_colorIdx.Add(wxSYS_COLOUR_INFOTEXT);
        s_colorIdx.Add(wxSYS_COLOUR_WINDOW);
        s_colorIdx.Add(wxSYS_COLOUR_WINDOWFRAME);
        s_colorIdx.Add(wxSYS_COLOUR_WINDOWTEXT);
        s_colorIdx.Add(wxPG_COLOUR_CUSTOM);
    }

    if(s_colorSysNames.IsEmpty()) {
        s_colorSysNames.Add("wxSYS_COLOUR_APPWORKSPACE");
        s_colorSysNames.Add("wxSYS_COLOUR_ACTIVEBORDER");
        s_colorSysNames.Add("wxSYS_COLOUR_ACTIVECAPTION");
        s_colorSysNames.Add("wxSYS_COLOUR_BTNFACE");
        s_colorSysNames.Add("wxSYS_COLOUR_BTNHIGHLIGHT");
        s_colorSysNames.Add("wxSYS_COLOUR_BTNSHADOW");
        s_colorSysNames.Add("wxSYS_COLOUR_BTNTEXT");
        s_colorSysNames.Add("wxSYS_COLOUR_CAPTIONTEXT");
        s_colorSysNames.Add("wxSYS_COLOUR_3DDKSHADOW");
        s_colorSysNames.Add("wxSYS_COLOUR_3DLIGHT");
        s_colorSysNames.Add("wxSYS_COLOUR_BACKGROUND");
        s_colorSysNames.Add("wxSYS_COLOUR_GRAYTEXT");
        s_colorSysNames.Add("wxSYS_COLOUR_HIGHLIGHT");
        s_colorSysNames.Add("wxSYS_COLOUR_HIGHLIGHTTEXT");
        s_colorSysNames.Add("wxSYS_COLOUR_INACTIVEBORDER");
        s_colorSysNames.Add("wxSYS_COLOUR_INACTIVECAPTION");
        s_colorSysNames.Add("wxSYS_COLOUR_INACTIVECAPTIONTEXT");
        s_colorSysNames.Add("wxSYS_COLOUR_MENU");
        s_colorSysNames.Add("wxSYS_COLOUR_SCROLLBAR");
        s_colorSysNames.Add("wxSYS_COLOUR_INFOBK");
        s_colorSysNames.Add("wxSYS_COLOUR_INFOTEXT");
        s_colorSysNames.Add("wxSYS_COLOUR_WINDOW");
        s_colorSysNames.Add("wxSYS_COLOUR_WINDOWFRAME");
        s_colorSysNames.Add("wxSYS_COLOUR_WINDOWTEXT");
    }
}

wxString wxCrafter::ColourToCpp(const wxString& guiname)
{
    init_color_indexes();
    wxString colorname = guiname;
    if(colorname == wxT("<Default>") || guiname.IsEmpty()) {
        return wxT("");

    } else if(colorname.StartsWith(wxT("("))) {
        // Custom color
        wxString code;
        colorname.Prepend("rgb");
        code << wxT("wxColour(") << WXT(colorname) << wxT(")");
        return code;

    } else if(colorname.StartsWith(wxT("#"))) {
        // Custom color
        wxString code;
        code << wxT("wxColour(") << WXT(colorname) << wxT(")");
        return code;

    } else {
        int guinameidx = s_colorNames.Index(guiname);
        wxString code;
        if(guinameidx != wxNOT_FOUND) {
            code << wxT("wxSystemSettings::GetColour(") << s_colorSysNames.Item(guinameidx) << wxT(")");
        }
        return code;
    }
    return "";
}

wxString wxCrafter::ESCAPE(const wxString& s)
{
    wxString escaped = s;

    // Remove any already escaped
    escaped.Replace("\\r", "\r");
    escaped.Replace("\\n", "\n");
    escaped.Replace("\\\"", "\"");

    escaped.Replace("\r", "\\r");
    escaped.Replace("\n", "\\n");
    escaped.Replace("\"", "\\\"");
    return escaped;
}

wxString wxCrafter::UNDERSCORE(const wxString& s)
{
    wxString str;
    if(s.empty()) {
        // Don't annoy translators by giving them "" to translate
        return WXT(s);
    } else {
        if(!wxcProjectMetadata::Get().IsUseUnderscoreMacro()) {
            // Use the wxT() macro
            str << "wxT(\"" << ESCAPE(s) << "\")";
        } else {
            str << "_(\"" << ESCAPE(s) << "\")";
        }
    }
    return str;
}

wxString wxCrafter::WXT(const wxString& s)
{
    wxString str;
    str << wxT("wxT(\"") << ESCAPE(s) << wxT("\")");
    return str;
}

wxString wxCrafter::FontToString(const wxFont& font)
{
    wxString str;
    if(!font.IsOk()) {
        return wxEmptyString;
    }

    // size, style, weight, family, underlined, face

    // Size
    str << font.GetPointSize() << wxT(",");

    // style
    if(font.GetStyle() == wxFONTSTYLE_ITALIC) {
        str << wxT("italic, ");

    } else if(font.GetStyle() == wxFONTSTYLE_SLANT) {
        str << wxT("slant, ");

    } else {
        str << wxT("normal, ");
    }

    // weight
    if(font.GetWeight() == wxFONTWEIGHT_LIGHT) {
        str << wxT("light, ");

    } else if(font.GetWeight() == wxFONTWEIGHT_BOLD) {
        str << wxT("bold, ");

    } else {
        str << wxT("normal, ");
    }

    // family
    if(font.GetFamily() == wxFONTFAMILY_DECORATIVE) {
        str << wxT("decorative, ");

    } else if(font.GetFamily() == wxFONTFAMILY_ROMAN) {
        str << wxT("roman, ");

    } else if(font.GetFamily() == wxFONTFAMILY_SCRIPT) {
        str << wxT("script, ");

    } else if(font.GetFamily() == wxFONTFAMILY_SWISS) {
        str << wxT("swiss, ");

    } else if(font.GetFamily() == wxFONTFAMILY_MODERN) {
        str << wxT("modern, ");

    } else if(font.GetFamily() == wxFONTFAMILY_TELETYPE) {
        str << wxT("teletype, ");

    } else {
        str << wxT("default, ");
    }

    str << font.GetUnderlined() << wxT(", ");

    // face
    str << font.GetFaceName();
    return str;
}

wxFont wxCrafter::StringToFont(const wxString& font)
{
    // is this a system font?
    wxFont f = GetSystemFont(font);
    if(f.IsOk() == true) {
        return f;
    }

    // Some imported fonts may be missing e.g. a 'face', so use wxTOKEN_RET_EMPTY_ALL
    wxArrayString parts = Split(font, wxT(","), wxTOKEN_RET_EMPTY_ALL);
    if(parts.GetCount() != 6) {
        return wxNullFont;
    }

    // size, style, weight, family, underlined, face
    int size = ToNumber(parts.Item(0), -1);
    wxFontStyle style = StringToFontStyle(parts.Item(1));
    wxFontWeight weight = StringToFontWeight(parts.Item(2));
    wxFontFamily family = StringToFontFamily(parts.Item(3));
    bool underlined = false;

    if(parts.Item(4) == wxT("1"))
        underlined = true;

    wxString face = parts.Item(5);

    return wxFont(size, family, style, weight, underlined, face);
}

bool wxCrafter::IsSystemFont(const wxString& font) { return GetSystemFont(font).IsOk(); }

wxString wxCrafter::FontToXRC(const wxString& font)
{
    wxString xrc;
    if(IsSystemFont(font)) {
        wxFont f = GetSystemFont(font);
        wxArrayString parts = Split(font, wxT(","));
        xrc << wxT("<sysfont>") << parts.Item(0) << wxT("</sysfont>");

        if(parts.GetCount() > 1)
            xrc << wxT("<style>") << parts.Item(1) << wxT("</style>");

        if(parts.GetCount() > 2)
            xrc << wxT("<weight>") << parts.Item(2) << wxT("</weight>");

        xrc << wxT("<underlined>") << (f.GetUnderlined() ? wxT("1") : wxT("0")) << wxT("</underlined>");

    } else {

        wxArrayString parts = Split(font, wxT(","));
        if(parts.GetCount() == 6) {
            xrc << wxT("<size>") << parts.Item(0) << wxT("</size>");
            xrc << wxT("<style>") << parts.Item(1) << wxT("</style>");
            xrc << wxT("<weight>") << parts.Item(2) << wxT("</weight>");
            xrc << wxT("<family>") << parts.Item(3) << wxT("</family>");
            xrc << wxT("<underlined>") << parts.Item(4) << wxT("</underlined>");
            xrc << wxT("<face>") << parts.Item(5) << wxT("</face>");
        }
    }
    return xrc;
}

wxString wxCrafter::XRCToFontstring(const wxXmlNode* node)
{
    wxFont font;
    // We might be offered a sysfont node +/- other info,
    // or else anything between 1 and 6 of: size,style,weight,family,underlined,face
    // So start with creating a basic font, either standard or otherwise
    wxXmlNode* sysfontnode = XmlUtils::FindFirstByTagName(node, wxT("sysfont"));
    if(sysfontnode) {
        // There's a system font requested
        font = StringToFont(sysfontnode->GetNodeContent());
    }

    if(!font.IsOk()) {
        // No preferred font, so use the standard one
        font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
        if(!font.IsOk()) {
            return "";
        }
    }

    // Now override with any other info
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("size"));
    if(propertynode) {
        wxString content = propertynode->GetNodeContent();
        // The XRC docs say this should be a wxSize for SetPixelSize()
        // but wxFB's XRC output contains a point setting
        if(content.Contains(",")) {
            font.SetPixelSize(DecodeSize(content));
        } else {
            int pt = ToNumber(content, -1);
            if(pt >= 0) {
                font.SetPointSize(pt);
            }
        }
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("style"));
    if(propertynode) {
        font.SetStyle(StringToFontStyle(propertynode->GetNodeContent()));
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("weight"));
    if(propertynode) {
        font.SetWeight(StringToFontWeight(propertynode->GetNodeContent()));
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("family"));
    if(propertynode) {
        font.SetFamily(StringToFontFamily(propertynode->GetNodeContent()));
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("underlined"));
    if(propertynode) {
        font.SetUnderlined(propertynode->GetNodeContent() == "1");
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("face"));
    if(propertynode) {
        font.SetFaceName(propertynode->GetNodeContent());
    }

    return FontToString(font);
}

wxString wxCrafter::FBToFontstring(const wxString& FBstr)
{
    wxString str;
    if(FBstr.empty()) {
        return str;
    }

    // Though FBstr is already a comma-delimited string, it's in the wrong order for wxC
    // wxFB supplies:  face,      style, weight, size,  family, underlined
    // We want:             size, style, weight,        family, underlined, face
    // The wxFB default font is also faceless, so we must use wxTOKEN_RET_EMPTY_ALL here
    wxArrayString arr = Split(FBstr, ",", wxTOKEN_RET_EMPTY_ALL);
    if(arr.GetCount() < 6) {
        return str;
    }
    wxString face = arr.Item(0), sz = arr.Item(3);
    // wxFB uses a value of -1 as (presumably) a default size if the user didn't specify.
    // This is the same as (the valid) wxFONTSIZE_SMALL but, at least on my system, results in a size of 12 pts which is
    // too large so use the system font size instead
    static int defaultfontsize = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT).GetPointSize();
    if(sz == "-1") {
        sz = wxString::Format("%d", defaultfontsize);
    }

    arr.RemoveAt(3);
    arr.Item(0) = sz;
    arr.Add(face);
    wxFont font = wxCrafter::StringToFont(Join(arr, ","));
    if(font.IsOk()) {
        str = wxCrafter::FontToString(font);
    }

    return str;
}

wxString wxCrafter::FontToCpp(const wxString& font, const wxString& fontMemberName)
{
    if(font.IsEmpty())
        return wxT("wxNullFont");

    wxString code;
    if(IsSystemFont(font)) {

        wxFont f = GetSystemFont(font);
        wxArrayString parts = Split(font, wxT(","));

        if(parts.Item(0) == "wxSYS_ANSI_FIXED_FONT") {

            code << "#ifdef __WXMSW__\n";
            code << "// To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family "
                    "set to wxFONTFAMILY_TELETYPE\n";
            code << "wxFont " << fontMemberName << " = "
                 << "wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);\n";
            code << fontMemberName << ".SetFamily(wxFONTFAMILY_TELETYPE);\n";
            code << "#else\n";
            code << "wxFont " << fontMemberName << " = "
                 << "wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);\n";
            code << fontMemberName << ".SetFamily(wxFONTFAMILY_TELETYPE);\n";
            code << "#endif\n";

        } else {
            code << wxT("wxFont ") << fontMemberName << wxT(" = ") << wxT("wxSystemSettings::GetFont(") << parts.Item(0)
                 << wxT(");\n");
        }

        if(f.GetStyle() == wxFONTSTYLE_ITALIC)
            code << fontMemberName << wxT(".SetStyle(wxFONTSTYLE_ITALIC);\n");

        if(f.GetUnderlined())
            code << fontMemberName << wxT(".SetUnderlined(true);\n");

        if(f.GetWeight() == wxFONTWEIGHT_BOLD)
            code << fontMemberName << wxT(".SetWeight(wxFONTWEIGHT_BOLD);\n");
        return code;

    } else {
        // Custom font
        wxArrayString parts = Split(font, wxT(","));
        if(parts.GetCount() != 6) {
            return wxT("wxNullFont");
        }

        // size, style, weight, family, underlined
        int size = ToNumber(parts.Item(0), -1);
        wxString style = wxT("wxFONTSTYLE_NORMAL");
        wxString weight = wxT("wxFONTWEIGHT_NORMAL");
        wxString family = wxT("wxFONTFAMILY_DEFAULT");
        wxString underlined = wxT("false");
        wxString face = parts.Item(5);

        if(parts.Item(1) == wxT("italic")) {
            style = wxT("wxFONTSTYLE_ITALIC");

        } else if(parts.Item(1) == wxT("slant")) {
            style = wxT("wxFONTSTYLE_SLANT");
        }

        if(parts.Item(2) == wxT("bold")) {
            weight = wxT("wxFONTWEIGHT_BOLD");

        } else if(parts.Item(2) == wxT("light")) {
            weight = wxT("wxFONTWEIGHT_LIGHT");
        }

        // family
        if(parts.Item(3) == wxT("decorative"))
            family = wxT("wxFONTFAMILY_DECORATIVE");

        if(parts.Item(3) == wxT("roman"))
            family = wxT("wxFONTFAMILY_ROMAN");

        if(parts.Item(3) == wxT("script"))
            family = wxT("wxFONTFAMILY_SCRIPT");

        if(parts.Item(3) == wxT("swiss"))
            family = wxT("wxFONTFAMILY_SWISS");

        if(parts.Item(3) == wxT("modern"))
            family = wxT("wxFONTFAMILY_MODERN");

        if(parts.Item(3) == wxT("teletype"))
            family = wxT("wxFONTFAMILY_TELETYPE");

        if(parts.Item(4) == wxT("1"))
            underlined = wxT("true");
        else
            underlined = wxT("false");

        code << wxT("wxFont ") << fontMemberName << wxT("(") << size << wxT(", ") << family << wxT(", ") << style
             << wxT(", ") << weight << wxT(", ") << underlined << wxT(", ") << WXT(face) << wxT(");\n");
        return code;
    }
}

wxString wxCrafter::XMLEncode(const wxString& text, bool decode /*=false*/)
{
    wxString str = text;
    static std::map<wxString, wxString> s_xmlEntities;
    if(s_xmlEntities.empty()) {
        s_xmlEntities[wxT("\"")] = wxT("&quot;");
        s_xmlEntities[wxT("'")] = wxT("&apos;");
        s_xmlEntities[wxT("<")] = wxT("&lt;");
        s_xmlEntities[wxT(">")] = wxT("&gt;");
        s_xmlEntities[wxT("&")] = wxT("&amp;");
        s_xmlEntities[wxT("\\n")] = wxT("&#x0A;");
    }

    std::map<wxString, wxString>::const_iterator iter = s_xmlEntities.begin();
    for(; iter != s_xmlEntities.end(); iter++) {
        if(!decode) {
            str.Replace(iter->first, iter->second);
        } else {
            str.Replace(iter->second, iter->first);
        }
    }
    return str;
}

wxString wxCrafter::ToBool(const wxString& text)
{
    if(text == wxT("1"))
        return wxT("true");
    else
        return wxT("false");
}

wxString wxCrafter::CDATA(const wxString& text)
{
    wxString s;

    s << wxT("<![CDATA[") << text << wxT("]]>");
    return s;
}

wxArrayString wxCrafter::MakeUnique(const wxArrayString& arr)
{
    wxArrayString res;
    for(size_t i = 0; i < arr.GetCount(); i++) {
        if(res.Index(arr.Item(i)) == wxNOT_FOUND) {
            res.Add(arr.Item(i));
        }
    }
    return res;
}

void wxCrafter::WriteFile(const wxFileName& filename, const wxString& content, bool overwriteContent)
{
    if(!overwriteContent && filename.FileExists()) {
        return;
    }

    wxFFile fp(filename.GetFullPath(), wxT("w+b"));
    if(fp.IsOpened()) {
        fp.Write(content, wxConvUTF8);
        fp.Close();
    }
}

bool wxCrafter::IsTheSame(const wxFileName& f1, const wxFileName& f2)
{
    if(f1.FileExists() == false || f2.FileExists() == false)
        return false;

    wxString fileOneContent, fileTwoContent;

    wxFFile fileOne(f1.GetFullPath(), wxT("r+b"));
    wxFFile fileTwo(f2.GetFullPath(), wxT("r+b"));

    if(fileOne.IsOpened() && fileTwo.IsOpened()) {
        // Compare the content
        if(fileOne.ReadAll(&fileOneContent, wxConvUTF8) && fileTwo.ReadAll(&fileTwoContent, wxConvUTF8) &&
           fileOneContent == fileTwoContent) {
            return true;
        }
    }
    return false;
}

bool wxCrafter::IsTheSame(const wxString& fileContent, const wxFileName& f)
{
    if(!f.FileExists())
        return false;

    wxString fileOneContent;

    wxFFile fileOne(f.GetFullPath(), wxT("r+b"));

    if(fileOne.IsOpened()) {
        // Compare the content
        if(fileOne.ReadAll(&fileOneContent, wxConvUTF8) && fileOneContent == fileContent) {
            return true;
        }
    }
    return false;
}

float wxCrafter::ToFloat(const wxString& str, float defaultValue)
{
    double v;

    /// Use ToCDouble to avoid cases (e.g. German locale)
    /// where 0.5 is actually 0,5 - which obvisouly breaks compilation
    if(str.ToCDouble(&v)) {
        return v;
    } else {
        return defaultValue;
    }
}

std::set<wxString> wxCrafter::VectorToSet(const std::vector<wxFileName>& v)
{
    std::set<wxString> s;
    for(size_t i = 0; i < v.size(); i++) {
        wxString fullpath = v.at(i).GetFullPath();
        s.insert(fullpath);
    }
    return s;
}

wxArrayString wxCrafter::GetToolTypes(bool forToolbar)
{
    wxArrayString kinds;
    kinds.Add(ITEM_NORMAL);
    kinds.Add(ITEM_CHECK);
    kinds.Add(ITEM_RADIO);
    kinds.Add(ITEM_SEPARATOR);
    if(forToolbar) {
        kinds.Add(ITEM_DROPDOWN);
    }
    return kinds;
}

wxCrafter::TOOL_TYPE wxCrafter::GetToolType(const wxString& name)
{
    if(name == ITEM_NORMAL) {
        return TOOL_TYPE_NORMAL;

    } else if(name == ITEM_CHECK) {
        return TOOL_TYPE_CHECK;

    } else if(name == ITEM_RADIO) {
        return TOOL_TYPE_RADIO;

    } else if(name == ITEM_SEPARATOR) {
        return TOOL_TYPE_SEPARATOR;

    } else if(name == ITEM_SPACE) {
        return TOOL_TYPE_SPACE;

    } else if(name == ITEM_DROPDOWN) {
        return TOOL_TYPE_DROPDOWN;

        /*    } else if(name == ITEM_AUILABEL) { This, and those of the other aui oddities, aren't needed atm
                return TOOL_TYPE_AUILABEL;*/

    } else {
        return TOOL_TYPE_NORMAL;
    }
}

wxString wxCrafter::ToolTypeToWX(TOOL_TYPE type)
{
    if(type == TOOL_TYPE_NORMAL) {
        return wxT("wxITEM_NORMAL");

    } else if(type == TOOL_TYPE_CHECK) {
        return wxT("wxITEM_CHECK");

    } else if(type == TOOL_TYPE_RADIO) {
        return wxT("wxITEM_RADIO");

    } else if(type == TOOL_TYPE_SEPARATOR) {
        return wxT("wxITEM_SEPARATOR");

    } else if(type == TOOL_TYPE_DROPDOWN) {
        return wxT("wxITEM_DROPDOWN");

    } else {
        return wxT("wxITEM_NORMAL");
    }
}

wxString wxCrafter::GetUserDataDir()
{
    wxFileName dir(wxStandardPaths::Get().GetUserDataDir(), wxEmptyString);
#ifndef NDEBUG
    dir.SetPath(dir.GetPath() + "-dbg");
#endif
    dir.AppendDir(wxT("wxcrafter"));
    static bool once = true;
    if(once) {
        dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        once = false;
    }
    return dir.GetPath();
}

void wxCrafter::SetStatusMessage(const wxString& msg)
{
    if(TopFrame()) {
        MainFrame* frame = dynamic_cast<MainFrame*>(TopFrame());
        if(frame) {
            frame->SetStatusMessage(msg);
        }
    }
}

wxString wxCrafter::GetConfigFile()
{
    wxFileName dir(GetUserDataDir(), wxT("wxcrafter.conf"));
    return dir.GetFullPath();
}

wxArrayString wxCrafter::Split(const wxString& str, const wxString& delim, wxStringTokenizerMode mode)
{
    wxArrayString arr = wxStringTokenize(str, delim, mode);
    for(size_t i = 0; i < arr.GetCount(); i++) {
        arr.Item(i).Trim().Trim(false);
    }
    return arr;
}

int wxCrafter::GetColourSysIndex(const wxString& name)
{
    init_color_indexes();
    int where = s_colorNames.Index(name);
    if(where == wxNOT_FOUND) {
        // try the system colours
        where = s_colorSysNames.Index(name);
        if(where != wxNOT_FOUND) {
            return s_colorIdx.Item(where);
        }
    } else {
        return s_colorIdx.Item(where);
    }
    return wxNOT_FOUND;
}

wxString wxCrafter::GetColourForXRC(const wxString& name)
{
    init_color_indexes();
    if(name == "<Default>" || name.IsEmpty())
        return wxEmptyString;

    if(s_colorSysNames.Index(name) != wxNOT_FOUND)
        return name;

    int colorIdx = s_colorNames.Index(name);
    if(colorIdx == wxNOT_FOUND) {
        wxString colname = name;
        colname.Trim().Trim(false);
        if(colname.StartsWith("(")) {
            colname.Prepend("rgb");
        }

        wxColour col(colname);
        return col.GetAsString(wxC2S_HTML_SYNTAX);

    } else {
        return s_colorSysNames.Item(colorIdx);
    }
}

wxColour wxCrafter::NameToColour(const wxString& name)
{
    int sysidx = GetColourSysIndex(name);
    if(sysidx != wxNOT_FOUND)
        return wxSystemSettings::GetColour((wxSystemColour)sysidx);

    // not a system colour name nor PG name, a custom color then
    wxString colname = name;
    colname.Trim().Trim(false);

    if(colname.StartsWith("#")) {
        // HTML format
        wxColour col(colname);
        return col;

    } else if(colname.StartsWith("rgb")) {
        return wxColour(colname);

    } else if(colname.StartsWith("(")) {
        return wxColour(colname.Prepend("rgb"));
    }
    return *wxWHITE;
}

wxString wxCrafter::ValueToColourString(const wxString& value)
{
    wxString str(value);
    // Value might be any of the ways of specifying colour,
    // but it may also be imported from wxFB as rgb() but without the syntax
    if((value.Left(1) != "(") && (value.BeforeLast(',') != "")) {
        str = '(' + value + ')';
    }

    wxColour col = NameToColour(str);
    return col.GetAsString();
}

wxString wxCrafter::WXVER_CHECK_BLOCK_END()
{
    wxString code;
    code << "#endif\n";
    return code;
}

wxString wxCrafter::WX294_BLOCK_START()
{
    wxString code;
    code << "#if wxVERSION_NUMBER >= 2904\n";
    return code;
}

wxString wxCrafter::WX30_BLOCK_START()
{
    wxString code;
    code << "#if wxVERSION_NUMBER >= 3000\n";
    return code;
}

wxString wxCrafter::WX31_BLOCK_START()
{
    wxString code;
    code << "#if wxVERSION_NUMBER >= 3100\n";
    return code;
}

wxString wxCrafter::WX33_BLOCK_START()
{
    wxString code;
    code << "#if wxVERSION_NUMBER >= 3300\n";
    return code;
}

wxString wxCrafter::WX29_BLOCK_START()
{
    wxString code;
    code << "#if wxVERSION_NUMBER >= 2900\n";
    return code;
}

wxString wxCrafter::AddQuotes(const wxString& str)
{
    wxString s = str;
    s.Trim().Trim(false);
    if(!s.StartsWith('"'))
        s.Prepend('"');

    if(!s.EndsWith('"'))
        s.Append('"');

    return s;
}

void wxCrafter::MakeAbsToProject(wxFileName& fn)
{
    if(fn.IsRelative()) {
        fn.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath());
    }
}

int wxCrafter::ToAligment(const wxString& aligment)
{
    static std::map<wxString, int> s_stringToAlignment;
    if(s_stringToAlignment.empty()) {
        s_stringToAlignment.insert(std::make_pair("wxALIGN_NOT", wxALIGN_NOT));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_CENTER_HORIZONTAL", wxALIGN_CENTER_HORIZONTAL));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_CENTRE_HORIZONTAL", wxALIGN_CENTRE_HORIZONTAL));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_LEFT", wxALIGN_LEFT));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_TOP", wxALIGN_TOP));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_RIGHT", wxALIGN_RIGHT));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_BOTTOM", wxALIGN_BOTTOM));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_CENTER_VERTICAL", wxALIGN_CENTER_VERTICAL));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_CENTRE_VERTICAL", wxALIGN_CENTRE_VERTICAL));
        s_stringToAlignment.insert(
            std::make_pair("wxALIGN_CENTER", wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_CENTRE", wxALIGN_CENTRE));
        s_stringToAlignment.insert(std::make_pair("wxALIGN_INVALID", wxALIGN_INVALID));
    }

    if(s_stringToAlignment.count(aligment) == 0)
        return wxALIGN_NOT;
    return s_stringToAlignment.find(aligment)->second;
}

wxString wxCrafter::FloatToCString(float f) { return wxString::FromCDouble(f); }

int wxCrafter::ColumnFlagsFromString(const wxString& col_flags)
{
    int flags = 0;
    static std::map<wxString, int> s_stringToFlag;
    if(s_stringToFlag.empty()) {
        s_stringToFlag.insert(std::make_pair("wxCOL_RESIZABLE", (int)wxCOL_RESIZABLE));
        s_stringToFlag.insert(std::make_pair("wxCOL_SORTABLE", (int)wxCOL_SORTABLE));
        s_stringToFlag.insert(std::make_pair("wxCOL_REORDERABLE", (int)wxCOL_REORDERABLE));
        s_stringToFlag.insert(std::make_pair("wxCOL_HIDDEN", (int)wxCOL_HIDDEN));
        s_stringToFlag.insert(std::make_pair("wxCOL_DEFAULT_FLAGS", (int)wxCOL_DEFAULT_FLAGS));
    }

    wxArrayString flagsStringArr = ::wxStringTokenize(col_flags, "|", wxTOKEN_STRTOK);
    for(size_t i = 0; i < flagsStringArr.GetCount(); ++i) {
        wxString flagString = flagsStringArr.Item(i).Trim();
        ;
        if(s_stringToFlag.count(flagString)) {
            flags |= s_stringToFlag.find(flagString)->second;
        }
    }
    return flags;
}

bool wxCrafter::IsArtProviderBitmap(const wxString& bmpString, wxString& artId, wxString& clientID, wxString& sizeHint)
{
    static wxArrayString arr;
    if(arr.IsEmpty()) {
        arr.Add(wxT("wxART_ERROR"));
        arr.Add(wxT("wxART_QUESTION"));
        arr.Add(wxT("wxART_WARNING"));
        arr.Add(wxT("wxART_INFORMATION"));
        arr.Add(wxT("wxART_ADD_BOOKMARK"));
        arr.Add(wxT("wxART_DEL_BOOKMARK"));
        arr.Add(wxT("wxART_HELP_SIDE_PANEL"));
        arr.Add(wxT("wxART_HELP_SETTINGS"));
        arr.Add(wxT("wxART_HELP_BOOK"));
        arr.Add(wxT("wxART_HELP_FOLDER"));
        arr.Add(wxT("wxART_HELP_PAGE"));
        arr.Add(wxT("wxART_GO_BACK"));
        arr.Add(wxT("wxART_GO_FORWARD"));
        arr.Add(wxT("wxART_GO_UP"));
        arr.Add(wxT("wxART_GO_DOWN"));
        arr.Add(wxT("wxART_GO_TO_PARENT"));
        arr.Add(wxT("wxART_GO_HOME"));
        arr.Add(wxT("wxART_GOTO_FIRST"));
        arr.Add(wxT("wxART_GOTO_LAST"));
        arr.Add(wxT("wxART_PRINT"));
        arr.Add(wxT("wxART_HELP"));
        arr.Add(wxT("wxART_TIP"));
        arr.Add(wxT("wxART_REPORT_VIEW"));
        arr.Add(wxT("wxART_LIST_VIEW"));
        arr.Add(wxT("wxART_NEW_DIR"));
        arr.Add(wxT("wxART_FOLDER"));
        arr.Add(wxT("wxART_FOLDER_OPEN"));
        arr.Add(wxT("wxART_GO_DIR_UP"));
        arr.Add(wxT("wxART_EXECUTABLE_FILE"));
        arr.Add(wxT("wxART_NORMAL_FILE"));
        arr.Add(wxT("wxART_TICK_MARK"));
        arr.Add(wxT("wxART_CROSS_MARK"));
        arr.Add(wxT("wxART_MISSING_IMAGE"));
        arr.Add(wxT("wxART_NEW"));
        arr.Add(wxT("wxART_FILE_OPEN"));
        arr.Add(wxT("wxART_FILE_SAVE"));
        arr.Add(wxT("wxART_FILE_SAVE_AS"));
        arr.Add(wxT("wxART_DELETE"));
        arr.Add(wxT("wxART_COPY"));
        arr.Add(wxT("wxART_CUT"));
        arr.Add(wxT("wxART_PASTE"));
        arr.Add(wxT("wxART_UNDO"));
        arr.Add(wxT("wxART_REDO"));
        arr.Add(wxT("wxART_PLUS"));
        arr.Add(wxT("wxART_MINUS"));
        arr.Add(wxT("wxART_CLOSE"));
        arr.Add(wxT("wxART_QUIT"));
        arr.Add(wxT("wxART_FIND"));
        arr.Add(wxT("wxART_FIND_AND_REPLACE"));
        arr.Add(wxT("wxART_HARDDISK"));
        arr.Add(wxT("wxART_FLOPPY"));
        arr.Add(wxT("wxART_CDROM"));
        arr.Add(wxT("wxART_REMOVABLE"));
        arr.Sort();
    }

    // Art ID
    wxArrayString parts = ::wxStringTokenize(bmpString, ",", wxTOKEN_STRTOK);
    sizeHint = "wxDefaultSize";
    if(parts.GetCount() > 1) {
        artId = parts.Item(0);
        clientID = parts.Item(1);
    }
    if(parts.GetCount() > 2) {
        sizeHint = parts.Item(2);
    }

    if(arr.Index(artId) != wxNOT_FOUND) {
        return true;
    }
    return false;
}

wxString wxCrafter::MakeWxSizeStr(const wxString& size)
{
    wxString strSize;
    if(size.StartsWith("wxSize")) {
        return size;
    }

    if(size.Contains(",")) {
        strSize << "wxSize(" << size << ")";

    } else if(size == "wxDefaultSize") {
        strSize = "wxDefaultSize";

    } else {
        strSize << "wxSize(" << size << ", " << size << ")";
    }
    return strSize;
}

wxShowEffect wxCrafter::ShowEffectFromString(const wxString& effect)
{
    std::map<wxString, wxShowEffect> m;
    m.insert(std::make_pair("wxSHOW_EFFECT_ROLL_TO_LEFT", wxSHOW_EFFECT_ROLL_TO_LEFT));
    m.insert(std::make_pair("wxSHOW_EFFECT_ROLL_TO_RIGHT", wxSHOW_EFFECT_ROLL_TO_RIGHT));
    m.insert(std::make_pair("wxSHOW_EFFECT_ROLL_TO_TOP", wxSHOW_EFFECT_ROLL_TO_TOP));
    m.insert(std::make_pair("wxSHOW_EFFECT_ROLL_TO_BOTTOM", wxSHOW_EFFECT_ROLL_TO_BOTTOM));
    m.insert(std::make_pair("wxSHOW_EFFECT_SLIDE_TO_LEFT", wxSHOW_EFFECT_SLIDE_TO_LEFT));
    m.insert(std::make_pair("wxSHOW_EFFECT_SLIDE_TO_RIGHT", wxSHOW_EFFECT_SLIDE_TO_RIGHT));
    m.insert(std::make_pair("wxSHOW_EFFECT_SLIDE_TO_TOP", wxSHOW_EFFECT_SLIDE_TO_TOP));
    m.insert(std::make_pair("wxSHOW_EFFECT_SLIDE_TO_BOTTOM", wxSHOW_EFFECT_SLIDE_TO_BOTTOM));
    m.insert(std::make_pair("wxSHOW_EFFECT_BLEND", wxSHOW_EFFECT_BLEND));
    m.insert(std::make_pair("wxSHOW_EFFECT_EXPAND", wxSHOW_EFFECT_EXPAND));

    if(m.count(effect)) {
        return m.find(effect)->second;
    }

    return wxSHOW_EFFECT_NONE;
}

void wxCrafter::NotifyFileSaved(const wxFileName& fn)
{
#if !STANDALONE_BUILD
    EventNotifier::Get()->PostFileSavedEvent(fn.GetFullPath());
#else
    wxUnusedVar(fn);
#endif
}

void wxCrafter::WrapInIfBlock(const wxString& condname, wxString& codeblock)
{
    if(condname.IsEmpty())
        return;

    wxString topBlock, bottomBlock;
    if(codeblock.StartsWith("\n")) {
        topBlock << "\n";
    }

    if(!codeblock.EndsWith("\n")) {
        bottomBlock << "\n";
    }

    topBlock << "#if " << condname << "\n";
    bottomBlock << "#endif // " << condname << "\n";

    codeblock.Prepend(topBlock).Append(bottomBlock);
}

void wxCrafter::GetWorkspaceFiles(wxStringSet_t& files)
{
    wxArrayString arrFiles;
    clCxxWorkspaceST::Get()->GetWorkspaceFiles(arrFiles);
    for(size_t i = 0; i < arrFiles.GetCount(); ++i) {
        files.insert(arrFiles.Item(i));
    }
}

void wxCrafter::GetProjectFiles(const wxString& projectName, wxStringSet_t& files)
{
    ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(projectName);
    if(!p) {
        return;
    }
    const Project::FilesMap_t& filesMap = p->GetFiles();
    files.reserve(filesMap.size());
    std::for_each(filesMap.begin(), filesMap.end(),
                  [&](const Project::FilesMap_t::value_type& vt) { files.insert(vt.first); });
}

void wxCrafter::FormatString(wxString& content, const wxFileName& filename)
{
#if !STANDALONE_BUILD
    clSourceFormatEvent event{ wxEVT_FORMAT_STRING };
    event.SetFileName(filename.GetFullPath());
    event.SetInputString(content);
    EventNotifier::Get()->ProcessEvent(event);
    if(!event.GetFormattedString().IsEmpty()) {
        content = event.GetFormattedString();
    }
#endif
}

void wxCrafter::FormatFile(const wxFileName& filename)
{
#if !STANDALONE_BUILD
    clSourceFormatEvent event(wxEVT_FORMAT_FILE);
    event.SetFileName(filename.GetFullPath());
    EventNotifier::Get()->ProcessEvent(event);
#endif
}

static wxWindow* sTopFrame = NULL;
wxWindow* wxCrafter::TopFrame()
{
    if(!sTopFrame) {
        return wxTheApp->GetTopWindow();
    }
    return sTopFrame;
}

void wxCrafter::SetTopFrame(wxWindow* frame) { sTopFrame = frame; }
