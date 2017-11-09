//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : ColoursAndFontsManager.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef LEXERCONFMANAGER_H
#define LEXERCONFMANAGER_H

#include "codelite_exports.h"
#include "lexer_configuration.h"
#include <vector>
#include <map>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/font.h>
#include "wxStringHash.h"

// When the version is 0, it means that we need to upgrade the colours for the line numbers
// and for the default state
#define LEXERS_UPGRADE_LINENUM_DEFAULT_COLOURS 0

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_UPGRADE_LEXERS_START, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_UPGRADE_LEXERS_END, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_UPGRADE_LEXERS_PROGRESS, clCommandEvent);

class WXDLLIMPEXP_SDK ColoursAndFontsManager : public wxEvtHandler
{
    typedef std::vector<LexerConf::Ptr_t> Vec_t;
    typedef std::unordered_map<wxString, ColoursAndFontsManager::Vec_t> Map_t;

protected:
    bool m_initialized;
    ColoursAndFontsManager::Map_t m_lexersMap;
    ColoursAndFontsManager::Vec_t m_allLexers;
    wxColour m_globalBgColour;
    wxColour m_globalFgColour;
    wxString m_globalTheme;
    LexerConf::Ptr_t m_defaultLexer;
    int m_lexersVersion;
    wxFont m_globalFont;

private:
    ColoursAndFontsManager();
    virtual ~ColoursAndFontsManager();

    void LoadOldXmls(const std::vector<wxXmlDocument*>& xmlFiles, bool userLexers = false);
    LexerConf::Ptr_t DoAddLexer(wxXmlNode* node);
    LexerConf::Ptr_t DoAddLexer(JSONElement json);
    void Clear();
    wxFileName GetConfigFile() const;
    void LoadJSON(const wxFileName& path);

public:
    static ColoursAndFontsManager& Get();
    /**
     * @brief Export themes to JSON file
     */
    bool ExportThemesToFile(const wxFileName& outputFile, const wxArrayString& names = wxArrayString()) const;
    
    /**
     * @brief import lexers from configuration file
     */
    bool ImportLexersFile(const wxFileName& inputFile, bool prompt = true);
    
    /**
     * @brief save the global settings
     */
    void SaveGlobalSettings();
    /**
     * @brief adjust the lexer colours to fit codelite's general look and feel
     */
    void UpdateLexerColours(LexerConf::Ptr_t lexer, bool force);

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

    void SetGlobalFont(const wxFont& font);
    const wxFont& GetGlobalFont() const;

    void SetGlobalFgColour(const wxColour& globalFgColour)
    {
        this->m_globalFgColour = globalFgColour;
        SaveGlobalSettings();
    }
    void SetGlobalTheme(const wxString& globalTheme) { this->m_globalTheme = globalTheme; }
    const wxString& GetGlobalTheme() const { return m_globalTheme; }
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
     * @brief return list of all themes
     */
    wxArrayString GetAllThemes() const;

    /**
     * @brief return lexer for a file
     */
    LexerConf::Ptr_t GetLexerForFile(const wxString& filename) const;

    /**
     * @brief restore the default colours
     * This is done by deleting the user defined XML files and
     */
    void RestoreDefaults();

    /**
     * @brief import an eclipse theme into codelite
     */
    bool ImportEclipseTheme(const wxString& eclipseXml);

    /**
     * @brief callback called by the helper thread indicating that it finished caching
     * the XML files
     */
    void OnLexerFilesLoaded(const std::vector<wxXmlDocument*>& userLexers);

    /**
     * @brief set a unified theme for all lexers. If the requested theme is not available for a given lexer,
     * use the closest one
     * @param themeName
     */
    void SetTheme(const wxString& themeName);

    /**
     * @brief add new lexer (replace an existing one if exists)
     */
    void AddLexer(LexerConf::Ptr_t lexer);
};

#endif // LEXERCONFMANAGER_H
