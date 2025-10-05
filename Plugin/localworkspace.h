//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : localoptions.cpp
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

#ifndef __localoptions__
#define __localoptions__

#include "VirtualDirectoryColour.h"
#include "codelite_exports.h"
#include "optionsconfig.h"

#include <memory>
#include <optional>
#include <wx/filename.h>

// Denotes whether we're dealing with preferences at a global, workspace, project or (maybe one day) file level
enum prefsLevel { pLevel_global, pLevel_workspace, pLevel_project, pLevel_file, pLevel_dunno };

class LocalOptionsConfig;
using LocalOptionsConfigPtr = std::shared_ptr<LocalOptionsConfig>;

class WXDLLIMPEXP_SDK LocalOptionsConfig
{
    std::optional<bool> m_localdisplayFoldMargin;
    std::optional<bool> m_localdisplayBookmarkMargin;
    std::optional<bool> m_localhighlightCaretLine;
    std::optional<bool> m_localTrimLine;
    std::optional<bool> m_localAppendLF;
    std::optional<bool> m_localdisplayLineNumbers;
    std::optional<bool> m_localshowIndentationGuidelines;
    std::optional<bool> m_localindentUsesTabs;
    std::optional<int> m_localindentWidth;
    std::optional<int> m_localtabWidth;
    std::optional<wxFontEncoding> m_localfileFontEncoding;
    std::optional<int> m_localshowWhitspaces;
    std::optional<wxString> m_localeolMode;
    std::optional<bool> m_localTrackChanges;

public:
    LocalOptionsConfig(); // Used for setting local values
    LocalOptionsConfig(OptionsConfigPtr opts,
                       wxXmlNode* node); // Used for merging local values into the already-found global ones
    LocalOptionsConfig(LocalOptionsConfigPtr opts,
                       wxXmlNode* node); // Used for storing local values in a previously-empty instance
    virtual ~LocalOptionsConfig() = default;

    bool IsTrackChangesIsValid() const { return m_localTrackChanges.has_value(); }
    bool DisplayFoldMarginIsValid() const { return m_localdisplayFoldMargin.has_value(); }
    bool DisplayBookmarkMarginIsValid() const { return m_localdisplayBookmarkMargin.has_value(); }
    bool HighlightCaretLineIsValid() const { return m_localhighlightCaretLine.has_value(); }
    bool TrimLineIsValid() const { return m_localTrimLine.has_value(); }
    bool AppendLFIsValid() const { return m_localAppendLF.has_value(); }
    bool DisplayLineNumbersIsValid() const { return m_localdisplayLineNumbers.has_value(); }
    bool ShowIndentationGuidelinesIsValid() const { return m_localshowIndentationGuidelines.has_value(); }
    bool IndentUsesTabsIsValid() const { return m_localindentUsesTabs.has_value(); }
    bool IndentWidthIsValid() const { return m_localindentWidth.has_value(); }
    bool TabWidthIsValid() const { return m_localtabWidth.has_value(); }
    bool FileFontEncodingIsValid() const { return m_localfileFontEncoding.has_value(); }
    bool ShowWhitespacesIsValid() const { return m_localshowWhitspaces.has_value(); }
    bool EolModeIsValid() const { return m_localeolMode.has_value(); }

    //-------------------------------------
    // Setters/Getters
    //-------------------------------------

    bool IsTrackChanges() const
    {
        return m_localTrackChanges.value_or(false);
        // It's invalid anyway, so false will do as well as anything
    }

    bool GetDisplayFoldMargin() const { return m_localdisplayFoldMargin.value_or(false); }
    bool GetDisplayBookmarkMargin() const { return m_localdisplayBookmarkMargin.value_or(false); }
    bool GetHighlightCaretLine() const { return m_localhighlightCaretLine.value_or(false); }
    bool GetTrimLine() const { return m_localTrimLine.value_or(false); }
    bool GetAppendLF() const { return m_localAppendLF.value_or(false); }
    bool GetDisplayLineNumbers() const { return m_localdisplayLineNumbers.value_or(false); }
    bool GetShowIndentationGuidelines() const { return m_localshowIndentationGuidelines.value_or(false); }
    bool GetIndentUsesTabs() const { return m_localindentUsesTabs.value_or(false); }

    void SetTrackChanges(bool b) { m_localTrackChanges = b; }
    void SetDisplayFoldMargin(bool b) { m_localdisplayFoldMargin = b; }
    void SetDisplayBookmarkMargin(bool b) { m_localdisplayBookmarkMargin = b; }
    void SetHighlightCaretLine(bool b) { m_localhighlightCaretLine = b; }
    void SetTrimLine(bool b) { m_localTrimLine = b; }
    void SetAppendLF(bool b) { m_localAppendLF = b; }
    void SetDisplayLineNumbers(bool b) { m_localdisplayLineNumbers = b; }
    void SetShowIndentationGuidelines(bool b) { m_localshowIndentationGuidelines = b; }
    void SetIndentUsesTabs(bool indentUsesTabs) { m_localindentUsesTabs = indentUsesTabs; }

