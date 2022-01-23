#include "ColoursAndFontsManager.h"

#include "JSON.h"
#include "ThemeImporterManager.hpp"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "macros.h"
#include "wxStringHash.h"
#include "xmlutils.h"

#include <algorithm>
#include <codelite_events.h>
#include <imanager.h>
#include <wx/busyinfo.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

// Upgrade macros
#define LEXERS_VERSION_STRING "LexersVersion"
#define LEXERS_VERSION 6

wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_START, clCommandEvent);
wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_END, clCommandEvent);
wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_PROGRESS, clCommandEvent);

namespace
{
const wxString LexerTextDefaultXML =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<Lexer Name=\"text\" Theme=\"Default\" IsActive=\"No\" UseCustomTextSelFgColour=\"Yes\" "
    "StylingWithinPreProcessor=\"yes\" Id=\"1\">"
    "  <KeyWords0/>"
    "  <KeyWords1/>"
    "  <KeyWords2/>"
    "  <KeyWords3/>"
    "  <KeyWords4/>"
    "  <Extensions/>"
    "  <Properties>"
    "    <Property Id=\"0\" Name=\"Default\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" Italic=\"no\" "
    "Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"34\" Name=\"Brace match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"cyan\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"35\" Name=\"Brace bad match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"red\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"37\" Name=\"Indent Guide\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-1\" Name=\"Fold Margin\" Bold=\"no\" Face=\"\" Colour=\"white\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-2\" Name=\"Text Selection\" Bold=\"no\" Face=\"\" Colour=\"#4E687D\" BgColour=\"#D6D2D0\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-3\" Name=\"Caret Colour\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-4\" Name=\"Whitespace\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"38\" Name=\"Calltip\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" Italic=\"no\" "
    "Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"33\" Name=\"Line Numbers\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"34\" Name=\"Brace match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"cyan\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"35\" Name=\"Brace bad match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"red\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"37\" Name=\"Indent Guide\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-1\" Name=\"Fold Margin\" Bold=\"no\" Face=\"\" Colour=\"white\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-2\" Name=\"Text Selection\" Bold=\"no\" Face=\"\" Colour=\"#4E687D\" BgColour=\"#D6D2D0\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-3\" Name=\"Caret Colour\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-4\" Name=\"Whitespace\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"38\" Name=\"Calltip\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" Italic=\"no\" "
    "Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"33\" Name=\"Line Numbers\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "  </Properties>"
    "</Lexer>";

void AddLexerKeywords(LexerConf::Ptr_t lexer, int setIndex, const std::vector<wxString>& words)
{
    wxString curwords = lexer->GetKeyWords(setIndex);
    wxArrayString arrWords = ::wxStringTokenize(curwords, " ", wxTOKEN_STRTOK);
    // use std::set to make sure that the elements are sorted
    std::set<wxString> uniqueSet;
    for(size_t i = 0; i < arrWords.size(); ++i) {
        uniqueSet.insert(arrWords.Item(i));
    }

    // add the new words
    for(const wxString& new_word : words) {
        uniqueSet.insert(new_word);
    }

    // serialise them back to space delimited string
    curwords.clear();
    for(const auto& word : uniqueSet) {
        curwords << word << " ";
    }
    lexer->SetKeyWords(curwords, setIndex);
}

wxString DEFAULT_THEME = "Atom One-Dark";

void AddFileExtension(LexerConf::Ptr_t lexer, const wxString& extension)
{
    wxString spec = lexer->GetFileSpec();
    wxString ext = extension;
    ext.Replace(".", wxEmptyString);
    ext.Replace("*", wxEmptyString);
    ext.Prepend(".").Prepend("*");

    wxString as_str;
    auto extensions = ::wxStringTokenize(spec, ";,", wxTOKEN_STRTOK);
    if(extensions.Index(ext) == wxNOT_FOUND) {
        extensions.Add(ext);
        as_str = ::wxJoin(extensions, ';');
        lexer->SetFileSpec(as_str);
    } else {
        // already exists
    }
}

} // namespace

class clCommandEvent;
ColoursAndFontsManager::ColoursAndFontsManager()
    : m_initialized(false)
{
    // Create a "go to lexer" when all is broken
    wxStringInputStream sis(LexerTextDefaultXML);
    wxXmlDocument doc;
    if(doc.Load(sis)) {
        m_defaultLexer.Reset(new LexerConf());
        m_defaultLexer->FromXml(doc.GetRoot());
    }
    m_lexersVersion = clConfig::Get().Read(LEXERS_VERSION_STRING, LEXERS_UPGRADE_LINENUM_DEFAULT_COLOURS);
    EventNotifier::Get()->Bind(wxEVT_INFO_BAR_BUTTON, &ColoursAndFontsManager::OnAdjustTheme, this);
}

ColoursAndFontsManager::~ColoursAndFontsManager()
{
    clConfig::Get().Write(LEXERS_VERSION_STRING, LEXERS_VERSION);
    EventNotifier::Get()->Unbind(wxEVT_INFO_BAR_BUTTON, &ColoursAndFontsManager::OnAdjustTheme, this);
}

ColoursAndFontsManager& ColoursAndFontsManager::Get()
{
    static ColoursAndFontsManager s_theManager;
    return s_theManager;
}

/**
 * @class ColoursAndFontsManagerLoaderHelper
 * @brief
 */
struct ColoursAndFontsManagerLoaderHelper {
    ColoursAndFontsManager* m_manager;
    ColoursAndFontsManagerLoaderHelper(ColoursAndFontsManager* manager)
        : m_manager(manager)
    {
    }

