#include "ColoursAndFontsManager.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>
#include <wx/dir.h>
#include "xmlutils.h"
#include <wx/xml/xml.h>
#include "editor_config.h"
#include "globals.h"
#include "event_notifier.h"
#include <codelite_events.h>
#include "cl_command_event.h"
#include "json_node.h"
#include "file_logger.h"
#include <algorithm>
#include "macros.h"
#include <wx/settings.h>
#include <wx/tokenzr.h>
#include "EclipseThemeImporterManager.h"
#include "fileextmanager.h"
#include "file_logger.h"
#include <wx/sstream.h>
#include "cl_command_event.h"
#include <wx/busyinfo.h>

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

    m_globalBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    m_globalFgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_lexersVersion = clConfig::Get().Read(LEXERS_VERSION_STRING, LEXERS_UPGRADE_LINENUM_DEFAULT_COLOURS);
}

ColoursAndFontsManager::~ColoursAndFontsManager() { clConfig::Get().Write(LEXERS_VERSION_STRING, LEXERS_VERSION); }

ColoursAndFontsManager& ColoursAndFontsManager::Get()
{
    static ColoursAndFontsManager s_theManager;
    return s_theManager;
}

/**
 * @class ColoursAndFontsManager_HelperThread
 * @brief
 */
struct ColoursAndFontsManager_HelperThread : public wxThread {
    ColoursAndFontsManager* m_manager;
    ColoursAndFontsManager_HelperThread(ColoursAndFontsManager* manager)
        : wxThread(wxTHREAD_DETACHED)
        , m_manager(manager)
    {
    }

    /**
     * @brief the lexer reader thread entry point
     */
    virtual void* Entry()
    {
        std::vector<wxXmlDocument*> defaultLexers;
        std::vector<wxXmlDocument*> userLexers;
        wxArrayString files;
//---------------------------------------------
// Load the installation lexers
//---------------------------------------------
#ifdef USE_POSIX_LAYOUT
        wxFileName defaultLexersPath(clStandardPaths::Get().GetDataDir() + wxT(INSTALL_DIR), "");
#else
        wxFileName defaultLexersPath(clStandardPaths::Get().GetDataDir(), "");
#endif
        defaultLexersPath.AppendDir("lexers");
        CL_DEBUG("Loading default lexer files...");
        wxDir::GetAllFiles(defaultLexersPath.GetPath(), &files, "lexer_*.xml");
        // For performance reasons, read each file and store it into wxString
        for(size_t i = 0; i < files.GetCount(); ++i) {
            wxString content;
            wxFFile xmlFile(files.Item(i), "rb");
            if(!xmlFile.IsOpened()) continue;
            if(xmlFile.ReadAll(&content, wxConvUTF8)) {
                wxXmlDocument* doc = new wxXmlDocument();
                wxStringInputStream sis(content);
                if(doc->Load(sis)) {
                    defaultLexers.push_back(doc);
                } else {
                    wxDELETE(doc);
                }
            }
        }
        CL_DEBUG("Loading default lexer files...done");

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
        }
        CL_DEBUG("Loading users lexers...done");
        m_manager->CallAfter(&ColoursAndFontsManager::OnLexerFilesLoaded, defaultLexers, userLexers);
        return NULL;
    }
};

void ColoursAndFontsManager::Load()
{
    if(m_initialized) return;
    m_lexersMap.clear();
    m_initialized = true;

    if(IsUpgradeNeeded()) {
        clCommandEvent event(wxEVT_UPGRADE_LEXERS_START);
        EventNotifier::Get()->AddPendingEvent(event);
    }

    // Load the global settings
    if(GetConfigFile().FileExists()) {
        JSONRoot root(GetConfigFile());
        if(root.isOk()) {
            m_globalBgColour = root.toElement().namedObject("m_globalBgColour").toColour(m_globalBgColour);
            m_globalFgColour = root.toElement().namedObject("m_globalFgColour").toColour(m_globalFgColour);
        }
    }

    // Load the lexers in the bg thread
    ColoursAndFontsManager_HelperThread* thr = new ColoursAndFontsManager_HelperThread(this);
    thr->Create();
    thr->Run();
}

