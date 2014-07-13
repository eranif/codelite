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

class clCommandEvent;
ColoursAndFontsManager::ColoursAndFontsManager()
    : m_initialized(false)
{
    m_globalBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    m_globalFgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
}

ColoursAndFontsManager::~ColoursAndFontsManager() {}

ColoursAndFontsManager& ColoursAndFontsManager::Get()
{
    static ColoursAndFontsManager s_theManager;
    return s_theManager;
}

void ColoursAndFontsManager::Load()
{
    if(m_initialized)
        return;

    m_lexersMap.clear();
    m_initialized = true;

    // Check to see if we have the new configuration files format (we break them from the unified XML file)
    // The naming convention used is:
    // lexer_<language>_<theme-name>.xml
    wxFileName cppLexerDefault(clStandardPaths::Get().GetUserDataDir(), "lexers_default.xml");
    cppLexerDefault.AppendDir("lexers");

    if(cppLexerDefault.FileExists()) {
        LoadOldXmls(cppLexerDefault.GetPath());

    } else {
        // search for the new format in the user data folder
        cppLexerDefault.SetName("lexer_c++_default");
        if(cppLexerDefault.FileExists()) {
            // this function loads and delete the old xml files so they won't be located
            // next time we search for them
            LoadNewXmls(cppLexerDefault.GetPath());

        } else {
            // load them from the installation folder
            cppLexerDefault.SetPath(clStandardPaths::Get().GetDataDir());
            cppLexerDefault.AppendDir("lexers");
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
    if(activeTheme.IsEmpty())
        activeTheme = "Default";

    // Each XMl represents a single lexer
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxXmlDocument doc;
        if(!doc.Load(files.Item(i)))
            continue;

        wxXmlNode* lexers = doc.GetRoot();
        wxXmlNode* child = lexers->GetChildren();
        wxString themeName = XmlUtils::ReadString(lexers, "Theme", "Default");
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

void ColoursAndFontsManager::DoAddLexer(wxXmlNode* node)
{
    wxString lexerName = XmlUtils::ReadString(node, "Name");
    lexerName.MakeLower();
    if(lexerName.IsEmpty())
        return;

    LexerConfPtr lexer(new LexerConf);
    lexer->FromXml(node);

    if(m_lexersMap.count(lexerName) == 0) {
        m_lexersMap.insert(std::make_pair(lexerName, ColoursAndFontsManager::Vec_t()));
    }
    m_lexersMap.find(lexerName)->second.push_back(lexer);
    m_allLexers.push_back(lexer);
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
    return themes;
}

LexerConfPtr ColoursAndFontsManager::GetLexer(const wxString& lexerName, const wxString& theme) const
{
    ColoursAndFontsManager::Map_t::const_iterator iter = m_lexersMap.find(lexerName.Lower());
    if(iter == m_lexersMap.end())
        return NULL;

    // Locate the requested theme
    LexerConfPtr firstLexer(NULL);
    LexerConfPtr defaultLexer(NULL);

    if(theme.IsEmpty()) {
        // return the active theme
        const ColoursAndFontsManager::Vec_t& lexers = iter->second;
        for(size_t i = 0; i < lexers.size(); ++i) {

            if(!firstLexer) {
                firstLexer = lexers.at(i);
            }

            if(!defaultLexer && lexers.at(i)->GetName() == "Default") {
                defaultLexer = lexers.at(i);
            }

            if(lexers.at(i)->IsActive())
                return lexers.at(i);
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
        LexerConfPtr lexer = m_allLexers.at(i);
        if(names.Index(lexer->GetName()) == wxNOT_FOUND) {
            names.Add(lexer->GetName());
        }
    }
    return names;
}

LexerConfPtr ColoursAndFontsManager::GetLexerForFile(const wxString& filename) const
{
    wxFileName fnFileName(filename);
    wxString fileNameLowercase = fnFileName.GetFullName();
    fileNameLowercase.MakeLower();

    // Scan the list of lexers, locate the active lexer for it and return it
    ColoursAndFontsManager::Vec_t::const_iterator iter = m_allLexers.begin();
    for(; iter != m_allLexers.end(); ++iter) {
        if(!(*iter)->IsActive()) {
            continue;
        }
        wxString fileMask = (*iter)->GetFileSpec().Lower();
        wxArrayString masks = ::wxStringTokenize(fileMask, ";", wxTOKEN_STRTOK);
        for(size_t i = 0; i < masks.GetCount(); ++i) {
            if(::wxMatchWild(masks.Item(i), fileNameLowercase)) {
                return *iter;
            }
        }
    }
    // Return the "Text" lexer
    return GetLexer("text");
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
}

void ColoursAndFontsManager::Save(LexerConfPtr lexer)
{
    wxXmlDocument doc;
    doc.SetRoot(lexer->ToXml());

    wxString filename;
    filename << "lexer_" << lexer->GetName().Lower() << "_" << lexer->GetThemeName().Lower() << ".xml";
    wxFileName xmlFile(clStandardPaths::Get().GetUserDataDir(), filename);
    xmlFile.AppendDir("lexers");
    ::SaveXmlToFile(&doc, xmlFile.GetFullPath());
}

void ColoursAndFontsManager::SetActiveTheme(const wxString& lexerName, const wxString& themeName)
{
    wxArrayString themes = GetAvailableThemesForLexer(lexerName);
    for(size_t i = 0; i < themes.GetCount(); ++i) {
        LexerConfPtr lexer = GetLexer(lexerName, themes.Item(i));
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
