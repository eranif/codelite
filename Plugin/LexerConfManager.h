#ifndef LEXERCONFMANAGER_H
#define LEXERCONFMANAGER_H

#include "codelite_exports.h"
#include "lexer_configuration.h"
#include <vector>
#include <map>
#include <wx/string.h>

class WXDLLIMPEXP_SDK LexerConfManager
{
    typedef std::vector<LexerConfPtr> Vec_t;
    typedef std::map<wxString, LexerConfManager::Vec_t> Map_t;
    
protected:
    bool m_initialized;
    LexerConfManager::Map_t m_lexersMap;
    LexerConfManager::Vec_t m_allLexers;

private:
    LexerConfManager();
    virtual ~LexerConfManager();

    void LoadNewXmls(const wxString &path);
    void LoadOldXmls(const wxString &path);
    void DoAddLexer(wxXmlNode* node);
    void Clear();
    
public:
    static LexerConfManager& Get();

    /**
     * @brief reload the lexers from the configuration files
     */
    void Reload();
    
    /**
     * @brief load the lexers
     */
    void Load();
    
    /**
     * @brief save the lexers into their proper file name
     */
    void Save();

    /**
     * @brief save a single lexer
     */
    void Save(LexerConfPtr lexer);

    /**
     * @brief set the active theme for a lexer by name
     */
    void SetActiveTheme(const wxString &lexerName, const wxString &themeName);
    /**
     * @brief return the lexer by name.
     * @param lexerName the lexer name, e.g. "c++"
     * @param theme optionally, return the lexer of a given theme
     */
    LexerConfPtr GetLexer(const wxString &lexerName, const wxString &theme = wxEmptyString) const;

    /**
     * @brief return an array of themes availabel for a given lexer
     */
    wxArrayString GetAvailableThemesForLexer(const wxString &lexerName) const;

    /**
     * @brief return an array of available lexers
     */
    wxArrayString GetAllLexersNames() const;

    /**
     * @brief return lexer for a file
     */
    LexerConfPtr GetLexerForFile(const wxString& filename) const;
};

#endif // LEXERCONFMANAGER_H
