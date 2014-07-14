#ifndef LEXERCONFMANAGER_H
#define LEXERCONFMANAGER_H

#include "codelite_exports.h"
#include "lexer_configuration.h"
#include <vector>
#include <map>
#include <wx/string.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK ColoursAndFontsManager
{
    typedef std::vector<LexerConf::Ptr_t> Vec_t;
    typedef std::map<wxString, ColoursAndFontsManager::Vec_t> Map_t;

protected:
    bool m_initialized;
    ColoursAndFontsManager::Map_t m_lexersMap;
    ColoursAndFontsManager::Vec_t m_allLexers;
    wxColour m_globalBgColour;
    wxColour m_globalFgColour;

private:
    ColoursAndFontsManager();
    virtual ~ColoursAndFontsManager();

    void LoadNewXmls(const wxString& path);
    void LoadOldXmls(const wxString& path);
    LexerConf::Ptr_t DoAddLexer(wxXmlNode* node);
    void Clear();
    wxFileName GetConfigFile() const;
    void SaveGlobalSettings();

public:
    static ColoursAndFontsManager& Get();

    const wxColour& GetGlobalBgColour() const { return m_globalBgColour; }
    const wxColour& GetGlobalFgColour() const { return m_globalFgColour; }

    /**
     * @brief create new theme for a lexer by copying an existing theme 'sourceTheme'
     * @param lexerName the lexer name
     * @param themeName the new theme name
     * @param sourceTheme source theme to copy the attributes from
     */
    LexerConf::Ptr_t CopyTheme(const wxString& lexerName, const wxString& themeName, const wxString& sourceTheme);

    void SetGlobalBgColour(const wxColour& globalBgColour)
    {
        this->m_globalBgColour = globalBgColour;
        SaveGlobalSettings();
    }
    void SetGlobalFgColour(const wxColour& globalFgColour)
    {
        this->m_globalFgColour = globalFgColour;
        SaveGlobalSettings();
    }
    /**
     * @brief reload the lexers from the configuration files
     */
    void Reload();

    /**
     * @brief load the lexers + global settings
     */
    void Load();

    /**
     * @brief save the lexers into their proper file name
     */
    void Save();

    /**
     * @brief save a single lexer
     */
    void Save(LexerConf::Ptr_t lexer);

    /**
     * @brief set the active theme for a lexer by name
     */
    void SetActiveTheme(const wxString& lexerName, const wxString& themeName);
    /**
     * @brief return the lexer by name.
     * @param lexerName the lexer name, e.g. "c++"
     * @param theme optionally, return the lexer of a given theme
     */
    LexerConf::Ptr_t GetLexer(const wxString& lexerName, const wxString& theme = wxEmptyString) const;

    /**
     * @brief return an array of themes availabel for a given lexer
     */
    wxArrayString GetAvailableThemesForLexer(const wxString& lexerName) const;

    /**
     * @brief return an array of available lexers
     */
    wxArrayString GetAllLexersNames() const;

    /**
     * @brief return lexer for a file
     */
    LexerConf::Ptr_t GetLexerForFile(const wxString& filename) const;
};

#endif // LEXERCONFMANAGER_H