    void Load()
    {
        std::vector<wxXmlDocument*> userLexers;
        wxArrayString files;

        //---------------------------------------------
        // Load user lexers (new format only)
        //---------------------------------------------
        files.Clear();
        wxFileName userLexersPath(clStandardPaths::Get().GetUserDataDir(), "");
        userLexersPath.AppendDir("lexers");

        CL_DEBUG("Loading users lexers");
        wxDir::GetAllFiles(userLexersPath.GetPath(), &files, "lexer_*.xml");
        // Each XMl represents a single lexer
        for(size_t i = 0; i < files.GetCount(); ++i) {
            wxString content;
            wxFFile xmlFile(files.Item(i), "rb");
            if(!xmlFile.IsOpened())
                continue;
            if(xmlFile.ReadAll(&content, wxConvUTF8)) {
                wxXmlDocument* doc = new wxXmlDocument();
                wxStringInputStream sis(content);
                if(doc->Load(sis)) {
                    userLexers.push_back(doc);
                } else {
                    wxDELETE(doc);
                }
            }
            xmlFile.Close();
            clRemoveFile(files.Item(i));
        }
        CL_DEBUG("Loading users lexers...done");
        m_manager->OnLexerFilesLoaded(userLexers);
    }
};

void ColoursAndFontsManager::Load()
{
    if(m_initialized)
        return;
    m_lexersMap.clear();
    m_initialized = true;
    m_globalTheme = DEFAULT_THEME;

    // Load the global settings
    if(GetConfigFile().FileExists()) {
        JSON root(GetConfigFile());
        if(root.isOk()) {
            m_globalTheme = root.toElement().namedObject("m_globalTheme").toString("Atom One-Dark");
        }
    }

    // Load the lexers
    ColoursAndFontsManagerLoaderHelper loader(this);
    loader.Load();

    // read the global font property
    m_globalFont = clConfig::Get().Read("GlobalThemeFont", DrawingUtils::GetDefaultFixedFont());
}

void ColoursAndFontsManager::LoadOldXmls(const std::vector<wxXmlDocument*>& xmlFiles, bool userLexers)
{
    // Each XMl represents a single lexer (the old format)
    for(size_t i = 0; i < xmlFiles.size(); ++i) {
        wxXmlDocument* doc = xmlFiles.at(i);
        DEPRECATRED_DoAddLexer(doc->GetRoot());
    }
}