void ColoursAndFontsManager::LoadNewXmls(const std::vector<wxXmlDocument*>& xmlFiles, bool userLexers)
{
    // Each XMl represents a single lexer
    for(size_t i = 0; i < xmlFiles.size(); ++i) {
        wxXmlDocument* doc = xmlFiles.at(i);
        DoAddLexer(doc->GetRoot());
    }

    if(IsUpgradeNeeded()) {
        // We tuned the colours, save them back to the file system
        Save(userLexers);
    }
}

void ColoursAndFontsManager::LoadOldXmls(const wxString& path)
{
    // Convert the old XML format to a per lexer format
    wxArrayString files;
    wxDir::GetAllFiles(path, &files, "lexers_*.xml");

    wxString activeTheme = EditorConfigST::Get()->GetString("LexerTheme", "Default");

    // Each XMl represents a single lexer
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxXmlDocument doc;
        if(!doc.Load(files.Item(i))) continue;

        wxXmlNode* lexers = doc.GetRoot();
        wxXmlNode* child = lexers->GetChildren();
        wxString themeName = XmlUtils::ReadString(lexers, "Theme", "Default");
        themeName = themeName.Capitalize();

        while(child) {
            if(child->GetName() == "Lexer") {
                // Assign theme to this lexer
                child->AddAttribute("Theme", themeName);
                child->AddAttribute("IsActive", themeName == activeTheme ? "Yes" : "No");
                DoAddLexer(child);
            }
            child = child->GetNext();
        }
    }

    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxLogNull nl;
        ::wxRenameFile(files.Item(i), files.Item(i) + ".back");
    }

    // Since we just created the lexers, store them (migrating from old format)
    Save();
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

    // Hack: fix Java lexer which is using the same
    // file extensions as C++...
    if(lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) {
        lexer->SetFileSpec("*.java");
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

    if(lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".html")) {
        lexer->SetFileSpec(lexer->GetFileSpec() + ";*.html;*.htm;*.xhtml");
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
    iter = std::find_if(
        m_allLexers.begin(), m_allLexers.end(), LexerConf::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
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

            if(!firstLexer) {
                firstLexer = lexers.at(i);
            }

            if(!defaultLexer && lexers.at(i)->GetThemeName() == "Default") {
                defaultLexer = lexers.at(i);
            }

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
        for(size_t i = 0; i < lexers.size(); ++i) {
            if(lexers.at(i)->GetThemeName() == theme) {
                return lexers.at(i);
            }
        }
        return m_defaultLexer;
    }
}

