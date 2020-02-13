#include "ColoursAndFontsManager.h"
#include "EclipseThemeImporterManager.h"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "JSON.h"
#include "macros.h"
#include "wxStringHash.h"
#include "xmlutils.h"
#include <algorithm>
#include <codelite_events.h>
#include <wx/busyinfo.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include "globals.h"
#include <imanager.h>
#include <wx/xrc/xmlres.h>
#include "theme_handler_helper.h"

// Upgrade macros
#define LEXERS_VERSION_STRING "LexersVersion"
#define LEXERS_VERSION 6

wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_START, clCommandEvent);
wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_END, clCommandEvent);
wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_PROGRESS, clCommandEvent);

static const wxString LexerTextDefaultXML =
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
            if(!xmlFile.IsOpened()) continue;
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
    if(m_initialized) return;
    m_lexersMap.clear();
    m_initialized = true;
    m_globalTheme = "Default";

    // Load the global settings
    if(GetConfigFile().FileExists()) {
        JSON root(GetConfigFile());
        if(root.isOk()) { m_globalTheme = root.toElement().namedObject("m_globalTheme").toString("Default"); }
    }

    // Load the lexers
    ColoursAndFontsManagerLoaderHelper loader(this);
    loader.Load();
}

void ColoursAndFontsManager::LoadOldXmls(const std::vector<wxXmlDocument*>& xmlFiles, bool userLexers)
{
    // Each XMl represents a single lexer (the old format)
    for(size_t i = 0; i < xmlFiles.size(); ++i) {
        wxXmlDocument* doc = xmlFiles.at(i);
        DoAddLexer(doc->GetRoot());
    }
}