LexerConf::Ptr_t ColoursAndFontsManager::DEPRECATRED_DoAddLexer(wxXmlNode* node)
{
    wxString lexerName = XmlUtils::ReadString(node, "Name");
    lexerName.MakeLower();
    if(lexerName.IsEmpty())
        return NULL;

    LexerConf::Ptr_t lexer(new LexerConf);
    lexer->FromXml(node);

    // ensure that the theme name is capitalized - this helps
    // when displaying the content in a wxListBox sorted
    wxString themeName = lexer->GetThemeName();
    themeName = themeName.Mid(0, 1).Capitalize() + themeName.Mid(1);
    lexer->SetThemeName(themeName);

    if(lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("final")) {
        lexer->SetKeyWords(lexer->GetKeyWords(0) + " final", 0);
    }

    // Add C++ keyword "override"
    if(lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("override")) {
        lexer->SetKeyWords(lexer->GetKeyWords(0) + " override", 0);
    }

    // Hack: fix Java lexer which is using the same
    // file extensions as C++...
    if(lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) {
        lexer->SetFileSpec("*.java");
    }

    // Append *.sqlite to the SQL lexer if missing
    if(lexer->GetName() == "sql" && !lexer->GetFileSpec().Contains(".sqlite")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.sqlite");
    }

    // Hack2: since we now provide our own PHP and javaScript lexer, remove the PHP/JS extensions from
    // the HTML lexer
    if(lexer->GetName() == "html" && (lexer->GetFileSpec().Contains(".php") || lexer->GetFileSpec().Contains("*.js"))) {
        lexer->SetFileSpec("*.htm;*.html;*.xhtml");
    }

    // Hack3: all the HTML support to PHP which have much more colour themes
    if(lexer->GetName() == "html" && lexer->GetFileSpec().Contains(".html")) {
        lexer->SetFileSpec("*.vbs;*.vbe;*.wsf;*.wsc;*.asp;*.aspx");
    }

    // Hack4: all the HTML support to PHP which have much more colour themes
    if(lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".qml")) {
        lexer->SetFileSpec("*.js;*.javascript;*.qml;*.json");
    }

    if(lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".html")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.html;*.htm;*.xhtml");
    }

    if(lexer->GetName() == "php" && !lexer->GetKeyWords(4).Contains("<?php")) {
        lexer->SetKeyWords(lexer->GetKeyWords(4) + " <?php <? ", 4);
    }

    // Add wxcp file extension to the JavaScript lexer
    if(lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".wxcp")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.wxcp");
    }

    if(lexer->GetName() == "javascript") {
        AddLexerKeywords(lexer, 0, { "class", "await", "async" });
    }

    // update script lexer with additional keywords
    if(lexer->GetName() == "script") {
        AddLexerKeywords(lexer, 0, { "return", "exit", "local", "function" });
    }

    // Add *.scss file extension to the css lexer
    if(lexer->GetName() == "css" && !lexer->GetFileSpec().Contains(".scss")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.scss");
    }

    // Add *.less file extension to the css lexer
    if(lexer->GetName() == "css" && !lexer->GetFileSpec().Contains(".less")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.less");
    }

    // Set the JavaScript keywords
    if(lexer->GetName() == "php" && !lexer->GetKeyWords(1).Contains("instanceof")) {
        lexer->SetKeyWords(
            "break do instanceof typeof case else new var catch finally return void continue for switch while "
            "debugger function this with default if throw delete in try abstract  export interface  static "
            "boolean  extends "
            "long super"
            "byte final native synchronized char float package throws class goto private transient const "
            "implements  protected "
            " volatile double import  public enum "
            "int short null true false",
            1);
    }

    if(lexer->GetName() == "makefile" && !lexer->GetFileSpec().Contains("*akefile.am")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*akefile.in;*akefile.am");
    }

    // Upgrade the lexer colours
    UpdateLexerColours(lexer, false);

    if(m_lexersMap.count(lexerName) == 0) {
        m_lexersMap.insert(std::make_pair(lexerName, ColoursAndFontsManager::Vec_t()));
    }

    ColoursAndFontsManager::Vec_t& vec = m_lexersMap.find(lexerName)->second;

    // Locate an instance with this name and theme in
    // both the m_alllexers and vector for this lexer
    // name
    ColoursAndFontsManager::Vec_t::iterator iter =
        std::find_if(vec.begin(), vec.end(), LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
    if(iter != vec.end()) {
        vec.erase(iter);
    }
    iter = std::find_if(m_allLexers.begin(), m_allLexers.end(),
                        LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
    if(iter != m_allLexers.end()) {
        m_allLexers.erase(iter);
    }
    vec.push_back(lexer);
    m_allLexers.push_back(lexer);
    return lexer;
}

wxArrayString ColoursAndFontsManager::GetAvailableThemesForLexer(const wxString& lexerName) const
{
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.find(lexerName.Lower());
    if(iter == m_lexersMap.end())
        return wxArrayString();

    wxArrayString themes;
    const ColoursAndFontsManager::Vec_t& lexers = iter->second;
    for(size_t i = 0; i < lexers.size(); ++i) {
        themes.Add(lexers.at(i)->GetThemeName());
    }

    // sort the list
    themes.Sort();
    return themes;
}

LexerConf::Ptr_t ColoursAndFontsManager::GetLexer(const wxString& lexerName, const wxString& theme) const
{
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.find(lexerName.Lower());
    if(iter == m_lexersMap.end())
        return m_defaultLexer;

    // Locate the requested theme
    LexerConf::Ptr_t firstLexer(NULL);
    LexerConf::Ptr_t defaultLexer(NULL);
    LexerConf::Ptr_t defaultDarkLexer(NULL);
    LexerConf::Ptr_t defaultLightLexer(NULL);

    // assume that the text theme is always up to date and exist
    // based on that information, extract the active theme
    auto textThemeIter = m_lexersMap.find("text");
    LexerConf::Ptr_t defaultTextLexer(nullptr);
    for(auto textTheme : textThemeIter->second) {
        if(textTheme->IsActive()) {
            defaultTextLexer = textTheme;
        }
    }

    if(theme.IsEmpty()) {
        // return the active theme
        auto& allLexers = iter->second;
        for(auto lexer : allLexers) {

            if(!firstLexer) {
                firstLexer = lexer;
            }
            if(!defaultLexer && lexer->GetThemeName() == DEFAULT_THEME) {
                defaultLexer = lexer;
            }
            if(!defaultDarkLexer && lexer->IsDark()) {
                defaultDarkLexer = lexer;
            }
            if(!defaultLightLexer && !lexer->IsDark()) {
                defaultLightLexer = lexer;
            }
            if(lexer->IsActive()) {
                return lexer;
            }
        }

        //
        // No match, we use this logic to determine the best lexer:
        //
        if(defaultDarkLexer && defaultTextLexer && defaultTextLexer->IsDark()) {
            // return the first dark lexer for this language
            return defaultDarkLexer;
        } else if(defaultLightLexer && defaultTextLexer && !defaultTextLexer->IsDark()) {
            // return the first light lexer for this language
            return defaultLightLexer;
        } else if(defaultLexer) {
            // return the lexer that matches the DEFAULT_THEME theme
            return defaultLexer;
        } else if(firstLexer) {
            // return the first one that we found
            return firstLexer;
        } else {
            // no lexers for this language, return the default lexer (which always exist)
            return m_defaultLexer;
        }

    } else {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        LexerConf::Ptr_t themeDefaultLexer = nullptr;
        for(size_t i = 0; i < lexers.size(); ++i) {
            if(lexers[i]->GetThemeName() == DEFAULT_THEME) {
                themeDefaultLexer = lexers[i];
            }
            if(lexers[i]->GetThemeName() == theme) {
                return lexers[i];
            }
        }
        // We failed to find the requested theme for this language. If we have a DEFAULT_THEME
        // lexer, return it, else use the minimal lexer ("m_defaultLexer")
        return (themeDefaultLexer ? themeDefaultLexer : m_defaultLexer);
    }
}

void ColoursAndFontsManager::Save(const wxFileName& lexer_json)
{
    bool for_export = lexer_json.IsOk();
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.begin();
    JSON root(cJSON_Array);
    JSONItem element = root.toElement();
    for(; iter != m_lexersMap.end(); ++iter) {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {
            element.arrayAppend(lexers.at(i)->ToJSON(for_export));
        }
    }

    wxFileName output_file = lexer_json;
    if(!output_file.IsOk()) {
        output_file = wxFileName(clStandardPaths::Get().GetUserDataDir(), "lexers.json");
        output_file.AppendDir("lexers");
    }
    output_file.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    root.save(output_file);
    // store the global font as well
    if(m_globalFont.IsOk()) {
        clConfig::Get().Write("GlobalThemeFont", m_globalFont);
    }
    SaveGlobalSettings();

    clCommandEvent event(wxEVT_CMD_COLOURS_FONTS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(event);
}

wxArrayString ColoursAndFontsManager::GetAllLexersNames() const
{
    wxArrayString names;
    for(size_t i = 0; i < m_allLexers.size(); ++i) {
        LexerConf::Ptr_t lexer = m_allLexers.at(i);
        if(names.Index(lexer->GetName()) == wxNOT_FOUND) {
            names.Add(lexer->GetName());
        }
    }
    names.Sort();
    return names;
}

LexerConf::Ptr_t ColoursAndFontsManager::GetLexerForFile(const wxString& filename) const
{
    if(filename.IsEmpty())
        return GetLexer("text");

    wxFileName fnFileName(filename);
    wxString fileNameLowercase = fnFileName.GetFullName();
    fileNameLowercase.MakeLower();

    LexerConf::Ptr_t defaultLexer = nullptr;
    LexerConf::Ptr_t firstLexer = nullptr;
    LexerConf::Ptr_t firstLexerForTheme = nullptr;

    // try and find a match for the current theme
    const wxString& theme_name = GetGlobalTheme();

    // Scan the list of lexers, locate the active lexer for it and return it
    for(auto lexer : m_allLexers) {
        wxString fileMask = lexer->GetFileSpec();
        if(FileUtils::WildMatch(fileMask, filename)) {
            if(lexer->IsActive()) {
                return lexer;

            } else if(!firstLexer) {
                firstLexer = lexer;

            } else if(!defaultLexer && lexer->GetThemeName() == DEFAULT_THEME) {
                defaultLexer = lexer;
            }

            // try to find a theme that matches the current one
            if(!firstLexerForTheme && lexer->GetThemeName() == theme_name) {
                firstLexerForTheme = lexer;
            }
        }
    }

    // If we got here, it means that we could not find an active lexer that matches
    // the file mask. However, if we did find a "firstLexer" it means
    // that we do have a lexer that matches the file extension, its just that it is not
    // set as active
    if(firstLexerForTheme) {
        return firstLexerForTheme;
    } else if(firstLexer) {
        return firstLexer;
    }

    // Try this:
    // Use the FileExtManager to get the file type by examinig its content
    LexerConf::Ptr_t lexerByContent; // Null by default
    FileExtManager::FileType fileType = FileExtManager::TypeOther;
    if(!FileExtManager::AutoDetectByContent(filename, fileType)) {
        fileType = FileExtManager::GetType(filename);
    }

    if(fileType != FileExtManager::TypeOther) {
        switch(fileType) {
        case FileExtManager::TypeShellScript:
            lexerByContent = GetLexer("script");
            break;
        case FileExtManager::TypeJS:
            lexerByContent = GetLexer("javascript");
            break;
        case FileExtManager::TypePhp:
            lexerByContent = GetLexer("php");
            break;
        case FileExtManager::TypeSourceCpp:
            lexerByContent = GetLexer("c++");
            break;
        case FileExtManager::TypeXml:
            lexerByContent = GetLexer("xml");
            break;
        case FileExtManager::TypePython:
            lexerByContent = GetLexer("python");
            break;
        case FileExtManager::TypeRuby:
            lexerByContent = GetLexer("ruby");
            break;
        case FileExtManager::TypeRust:
            lexerByContent = GetLexer("rust");
            break;
        case FileExtManager::TypeJava:
            lexerByContent = GetLexer("java");
            break;
        case FileExtManager::TypeWorkspaceDocker:
        case FileExtManager::TypeWorkspaceFileSystem:
        case FileExtManager::TypeWorkspaceNodeJS:
        case FileExtManager::TypeWorkspacePHP:
        case FileExtManager::TypeWxCrafter:
        case FileExtManager::TypeJSON:
#if wxCHECK_VERSION(3, 1, 0)
            lexerByContent = GetLexer("json");
#else
            lexerByContent = GetLexer("javascript");
#endif
            break;
        default:
            break;
        }
    }

    // If we managed to find a lexer by content, use it
    if(lexerByContent)
        return lexerByContent;

    // If we reached here, it means we could not locate an active lexer for this file type
    if(defaultLexer) {
        return defaultLexer;

    } else if(firstLexer) {
        return firstLexer;

    } else {
        // Return the "Text" lexer
        return GetLexer("text");
    }
}

void ColoursAndFontsManager::Reload()
{
    Clear();
    Load();
}

void ColoursAndFontsManager::Clear()
{
    m_allLexers.clear();
    m_lexersMap.clear();
    m_initialized = false;
}

void ColoursAndFontsManager::SetActiveTheme(const wxString& lexerName, const wxString& themeName)
{
    wxArrayString themes = GetAvailableThemesForLexer(lexerName);
    for(size_t i = 0; i < themes.GetCount(); ++i) {
        LexerConf::Ptr_t lexer = GetLexer(lexerName, themes.Item(i));
        if(lexer && lexer->GetName() == lexerName) {
            lexer->SetIsActive(lexer->GetThemeName() == themeName);
        }
    }
}

wxFileName ColoursAndFontsManager::GetConfigFile() const
{
    wxFileName fnSettings(clStandardPaths::Get().GetUserDataDir(), "ColoursAndFonts.conf");
    fnSettings.AppendDir("config");
    return fnSettings;
}

void ColoursAndFontsManager::SaveGlobalSettings()
{
    // save the global settings
    JSON root(cJSON_Object);
    root.toElement().addProperty("m_globalTheme", m_globalTheme);
    wxFileName fnSettings = GetConfigFile();
    root.save(fnSettings.GetFullPath());

    wxCommandEvent evtThemeChanged(wxEVT_CL_THEME_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evtThemeChanged);
}

LexerConf::Ptr_t ColoursAndFontsManager::CopyTheme(const wxString& lexerName, const wxString& themeName,
                                                   const wxString& sourceTheme)
{
    LexerConf::Ptr_t sourceLexer = GetLexer(lexerName, sourceTheme);
    CHECK_PTR_RET_NULL(sourceLexer);

    JSONItem json = sourceLexer->ToJSON();
    LexerConf::Ptr_t newLexer(new LexerConf());
    newLexer->FromJSON(json);

    // Update the theme name
    newLexer->SetThemeName(themeName);

    // Add it
    return DoAddLexer(newLexer->ToJSON());
}

void ColoursAndFontsManager::RestoreDefaults()
{
    wxFont font = GetGlobalFont();
    wxString globalTheme = GetGlobalTheme();

    // First we delete the user settings
    {
        wxLogNull noLog;
        wxFileName fnLexersJSON(clStandardPaths::Get().GetUserLexersDir(), "lexers.json");
        if(fnLexersJSON.Exists()) {
            clRemoveFile(fnLexersJSON.GetFullPath());
        }
    }

    // Now, we simply reload the settings
    Reload();

    SetGlobalFont(font);
    SetGlobalTheme(globalTheme);
}

wxString ColoursAndFontsManager::ImportEclipseTheme(const wxString& theme_file)
{
    if(!theme_file.IsEmpty()) {
        ThemeImporterManager importer;
        return importer.Import(theme_file);
    }
    return wxEmptyString;
}

void ColoursAndFontsManager::OnLexerFilesLoaded(const std::vector<wxXmlDocument*>& userLexers)
{
    // User lexers
    wxFileName fnUserLexers(clStandardPaths::Get().GetUserDataDir(), "lexers.json");
    fnUserLexers.AppendDir("lexers");

    // Default installation lexers
    wxFileName defaultLexersFileName(clStandardPaths::Get().GetDataDir(), "");

    defaultLexersFileName.AppendDir("lexers");
    defaultLexersFileName.SetFullName("lexers.json");

    wxString str_defaultLexersFileName = defaultLexersFileName.GetFullPath();
    wxUnusedVar(str_defaultLexersFileName);

    m_allLexers.clear();
    m_lexersMap.clear();

    if(!fnUserLexers.FileExists()) {
        // Load default settings
        LoadJSON(defaultLexersFileName);

        // Use old XML files
        LoadOldXmls(userLexers);

        // Call save to create an initial user settings
        Save();

    } else {
        // Load the user settings
        LoadJSON(fnUserLexers);
    }
    // Update lexers versions
    clConfig::Get().Write(LEXERS_VERSION_STRING, LEXERS_VERSION);
}

void ColoursAndFontsManager::UpdateLexerColours(LexerConf::Ptr_t lexer, bool force)
{
    StyleProperty& defaultProp = lexer->GetProperty(0); // Default
    if(force || m_lexersVersion < 1) {
        // adjust line numbers
        if(lexer->IsDark()) {
            StyleProperty& lineNumbers = lexer->GetProperty(LINE_NUMBERS_ATTR_ID); // Line numbers
            if(!defaultProp.IsNull()) {
                if(lexer->GetName() == "c++") {
                    defaultProp.SetFgColour(
                        wxColour(defaultProp.GetBgColour()).ChangeLightness(120).GetAsString(wxC2S_HTML_SYNTAX));
                }
                if(!lineNumbers.IsNull()) {
                    lineNumbers.SetFgColour(
                        wxColour(defaultProp.GetBgColour()).ChangeLightness(120).GetAsString(wxC2S_HTML_SYNTAX));
                    lineNumbers.SetBgColour(defaultProp.GetBgColour());
                }
            }

        } else {
            lexer->SetLineNumbersFgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
            StyleProperty& lineNumbers = lexer->GetProperty(LINE_NUMBERS_ATTR_ID); // Line numbers
            if(!lineNumbers.IsNull()) {
                lineNumbers.SetBgColour(defaultProp.GetBgColour());
            }

            // don't adjust PHP and HTML default colours, since they also affects the various operators
            // foreground colours
            if(lexer->GetName() != "php" && lexer->GetName() != "html" && lexer->GetName() != "text" &&
               lexer->GetName() != "cmake" && lexer->GetName() != "xml") {
                lexer->SetDefaultFgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
            }
        }
    }

    //=====================================================================
    // Second upgrade stage: adjust whitespace colour and fold margin
    //=====================================================================
    if(force || m_lexersVersion < 2) {
        // adjust line numbers
        StyleProperty& fold = lexer->GetProperty(FOLD_MARGIN_ATTR_ID);       // fold margin
        StyleProperty& whitespace = lexer->GetProperty(WHITE_SPACE_ATTR_ID); // whitespace
        if(lexer->IsDark()) {
            wxColour newCol = wxColour(defaultProp.GetBgColour()).ChangeLightness(110);

            fold.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
            fold.SetBgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
            whitespace.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));

        } else {
            wxColour newCol = wxColour(defaultProp.GetBgColour()).ChangeLightness(95);

            fold.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
            fold.SetBgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
            whitespace.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
        }
    }

    //=====================================================================
    // Third upgrade stage: adjust whitespace colour and fold margin
    //=====================================================================
    if(force || m_lexersVersion < 3) {
        // remove the *.js;*.javascript from the C++ lexer
        if(lexer->GetName() == "c++") {
            lexer->SetFileSpec("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.ipp;*.hxx;*.h++");
        }
    }

    if((lexer->GetName() == "c++") && (lexer->GetFileSpec().IsEmpty() || !lexer->GetFileSpec().Contains("*.cpp"))) {
        lexer->SetFileSpec("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.ipp;*.hxx;*.h++;*.ino");
    }

    // Add Arduino sketches files as C++ (*.ino)
    if(lexer->GetName() == "c++" && !lexer->GetFileSpec().Contains(".ino")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.ino");
    }

    // Upgrade CSS colours
    if((force || m_lexersVersion < 4) && lexer->GetName().Lower() == "css") {
        // adjust line numbers
        bool isDark = lexer->IsDark();
        StyleProperty& var = lexer->GetProperty(wxSTC_CSS_VARIABLE);
        StyleProperty& identifier = lexer->GetProperty(wxSTC_CSS_IDENTIFIER);
        StyleProperty& identifier2 = lexer->GetProperty(wxSTC_CSS_IDENTIFIER2);
        StyleProperty& identifier3 = lexer->GetProperty(wxSTC_CSS_IDENTIFIER3);
        StyleProperty& oper = lexer->GetProperty(wxSTC_CSS_OPERATOR);
        if(!var.IsNull()) {
            if(!identifier.IsNull()) {
                identifier.SetFgColour(var.GetFgColour());
            }
            if(!identifier2.IsNull()) {
                identifier2.SetFgColour(var.GetFgColour());
            }
            if(!identifier3.IsNull()) {
                identifier3.SetFgColour(var.GetFgColour());
            }
            if(!oper.IsNull()) {
                oper.SetFgColour(isDark ? "WHITE" : "BLACK");
            }
        }
    }

    if(force || m_lexersVersion < 5) {
        // Indentation guides (style #37)
        StyleProperty& indentGuides = lexer->GetProperty(37);
        indentGuides.SetFgColour(defaultProp.GetBgColour());
        indentGuides.SetBgColour(defaultProp.GetBgColour());
    }

    if(force || m_lexersVersion < 6) {
        StyleProperty& caret = lexer->GetProperty(CARET_ATTR_ID);
        if(lexer->IsDark()) {
            caret.SetFgColour("rgb(255, 128, 0)");
        }
    }
}