void ColoursAndFontsManager::Save(bool userLexers)
{
    // count the total lexers count
    int lexersCount(0);
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.begin();
    for(; iter != m_lexersMap.end(); ++iter) {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {
            ++lexersCount;
        }
    }

    if(IsUpgradeNeeded()) {
        clCommandEvent event(wxEVT_UPGRADE_LEXERS_START);
        event.SetInt(lexersCount);
        EventNotifier::Get()->ProcessEvent(event);
    }

    iter = m_lexersMap.begin();
    lexersCount = 0;
    for(; iter != m_lexersMap.end(); ++iter) {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {
            ++lexersCount;
            // report the progress
            if(IsUpgradeNeeded()) {
                clCommandEvent event(wxEVT_UPGRADE_LEXERS_PROGRESS);
                event.SetInt(lexersCount); // the progress range
                wxString label = userLexers ? _("Upgrading user theme: ") : _("Upgrading theme: ");
                event.SetString(wxString() << label << lexers.at(i)->GetThemeName() << ", " << lexers.at(i)->GetName());
                EventNotifier::Get()->ProcessEvent(event);
            }
            Save(lexers.at(i));
        }
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
    if(filename.IsEmpty()) return GetLexer("text");

    wxFileName fnFileName(filename);
    wxString fileNameLowercase = fnFileName.GetFullName();
    fileNameLowercase.MakeLower();

    LexerConf::Ptr_t defaultLexer(NULL);
    LexerConf::Ptr_t firstLexer(NULL);

    // Scan the list of lexers, locate the active lexer for it and return it
    ColoursAndFontsManager::Vec_t::const_iterator iter = m_allLexers.begin();
    for(; iter != m_allLexers.end(); ++iter) {
        wxString fileMask = (*iter)->GetFileSpec().Lower();
        wxArrayString masks = ::wxStringTokenize(fileMask, ";", wxTOKEN_STRTOK);
        for(size_t i = 0; i < masks.GetCount(); ++i) {
            if(::wxMatchWild(masks.Item(i), fileNameLowercase)) {
                if((*iter)->IsActive()) {
                    return *iter;

                } else if(!firstLexer) {
                    firstLexer = *iter;

                } else if(!defaultLexer && (*iter)->GetThemeName() == "Default") {
                    defaultLexer = *iter;
                }
            }
        }
    }

    // Try this:
    // Use the FileExtManager to get the file type by examinig its content
    LexerConf::Ptr_t lexerByContent; // Null by default
    FileExtManager::FileType fileType = FileExtManager::TypeOther;
    if(FileExtManager::AutoDetectByContent(filename, fileType) && fileType != FileExtManager::TypeOther) {
        switch(fileType) {
        case FileExtManager::TypeScript:
            lexerByContent = GetLexer("script");
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

void ColoursAndFontsManager::Save(LexerConf::Ptr_t lexer)
{
    wxXmlDocument doc;
    doc.SetRoot(lexer->ToXml());

    wxString filename;
    wxString themeName = lexer->GetThemeName().Lower();
    themeName.Replace(" ", "_");
    themeName.Replace("::", "_");
    themeName.Replace("(", "_");
    themeName.Replace(")", "_");
    themeName.Replace(":", "_");
    themeName.Replace(",", "_");
    themeName.Replace(".", "_");
    themeName.Replace(";", "_");

    filename << "lexer_" << lexer->GetName().Lower() << "_" << themeName << ".xml";
    wxFileName xmlFile(clStandardPaths::Get().GetUserDataDir(), filename);
    xmlFile.AppendDir("lexers");
    ::SaveXmlToFile(&doc, xmlFile.GetFullPath());
}

void ColoursAndFontsManager::SetActiveTheme(const wxString& lexerName, const wxString& themeName)
{
    wxArrayString themes = GetAvailableThemesForLexer(lexerName);
    for(size_t i = 0; i < themes.GetCount(); ++i) {
        LexerConf::Ptr_t lexer = GetLexer(lexerName, themes.Item(i));
        if(lexer && lexer->GetName() == lexerName) {
            lexer->SetIsActive(lexer->GetThemeName() == themeName);
            Save(lexer);
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
    JSONRoot root(cJSON_Object);
    root.toElement().addProperty("m_globalBgColour", m_globalBgColour).addProperty("m_globalFgColour",
                                                                                   m_globalFgColour);
    wxFileName fnSettings = GetConfigFile();
    root.save(fnSettings.GetFullPath());

    wxCommandEvent evtThemeChanged(wxEVT_CL_THEME_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evtThemeChanged);
}

LexerConf::Ptr_t
ColoursAndFontsManager::CopyTheme(const wxString& lexerName, const wxString& themeName, const wxString& sourceTheme)
{
    LexerConf::Ptr_t sourceLexer = GetLexer(lexerName, sourceTheme);
    CHECK_PTR_RET_NULL(sourceLexer);

    wxXmlNode* sourceLexerXml = sourceLexer->ToXml();
    LexerConf::Ptr_t newLexer(new LexerConf());
    newLexer->FromXml(sourceLexerXml);

    // Update the theme name
    newLexer->SetThemeName(themeName);

    // Add it
    return DoAddLexer(newLexer->ToXml());
}

void ColoursAndFontsManager::RestoreDefaults()
{
    wxArrayString files;
    wxDir::GetAllFiles(clStandardPaths::Get().GetUserLexersDir(), &files, "lexer_*.xml");

    // First we delete the user settings
    {
        wxLogNull noLog;
        for(size_t i = 0; i < files.GetCount(); ++i) {
            ::wxRemoveFile(files.Item(i));
        }
    }

    // Now, we simply reload the settings
    Reload();
}

bool ColoursAndFontsManager::ImportEclipseTheme(const wxString& eclipseXml)
{
    bool res = false;
    if(!eclipseXml.IsEmpty()) {
        EclipseThemeImporterManager importer;
        res = importer.Import(eclipseXml);
        if(res) {
            Reload();
        }
    }
    return res;
}

void ColoursAndFontsManager::OnLexerFilesLoaded(const std::vector<wxXmlDocument*>& defaultLexers,
                                                const std::vector<wxXmlDocument*>& userLexers)
{
// Always load first the installation lexers
#ifdef USE_POSIX_LAYOUT
    wxFileName defaultLexersPath(clStandardPaths::Get().GetDataDir() + wxT(INSTALL_DIR), "");
#else
    wxFileName defaultLexersPath(clStandardPaths::Get().GetDataDir(), "");
#endif
    defaultLexersPath.AppendDir("lexers");
    LoadNewXmls(defaultLexers);

    //+++----------------------------------
    // Now handle user lexers
    //+++----------------------------------

    // Check to see if we have the new configuration files format (we break them from the unified XML file)
    // The naming convention used is:
    // lexer_<language>_<theme-name>.xml
    wxFileName cppLexerDefault(clStandardPaths::Get().GetUserDataDir(), "lexers_default.xml");
    cppLexerDefault.AppendDir("lexers");

    if(cppLexerDefault.FileExists()) {
        // We found the old lexer style here (single XML file for all the lexers)
        // Merge them with the installation lexers
        CL_DEBUG("Migrating old lexers XML files ...");
        LoadOldXmls(cppLexerDefault.GetPath());

    } else {
        // search for the new format in the user data folder
        cppLexerDefault.SetName("lexer_c++_default");
        if(cppLexerDefault.FileExists()) {
            CL_DEBUG("Using user lexer XML files from %s ...", cppLexerDefault.GetPath());
            // this function loads and delete the old xml files so they won't be located
            // next time we search for them
            LoadNewXmls(userLexers, true);
        }
    }

    if(IsUpgradeNeeded()) {
        // The upgrade has now completed; so tell everyone, and set the new version
        clConfig::Get().Write(LEXERS_VERSION_STRING, LEXERS_VERSION);
        clCommandEvent event(wxEVT_UPGRADE_LEXERS_END);
        EventNotifier::Get()->AddPendingEvent(event);

        m_lexersVersion = LEXERS_VERSION;
    }

    // Notify about colours and fonts configuration loaded
    clColourEvent event(wxEVT_COLOURS_AND_FONTS_LOADED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void ColoursAndFontsManager::UpdateLexerColours(LexerConf::Ptr_t lexer, bool force)
{
    StyleProperty& defaultProp = lexer->GetProperty(0); // Default
    if(force || m_lexersVersion < 1) {
        // adjust line numbers
        if(lexer->IsDark()) {
            StyleProperty& lineNumbers = lexer->GetProperty(LINE_NUMBERS_ATTR_ID); // Line numbers
            if(!defaultProp.IsNull()) {
                if(lexer->GetName() != "php" && lexer->GetName() != "html" && lexer->GetName() != "text" &&
                   lexer->GetName() != "cmake") {
                    // don't adjust PHP, HTML and Text default colours, since they also affects the various operators
                    // foreground colours
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
               lexer->GetName() != "cmake") {
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
            fold.SetFgColour(wxColour(defaultProp.GetBgColour()).ChangeLightness(105).GetAsString(wxC2S_HTML_SYNTAX));
            fold.SetBgColour(wxColour(defaultProp.GetBgColour()).ChangeLightness(105).GetAsString(wxC2S_HTML_SYNTAX));

            whitespace.SetFgColour(
                wxColour(defaultProp.GetBgColour()).ChangeLightness(105).GetAsString(wxC2S_HTML_SYNTAX));

        } else {
            fold.SetFgColour(wxColour(defaultProp.GetBgColour()).ChangeLightness(97).GetAsString(wxC2S_HTML_SYNTAX));
            fold.SetBgColour(wxColour(defaultProp.GetBgColour()).ChangeLightness(97).GetAsString(wxC2S_HTML_SYNTAX));

            whitespace.SetFgColour(
                wxColour(defaultProp.GetBgColour()).ChangeLightness(97).GetAsString(wxC2S_HTML_SYNTAX));
        }
    }
}
