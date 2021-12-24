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

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "lexer_configuration.h"
#include "wxStringHash.h"

#include <map>
#include <vector>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/font.h>
#include <wx/string.h>

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
    wxString m_globalTheme;
    LexerConf::Ptr_t m_defaultLexer;
    int m_lexersVersion;
    wxFont m_globalFont;

private:
    ColoursAndFontsManager();
    virtual ~ColoursAndFontsManager();

    void LoadOldXmls(const std::vector<wxXmlDocument*>& xmlFiles, bool userLexers = false);
    LexerConf::Ptr_t DEPRECATRED_DoAddLexer(wxXmlNode* node);
    LexerConf::Ptr_t DoAddLexer(JSONItem json);
    void Clear();
    wxFileName GetConfigFile() const;
    void LoadJSON(const wxFileName& path);

protected:
    void OnAdjustTheme(clCommandEvent& event);

public:
    static ColoursAndFontsManager& Get();

    /**
     * @brief return the default editor font (monospaced)
     */
    wxFont GetFixedFont(bool small = false) const;
    /**
     * @brief return a suitable background colour that matches the lexer's bg colour
     */
    wxColour GetBackgroundColourFromLexer(LexerConf::Ptr_t lexer) const;

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

    /**
     * @brief create new theme for a lexer by copying an existing theme 'sourceTheme'
     * @param lexerName the lexer name
     * @param themeName the new theme name
     * @param sourceTheme source theme to copy the attributes from
     */
    LexerConf::Ptr_t CopyTheme(const wxString& lexerName, const wxString& themeName, const wxString& sourceTheme);

    void SetGlobalFont(const wxFont& font);
    const wxFont& GetGlobalFont() const;

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
     * @param lexer_json optional output file
     */
    void Save(const wxFileName& lexer_json = {});

    /**
     * @brief set the active theme for a lexer by name
     */
    void SetActiveTheme(const wxString& lexerName, const wxString& themeName);

    /**
     * @brief update a theme text selection colours
     */
    void SetThemeTextSelectionColours(const wxString& theme_name, const wxColour& bg, const wxColour& fg,
                                      bool useCustomerFgColour = true);

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
     * @brief import an eclipse theme into CodeLite, return its name
     */
    wxString ImportEclipseTheme(const wxString& theme_file);

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

    /**
     * @brief return true if the current theme is dark
     */
    bool IsDarkTheme() const;
};

#endif // LEXERCONFMANAGER_H