LexerConf::Ptr_t ColoursAndFontsManager::DoAddLexer(wxXmlNode* node)
{
    wxString lexerName = XmlUtils::ReadString(node, "Name");
    lexerName.MakeLower();
    if(lexerName.IsEmpty()) return NULL;

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
    if(lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) { lexer->SetFileSpec("*.java"); }

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
        wxString jsWords = lexer->GetKeyWords(0);
        wxArrayString arrWords = ::wxStringTokenize(jsWords, " ", wxTOKEN_STRTOK);
        // use std::set to make sure that the elements are sorted
        std::set<wxString> uniqueSet;
        for(size_t i = 0; i < arrWords.size(); ++i) {
            uniqueSet.insert(arrWords.Item(i));
        }
        uniqueSet.insert("class");
        uniqueSet.insert("await");
        uniqueSet.insert("async");
        jsWords.clear();
        std::for_each(uniqueSet.begin(), uniqueSet.end(), [&](const wxString& word) { jsWords << word << " "; });
        lexer->SetKeyWords(jsWords, 0);
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
    if(iter != vec.end()) { vec.erase(iter); }
    iter = std::find_if(m_allLexers.begin(), m_allLexers.end(),
                        LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
    if(iter != m_allLexers.end()) { m_allLexers.erase(iter); }
    vec.push_back(lexer);
    m_allLexers.push_back(lexer);
    return lexer;
}

wxArrayString ColoursAndFontsManager::GetAvailableThemesForLexer(const wxString& lexerName) const
{
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.find(lexerName.Lower());
    if(iter == m_lexersMap.end()) return wxArrayString();

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
    if(iter == m_lexersMap.end()) return m_defaultLexer;

    // Locate the requested theme
    LexerConf::Ptr_t firstLexer(NULL);
    LexerConf::Ptr_t defaultLexer(NULL);

    if(theme.IsEmpty()) {
        // return the active theme
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {

            if(!firstLexer) { firstLexer = lexers.at(i); }

            if(!defaultLexer && lexers.at(i)->GetThemeName() == "Default") { defaultLexer = lexers.at(i); }

            if(lexers.at(i)->IsActive()) return lexers.at(i);
        }

        // No match
        if(defaultLexer)
            return defaultLexer;
        else if(firstLexer)
            return firstLexer;
        else
            return m_defaultLexer;

    } else {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        LexerConf::Ptr_t themeDefaultLexer = nullptr;
        for(size_t i = 0; i < lexers.size(); ++i) {
            if(lexers[i]->GetThemeName() == "Default") { themeDefaultLexer = lexers[i]; }
            if(lexers[i]->GetThemeName() == theme) { return lexers[i]; }
        }
        // We failed to find the requested theme for this language. If we have a "Default"
        // lexer, return it, else use the minimal lexer ("m_defaultLexer")
        return (themeDefaultLexer ? themeDefaultLexer : m_defaultLexer);
    }
}

void ColoursAndFontsManager::Save(bool forExport)
{
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.begin();
    JSON root(cJSON_Array);
    JSONItem element = root.toElement();
    for(; iter != m_lexersMap.end(); ++iter) {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {
            element.arrayAppend(lexers.at(i)->ToJSON(forExport));
        }
    }

    wxFileName lexerFiles(clStandardPaths::Get().GetUserDataDir(), "lexers.json");
    lexerFiles.AppendDir("lexers");
    root.save(lexerFiles);
    SaveGlobalSettings();

    clCommandEvent event(wxEVT_CMD_COLOURS_FONTS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(event);
}

wxArrayString ColoursAndFontsManager::GetAllLexersNames() const
{
    wxArrayString names;
    for(size_t i = 0; i < m_allLexers.size(); ++i) {
        LexerConf::Ptr_t lexer = m_allLexers.at(i);
        if(names.Index(lexer->GetName()) == wxNOT_FOUND) { names.Add(lexer->GetName()); }
    }
    names.Sort();
    return names;
}

LexerConf::Ptr_t ColoursAndFontsManager::GetLexerForFile(const wxString& filename) const
{
    if(filename.IsEmpty()) return GetLexer("text");

    wxFileName fnFileName(filename);
    wxString fileNameLowercase = fnFileName.GetFullName();
    fileNameLowercase.MakeLower();

    LexerConf::Ptr_t defaultLexer(NULL);
    LexerConf::Ptr_t firstLexer(NULL);

    // Scan the list of lexers, locate the active lexer for it and return it
    ColoursAndFontsManager::Vec_t::const_iterator iter = m_allLexers.begin();
    for(; iter != m_allLexers.end(); ++iter) {
        wxString fileMask = (*iter)->GetFileSpec();
        if(FileUtils::WildMatch(fileMask, filename)) {
            if((*iter)->IsActive()) {
                return *iter;

            } else if(!firstLexer) {
                firstLexer = *iter;

            } else if(!defaultLexer && (*iter)->GetThemeName() == "Default") {
                defaultLexer = *iter;
            }
        }
    }

    // If we got here, it means that we could not find an active lexer that matches
    // the file mask. However, if we did find a "firstLexer" it means
    // that we do have a lexer that matches the file extension, its just that it is not
    // set as active
    if(firstLexer) { return firstLexer; }

    // Try this:
    // Use the FileExtManager to get the file type by examinig its content
    LexerConf::Ptr_t lexerByContent; // Null by default
    FileExtManager::FileType fileType = FileExtManager::TypeOther;
    if(FileExtManager::AutoDetectByContent(filename, fileType) && fileType != FileExtManager::TypeOther) {
        switch(fileType) {
        case FileExtManager::TypeScript:
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
        default:
            break;
        }
    }

    // If we managed to find a lexer by content, use it
    if(lexerByContent) return lexerByContent;

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
        if(lexer && lexer->GetName() == lexerName) { lexer->SetIsActive(lexer->GetThemeName() == themeName); }
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
    // First we delete the user settings
    {
        wxLogNull noLog;
        wxFileName fnLexersJSON(clStandardPaths::Get().GetUserLexersDir(), "lexers.json");
        if(fnLexersJSON.Exists()) { clRemoveFile(fnLexersJSON.GetFullPath()); }
    }

    // Now, we simply reload the settings
    Reload();
}

bool ColoursAndFontsManager::ImportEclipseTheme(const wxString& eclipseXml)
{
    bool res = false;
    if(!eclipseXml.IsEmpty()) {
        EclipseThemeImporterManager importer;
        return importer.Import(eclipseXml);
    }
    return res;
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
            if(!lineNumbers.IsNull()) { lineNumbers.SetBgColour(defaultProp.GetBgColour()); }

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
            if(!identifier.IsNull()) { identifier.SetFgColour(var.GetFgColour()); }
            if(!identifier2.IsNull()) { identifier2.SetFgColour(var.GetFgColour()); }
            if(!identifier3.IsNull()) { identifier3.SetFgColour(var.GetFgColour()); }
            if(!oper.IsNull()) { oper.SetFgColour(isDark ? "WHITE" : "BLACK"); }
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
    if(!path.FileExists()) return;

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
    if(lexerName.IsEmpty()) return NULL;

    // ensure that the theme name is capitalized - this helps
    // when displaying the content in a wxListBox sorted
    wxString themeName = lexer->GetThemeName();
    themeName = themeName.Mid(0, 1).Capitalize() + themeName.Mid(1);
    lexer->SetThemeName(themeName);

    clDEBUG1() << "Loading lexer:" << lexerName;

    if(lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("final")) {
        lexer->SetKeyWords(lexer->GetKeyWords(0) + " final", 0);
    }

    if(lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("override")) {
        lexer->SetKeyWords(lexer->GetKeyWords(0) + " override", 0);
    }

    // Hack: fix Java lexer which is using the same
    // file extensions as C++...
    if(lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) { lexer->SetFileSpec("*.java"); }

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

    // Hack5: all the remove *.scss from the css lexer (it now has its own lexer)
    if(lexer->GetName() == "css" && lexer->GetFileSpec().Contains(".scss")) { lexer->SetFileSpec("*.css"); }

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
    if(lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".wxcp")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.wxcp");
    }

    if(lexer->GetName() == "javascript") {
        wxString jsWords = lexer->GetKeyWords(0);
        wxArrayString arrWords = ::wxStringTokenize(jsWords, " ", wxTOKEN_STRTOK);
        // use std::set to make sure that the elements are sorted
        std::set<wxString> uniqueSet;
        for(size_t i = 0; i < arrWords.size(); ++i) {
            uniqueSet.insert(arrWords.Item(i));
        }
        uniqueSet.insert("class");
        uniqueSet.insert("await");
        uniqueSet.insert("async");
        uniqueSet.insert("extends");
        uniqueSet.insert("constructor");
        uniqueSet.insert("super");
        jsWords.clear();
        std::for_each(uniqueSet.begin(), uniqueSet.end(), [&](const wxString& word) { jsWords << word << " "; });
        lexer->SetKeyWords(jsWords, 0);
    }

    if(lexer->GetName() == "text") { lexer->SetFileSpec(wxEmptyString); }

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
    if(iter != vec.end()) { vec.erase(iter); }

    iter = std::find_if(m_allLexers.begin(), m_allLexers.end(),
                        LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
    if(iter != m_allLexers.end()) { m_allLexers.erase(iter); }
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
        if(M.empty() || M.count(lexer->GetThemeName().Lower())) { Lexers.push_back(lexer); }
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
        if(m_lexersMap.count(lexer->GetName()) == 0) { m_lexersMap[lexer->GetName()] = Vec_t(); }
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
            if(iter == v.end()) { v.push_back(lexer); }
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
    if(event.GetInt() != XRCID("adjust-current-theme")) { return; }
    event.Skip(false);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(!lexer) { return; }

    wxColour bgColour = GetBackgroundColourFromLexer(lexer);
    if(!bgColour.IsOk()) { return; }

    // Adjust the colours
    // Save the base colour changes
    clConfig::Get().Write("BaseColour", bgColour);
    clConfig::Get().Write("UseCustomBaseColour", true);

    // Notify this change
    clCommandEvent evt(wxEVT_CMD_COLOURS_FONTS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(evt);

    clGetManager()->DisplayMessage(_("A CodeLite restart is needed. Would you like to restart it now?"),
                                   wxICON_QUESTION, { { XRCID("restart-codelite"), _("Yes") }, { wxID_NO, _("No") } });
}

wxColour ColoursAndFontsManager::GetBackgroundColourFromLexer(LexerConf::Ptr_t lexer) const
{
    if(!lexer) { return wxNullColour; }
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
    if(!lexer) { return false; }
    return lexer->IsDark();
}
