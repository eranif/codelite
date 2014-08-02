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
#include "EclipseCXXThemeImporter.h"

class clCommandEvent;
ColoursAndFontsManager::ColoursAndFontsManager()
    : m_initialized(false)
{
    m_globalBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    m_globalFgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
}

ColoursAndFontsManager::~ColoursAndFontsManager()
{
}

ColoursAndFontsManager& ColoursAndFontsManager::Get()
{
    static ColoursAndFontsManager s_theManager;
    return s_theManager;
}

void ColoursAndFontsManager::Load()
{
    if(m_initialized) return;

    m_lexersMap.clear();
    m_initialized = true;

    // Always load first the installation lexers
    wxFileName defaultLexersPath(clStandardPaths::Get().GetDataDir(), "");
    defaultLexersPath.AppendDir("lexers");
    LoadNewXmls(defaultLexersPath.GetPath());

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
            LoadNewXmls(cppLexerDefault.GetPath());
        }
    }

    // Load the global settings
    if(GetConfigFile().FileExists()) {
        JSONRoot root(GetConfigFile());
        if(root.isOk()) {
            m_globalBgColour = root.toElement().namedObject("m_globalBgColour").toColour(m_globalBgColour);
            m_globalFgColour = root.toElement().namedObject("m_globalFgColour").toColour(m_globalFgColour);
        }
    }
}

void ColoursAndFontsManager::LoadNewXmls(const wxString& path)
{
    // load all XML files
    wxArrayString files;
    wxDir::GetAllFiles(path, &files, "lexer_*.xml");

    // Each XMl represents a single lexer
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxXmlDocument doc;
        if(!doc.Load(files.Item(i))) {
            continue;
        }
        DoAddLexer(doc.GetRoot());
    }
}

void ColoursAndFontsManager::LoadOldXmls(const wxString& path)
{
    // Convert the old XML format to a per lexer format
    wxArrayString files;
    wxDir::GetAllFiles(path, &files, "lexers_*.xml");

    wxString activeTheme = EditorConfigST::Get()->GetStringValue("LexerTheme");
    if(activeTheme.IsEmpty()) activeTheme = "Default";

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
    lexer->SetThemeName( themeName );
    
    // Hack: fix Java lexer which is using the same
    // file extensions as C++...
    if(lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) {
        lexer->SetFileSpec("*.java");
    }

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
    if(iter == m_lexersMap.end()) return NULL;

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
            return NULL;

    } else {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {
            if(lexers.at(i)->GetThemeName() == theme) {
                return lexers.at(i);
            }
        }
        return NULL;
    }
}

void ColoursAndFontsManager::Save()
{
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.begin();
    for(; iter != m_lexersMap.end(); ++iter) {
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {
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

bool ColoursAndFontsManager::ImportEclipseTheme(const wxString& eclipseXml, wxString& outputFile)
{
    bool res = false;
    if(!eclipseXml.IsEmpty()) {
        EclipseCXXThemeImporter importer;
        res = importer.Import(eclipseXml, outputFile);
        if(res) {
            Reload();
        }
    }
    return res;
}