    void SetIndentWidth(int indentWidth) { m_localindentWidth = indentWidth; }
    int GetIndentWidth() const { return m_localindentWidth.value_or(wxNOT_FOUND); }
    void SetTabWidth(int tabWidth) { m_localtabWidth = tabWidth; }
    int GetTabWidth() const { return m_localtabWidth.value_or(wxNOT_FOUND); }

    wxFontEncoding GetFileFontEncoding() const
    {
        return m_localfileFontEncoding.value_or(static_cast<wxFontEncoding>(wxFONTENCODING_MAX + 1));
    }
    void SetFileFontEncoding(const wxString& strFileFontEncoding);

    void SetShowWhitespaces(int showWhitespaces) { m_localshowWhitspaces = showWhitespaces; }
    int GetShowWhitespaces() const { return m_localshowWhitspaces.value_or(wxNOT_FOUND); }

    void SetEolMode(const wxString& eolMode) { m_localeolMode = eolMode; }
    wxString GetEolMode() const { return m_localeolMode.value_or(wxT("")); }

    /**
     * Return an XML representation of this object
     * \return XML node
     */
    wxXmlNode* ToXml(wxXmlNode* parent = NULL, const wxString& nodename = wxT("Options")) const;
};

class WXDLLIMPEXP_SDK LocalWorkspace
{
public:
    enum CC_FLAGS {
        EnableCpp11 = 0x00000001,
        EnableCpp14 = 0x00000002,
        EnableCpp17 = 0x00000004,
        EnableSWTLW = 0x00000008, // Save Parse folders to Workspace file.
        EnableCpp20 = 0x00000010,
    };

private:
    friend class LocalWorkspaceST;
    wxXmlDocument m_doc;
    wxFileName m_fileName;

protected:
    /*void SaveWorkspaceOptions(LocalOptionsConfigPtr opts);
    void SaveProjectOptions(LocalOptionsConfigPtr opts, const wxString& projectname);*/
    bool SanityCheck();
    bool Create();
    bool SaveXmlFile();
    wxFileName DoGetFilePath() const;

public:
    /// Constructor
    LocalWorkspace() = default;

    /// Destructor
    virtual ~LocalWorkspace() = default;

    /**
     * @brief return list of pinned c++ projects
     */
    size_t GetPinnedProjects(wxArrayString& projects);
    bool SetPinnedProjects(const wxArrayString& projects);

    /**
     * @brief flush the XML content to the disk
     */
    void Flush() { SaveXmlFile(); }

    /**
     * @brief Get any local editor preferences, merging the values into the global options
     * \param options the global options
     * \param projectname the name of the currently active project
     */
    void GetOptions(OptionsConfigPtr options, const wxString& projectname);
    /**
     * @brief Sets any local editor preferences for the current workspace
     * \param opts the local options to save
     */
    bool SetWorkspaceOptions(LocalOptionsConfigPtr opts);
    /**
     * @brief Sets any local editor preferences for the named project
     * \param opts the local options to save
     * \param projectname the name of the project
     */
    bool SetProjectOptions(LocalOptionsConfigPtr opts, const wxString& projectname);

    /**
     * @brief Set virtual folder colours for the workspace
     */
    bool SetFolderColours(const FolderColour::Map_t& vdColours);

    /**
     * @brief Get virtual folder colours for the workspace
     */
    bool GetFolderColours(FolderColour::Map_t& vdColours);

    /**
     * @brief Returns the node where any current local workspace options are stored
     */
    wxXmlNode* GetLocalWorkspaceOptionsNode() const;
    /**
     * @brief Returns the node where any current local project options are stored
     * \param projectname the name of the project
     */
    wxXmlNode* GetLocalProjectOptionsNode(const wxString& projectname) const;

    /**
     * @brief return the workspace C++ parser specific include + exclude paths
     * @param inclduePaths [output]
     * @param excludePaths [output]
     */
    void GetParserPaths(wxArrayString& inclduePaths, wxArrayString& excludePaths);
    void SetParserPaths(const wxArrayString& inclduePaths, const wxArrayString& excludePaths);

    size_t GetParserFlags();
    void SetParserFlags(size_t flags);

    void GetParserMacros(wxString& macros);
    void SetParserMacros(const wxString& macros);

    void SetSearchInFilesMask(const wxString& findInFileMask);
    void GetSearchInFilesMask(wxString& findInFileMask, const wxString& defaultValue = "");

    void SetCustomData(const wxString& name, const wxString& value);
    wxString GetCustomData(const wxString& name);

    /**
     * @brief set and get the currently selected build configuration name
     */
    void SetSelectedBuildConfiguration(const wxString& confName);
    wxString GetSelectedBuildConfiguration();

    /**
     * @brief set and get the active environment variables set name
     */
    void SetActiveEnvironmentSet(const wxString& setName);
    wxString GetActiveEnvironmentSet();
};

#endif // __localoptions__