void ColoursAndFontsManager::SetTheme(const wxString& themeName)
{
    LexerConf::Ptr_t lexer = GetLexer("c++", themeName);
    CHECK_PTR_RET(lexer);

    bool isDark = lexer->IsDark();
    wxString fallbackTheme;
    if(isDark) {
        fallbackTheme = "One Dark Like";
    } else {
        fallbackTheme = "Atom One Light";
    }

    const wxArrayString& lexers = GetAllLexersNames();
    for(size_t i = 0; i < lexers.size(); ++i) {
        wxArrayString themesForLexer = GetAvailableThemesForLexer(lexers.Item(i));
        if(themesForLexer.Index(themeName) == wxNOT_FOUND) {
            SetActiveTheme(lexers.Item(i), fallbackTheme);
        } else {
            SetActiveTheme(lexers.Item(i), themeName);
        }
    }
    SetGlobalTheme(themeName);

    clColours colours;
    bool useCustomColour = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustomColour) {
        wxColour bgColour = GetBackgroundColourFromLexer(lexer);
        clConfig::Get().Write("BaseColour", bgColour);
    }
}

void ColoursAndFontsManager::LoadJSON(const wxFileName& path)
{
    if(!path.FileExists())
        return;

    JSON root(path);
    JSONItem arr = root.toElement();
    int arrSize = arr.arraySize();
    CL_DEBUG("Loading JSON file: %s (contains %d lexers)", path.GetFullPath(), arrSize);
    for(int i = 0; i < arrSize; ++i) {
        JSONItem json = arr.arrayItem(i);
        DoAddLexer(json);
    }
    CL_DEBUG("Loading JSON file...done");
}

LexerConf::Ptr_t ColoursAndFontsManager::DoAddLexer(JSONItem json)
{
    LexerConf::Ptr_t lexer(new LexerConf());
    lexer->FromJSON(json);

    wxString lexerName = lexer->GetName().Lower();
    if(lexerName.IsEmpty())
        return NULL;

    // ensure that the theme name is capitalized - this helps
    // when displaying the content in a wxListBox sorted
    wxString themeName = lexer->GetThemeName();
    themeName = themeName.Mid(0, 1).Capitalize() + themeName.Mid(1);
    lexer->SetThemeName(themeName);

    clDEBUG1() << "Loading lexer:" << lexerName;

    // Fix C++ lexer
    if(lexer->GetName() == "c++") {
        AddLexerKeywords(lexer, 0, { "override", "final", "constexpr" });
        wxString filespec = lexer->GetFileSpec();
        filespec.Replace("*.javascript", wxEmptyString);
        filespec.Replace("*.js", wxEmptyString);
        lexer->SetFileSpec(filespec);
    }

    // Hack: fix Java lexer which is using the same
    // file extensions as C++...
    if(lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) {
        lexer->SetFileSpec("*.java");
    }

    // Append *.sqlite to the SQL lexer if missing
    if(lexer->GetName() == "sql" && !lexer->GetFileSpec().Contains(".sqlite")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.sqlite");
    }

    // Hack2: since we now provide our own PHP and javaScript lexer, remove the PHP/JS extensions from
    // the HTML lexer
    if(lexer->GetName() == "html" && (lexer->GetFileSpec().Contains(".php") || lexer->GetFileSpec().Contains("*.js"))) {
        lexer->SetFileSpec("*.htm;*.html;*.xhtml");
    }

    // Hack3: all the HTML support to PHP which have much more colour themes
    if(lexer->GetName() == "html" && lexer->GetFileSpec().Contains(".html")) {
        lexer->SetFileSpec("*.vbs;*.vbe;*.wsf;*.wsc;*.asp;*.aspx");
    }

    // Hack5: all the remove *.scss from the css lexer (it now has its own lexer)
    if(lexer->GetName() == "css" && lexer->GetFileSpec().Contains(".scss")) {
        lexer->SetFileSpec("*.css");
    }

    // Add *.less file extension to the css lexer
    if(lexer->GetName() == "css" && !lexer->GetFileSpec().Contains(".less")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.less");
    }

    if(lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".html")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.html;*.htm;*.xhtml");
    }

    if(lexer->GetName() == "php" && !lexer->GetKeyWords(4).Contains("<?php")) {
        lexer->SetKeyWords(lexer->GetKeyWords(4) + " <?php <? ", 4);
    }

    if(lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".php5")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.php5");
    }

    if(lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".ctp")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.ctp");
    }

    // Add wxcp file extension to the JavaScript lexer
    if(lexer->GetName() == "javascript") {
#if wxCHECK_VERSION(3, 1, 0)
        // remove the JSON file from the JavaScript
        if(lexer->GetFileSpec().Contains("*.json")) {
            auto specs = ::wxStringTokenize(lexer->GetFileSpec(), ";,", wxTOKEN_STRTOK);
            int where = specs.Index("*.json");
            if(where != wxNOT_FOUND) {
                specs.RemoveAt(where);
            }
            lexer->SetFileSpec(wxJoin(specs, ';'));
        }
#else
        // wxCrafter files
        if(!lexer->GetFileSpec().Contains(".wxcp")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.wxcp");
        }

#endif
        if(!lexer->GetFileSpec().Contains(".qml")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.qml");
        }
        // typescript
        if(!lexer->GetFileSpec().Contains(".ts")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.ts");
        }
        const wxString ts_keywords = "break as any "
                                     "case implements boolean "
                                     "catch interface constructor "
                                     "class let declare "
                                     "const package get "
                                     "continue private module "
                                     "debugger protected require "
                                     "default public number "
                                     "delete static set "
                                     "do yield string "
                                     "else symbol "
                                     "enum type "
                                     "export from "
                                     "extends of "
                                     "false  "
                                     "finally  "
                                     "for  "
                                     "function  "
                                     "if  "
                                     "import  "
                                     "in  "
                                     "instanceof  "
                                     "new  "
                                     "null  "
                                     "readonly "
                                     "return  "
                                     "super  "
                                     "switch  "
                                     "this  "
                                     "throw  "
                                     "true  "
                                     "try  "
                                     "typeof  "
                                     "var  "
                                     "void  "
                                     "while  "
                                     "with ";
        wxArrayString arr_keywords = ::wxStringTokenize(ts_keywords, " \t\n", wxTOKEN_STRTOK);
        std::vector<wxString> vec_keywords;
        vec_keywords.reserve(arr_keywords.size());
        vec_keywords.insert(vec_keywords.end(), arr_keywords.begin(), arr_keywords.end());

        AddLexerKeywords(lexer, 0, vec_keywords);
    }

    // update script lexer with additional keywords
    if(lexer->GetName() == "script") {
        AddLexerKeywords(lexer, 0, { "return", "exit", "local", "function" });
    }

    if(lexer->GetName() == "text") {
        lexer->SetFileSpec(wxEmptyString);
    }

    // Set the JavaScript keywords
    if(lexer->GetName() == "php" && !lexer->GetKeyWords(1).Contains("instanceof")) {
        lexer->SetKeyWords(
            "break do instanceof typeof case else new var catch finally return void continue for switch while "
            "debugger function this with default if throw delete in try abstract  export interface  static "
            "boolean  extends "
            "long super"
            "byte final native synchronized char float package throws class goto private transient const "
            "implements  protected "
            " volatile double import  public enum "
            "int short null true false",
            1);
    }

    if(lexer->GetName() == "python") {
        AddLexerKeywords(lexer, 0, { "await", "async", "True", "False", "None", "pass", "self" });
        lexer->SetKeyWords("", 1);
        lexer->SetKeyWords("", 2);
        lexer->SetKeyWords("", 3);
        lexer->SetKeyWords("", 4);
        auto& sp_word_0 = lexer->GetProperty(wxSTC_P_WORD);
        auto& sp_word_2 = lexer->GetProperty(wxSTC_P_WORD2);
        if(sp_word_2.GetFgColour() == sp_word_0.GetFgColour()) {
            // old configuration, dont set the variables fg colour to the same colour as the keywords
            sp_word_2.SetFgColour(lexer->GetProperty(wxSTC_P_DEFNAME).GetFgColour());
        }
    }

    // make sure we include Rakefile as a Ruby file
    if(lexer->GetName() == "ruby") {
        if(!lexer->GetFileSpec().Contains("Rakefile")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";Rakefile");
        }
    }

    if(lexer->GetName() == "makefile" && !lexer->GetFileSpec().Contains("*akefile.am")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*akefile.in;*akefile.am");
    }
    if(lexer->GetName() == "properties") {
        AddFileExtension(lexer, "*.toml");
    }

    // Upgrade the lexer colours
    UpdateLexerColours(lexer, false);

    if(m_lexersMap.count(lexerName) == 0) {
        m_lexersMap.insert({ lexerName, ColoursAndFontsManager::Vec_t() });
    }

    ColoursAndFontsManager::Vec_t& vec = m_lexersMap.find(lexerName)->second;

    // Locate an instance with this name and theme in
    // both the m_alllexers and vector for this lexer
    // name
    ColoursAndFontsManager::Vec_t::iterator iter =
        std::find_if(vec.begin(), vec.end(), LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
    if(iter != vec.end()) {
        vec.erase(iter);
    }

    iter = std::find_if(m_allLexers.begin(), m_allLexers.end(),
                        LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
    if(iter != m_allLexers.end()) {
        m_allLexers.erase(iter);
    }
    vec.push_back(lexer);
    m_allLexers.push_back(lexer);
    return lexer;
}

void ColoursAndFontsManager::AddLexer(LexerConf::Ptr_t lexer)
{
    CHECK_PTR_RET(lexer);
    DoAddLexer(lexer->ToJSON());
}

void ColoursAndFontsManager::SetGlobalFont(const wxFont& font)
{
    this->m_globalFont = font;

    // Loop for every lexer and update the font per style
    std::for_each(m_allLexers.begin(), m_allLexers.end(), [&](LexerConf::Ptr_t lexer) {
        StyleProperty::Map_t& props = lexer->GetLexerProperties();
        StyleProperty::Map_t::iterator iter = props.begin();
        for(; iter != props.end(); ++iter) {
            StyleProperty& sp = iter->second;
            sp.SetFaceName(font.GetFaceName());
            sp.SetFontSize(font.GetPointSize());
            sp.SetBold(font.GetWeight() == wxFONTWEIGHT_BOLD);
            sp.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
            sp.SetUnderlined(font.GetUnderlined());
        }
    });
}

const wxFont& ColoursAndFontsManager::GetGlobalFont() const { return this->m_globalFont; }

bool ColoursAndFontsManager::ExportThemesToFile(const wxFileName& outputFile, const wxArrayString& names) const
{
    wxStringSet_t M;
    for(size_t i = 0; i < names.size(); ++i) {
        M.insert(names.Item(i).Lower());
    }

    JSON root(cJSON_Array);
    JSONItem arr = root.toElement();
    std::vector<LexerConf::Ptr_t> Lexers;
    std::for_each(m_allLexers.begin(), m_allLexers.end(), [&](LexerConf::Ptr_t lexer) {
        if(M.empty() || M.count(lexer->GetThemeName().Lower())) {
            Lexers.push_back(lexer);
        }
    });
    std::for_each(Lexers.begin(), Lexers.end(), [&](LexerConf::Ptr_t lexer) { arr.append(lexer->ToJSON(true)); });
    return FileUtils::WriteFileContent(outputFile, root.toElement().format());
}

bool ColoursAndFontsManager::ImportLexersFile(const wxFileName& inputFile, bool prompt)
{
    JSON root(inputFile);
    if(!root.isOk()) {
        clWARNING() << "Invalid lexers input file:" << inputFile << clEndl;
        return false;
    }

    if(prompt) {
        if(::wxMessageBox(
               _("Importing syntax highlight file will override any duplicate syntax highlight settings.\nContinue?"),
               "CodeLite", wxICON_QUESTION | wxYES_NO | wxCANCEL | wxYES_DEFAULT, NULL) != wxYES) {
            return false;
        }
    }

    std::vector<LexerConf::Ptr_t> Lexers;
    JSONItem arr = root.toElement();
    int arrSize = arr.arraySize();
    for(int i = 0; i < arrSize; ++i) {
        JSONItem lexerObj = arr.arrayItem(i);
        LexerConf::Ptr_t lexer(new LexerConf());
        lexer->FromJSON(lexerObj);
        Lexers.push_back(lexer);
    }

    std::for_each(Lexers.begin(), Lexers.end(), [&](LexerConf::Ptr_t lexer) {
        if(m_lexersMap.count(lexer->GetName()) == 0) {
            m_lexersMap[lexer->GetName()] = Vec_t();
        }
        Vec_t& v = m_lexersMap[lexer->GetName()];
        Vec_t::iterator iter = std::find_if(
            v.begin(), v.end(), [&](LexerConf::Ptr_t l) { return l->GetThemeName() == lexer->GetThemeName(); });
        if(prompt) {
            // Override this theme with the new one
            if(iter != v.end()) {
                // erase old lexer
                v.erase(iter);
            }
            v.push_back(lexer);
        } else {
            // We dont have this theme, add it
            if(iter == v.end()) {
                v.push_back(lexer);
            }
        }
    });

    // Rebuild "m_allLexers" after the merge
    m_allLexers.clear();
    std::for_each(m_lexersMap.begin(), m_lexersMap.end(), [&](ColoursAndFontsManager::Map_t::value_type& vt) {
        std::for_each(vt.second.begin(), vt.second.end(),
                      [&](LexerConf::Ptr_t lexer) { m_allLexers.push_back(lexer); });
    });
    Save();
    Reload();
    return true;
}

wxArrayString ColoursAndFontsManager::GetAllThemes() const
{
    wxStringSet_t themes;
    std::for_each(m_allLexers.begin(), m_allLexers.end(),
                  [&](LexerConf::Ptr_t lexer) { themes.insert(lexer->GetThemeName()); });
    wxArrayString arr;
    std::for_each(themes.begin(), themes.end(), [&](const wxString& name) { arr.push_back(name); });
    return arr;
}

void ColoursAndFontsManager::OnAdjustTheme(clCommandEvent& event)
{
    event.Skip();
    if(event.GetInt() != XRCID("adjust-current-theme")) {
        return;
    }
    event.Skip(false);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(!lexer) {
        return;
    }

    wxColour bgColour = GetBackgroundColourFromLexer(lexer);
    if(!bgColour.IsOk()) {
        return;
    }

    // Adjust the colours
    // Save the base colour changes
    clConfig::Get().Write("BaseColour", bgColour);
    clConfig::Get().Write("UseCustomBaseColour", true);

    // Notify this change
    clCommandEvent evt(wxEVT_CMD_COLOURS_FONTS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

wxColour ColoursAndFontsManager::GetBackgroundColourFromLexer(LexerConf::Ptr_t lexer) const
{
    if(!lexer) {
        return wxNullColour;
    }
    wxColour bgColour;
    if(lexer->IsDark()) {
        bgColour = lexer->GetProperty(0).GetBgColour();
        bgColour = bgColour.ChangeLightness(110);
    } else {
        bgColour = lexer->GetProperty(0).GetBgColour();
        bgColour = bgColour.ChangeLightness(95);
    }
    return bgColour;
}

bool ColoursAndFontsManager::IsDarkTheme() const
{
    LexerConf::Ptr_t lexer = GetLexer("text");
    if(!lexer) {
        return false;
    }
    return lexer->IsDark();
}

void ColoursAndFontsManager::SetThemeTextSelectionColours(const wxString& theme_name, const wxColour& bg,
                                                          const wxColour& fg, bool useCustomerFgColour)
{
    wxString theme_name_lc = theme_name.Lower();
    for(auto& lexer : m_allLexers) {
        if(lexer->GetThemeName().CmpNoCase(theme_name) == 0) {
            auto& sp = lexer->GetProperty(SEL_TEXT_ATTR_ID);
            sp.SetBgColour(bg.GetAsString(wxC2S_HTML_SYNTAX));
            sp.SetFgColour(fg.GetAsString(wxC2S_HTML_SYNTAX));
            lexer->SetUseCustomTextSelectionFgColour(useCustomerFgColour);
        }
    }
}

wxFont ColoursAndFontsManager::GetFixedFont(bool small) const
{
    auto lexer = GetLexer("text");
    auto font = lexer->GetFontForSyle(0, EventNotifier::Get()->TopFrame());
#ifndef __WXMAC__
    if(small) {
#if wxCHECK_VERSION(3, 1, 2)
        font.SetFractionalPointSize(font.GetPointSize() * 0.9);
#else
        font.SetPointSize(font.GetPointSize() * 0.9);
#endif
    }
#else
    wxUnusedVar(small);
#endif
    return font;
}
