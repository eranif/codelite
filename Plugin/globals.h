//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : globals.h
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
#ifndef GLOBALS_H
#define GLOBALS_H

#include <wx/string.h>
#include <wx/colour.h>
#include <wx/arrstr.h>
#include "codelite_exports.h"

#include "workspace.h"

class wxListCtrl;
class IEditor;
class IManager;

/**
 * \brief send command event to the application (wxTheApp),
 * \param eventId
 * \param clientData any user data. Must NOT be freed by the handler
 * \return same as wxTheApp->ProcessEvent()
 */
WXDLLIMPEXP_SDK bool SendCmdEvent(int eventId, void *clientData = NULL);

/**
 * @brief send command event to the application (wxTheApp),
 * @param eventId
 * @param clientData any user data. Must NOT be freed by the handler
 * @param str this string will be available by the handlers by calling event.GetString()
 * @return same as wxTheApp->ProcessEvent()
 */
WXDLLIMPEXP_SDK bool SendCmdEvent(int eventId, void *clientData, const wxString &str);

/**
 * \brief post command event to the application (wxTheApp),
 * \param eventId
 * \param clientData allocated data on the heap. Must be freed by the handler
 */
WXDLLIMPEXP_SDK void PostCmdEvent(int eventId, void *clientData = NULL);

/**
 * \brief set column text
 * \param list the list
 * \param indx row ID
 * \param column column ID
 * \param rText the text
 * \param imgId image id
 */
WXDLLIMPEXP_SDK void SetColumnText (wxListCtrl *list, long indx, long column, const wxString &rText, int imgId = wxNOT_FOUND );

/**
 * \brief return column's text
 * \param list the list control
 * \param index the row ID
 * \param column the column ID
 * \return the column's text
 */
WXDLLIMPEXP_SDK wxString GetColumnText(wxListCtrl *list, long index, long column);

/**
 * \brief append row to list control
 * \param list the list
 * \return new row index
 */
WXDLLIMPEXP_SDK long AppendListCtrlRow(wxListCtrl *list);

/**
 * \brief read file from disk using appropriate file conversion
 * \param fileName file name
 * \param content output string
 * \param encoding
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool ReadFileWithConversion(const wxString &fileName, wxString &content, wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

/**
 * \brief write file using UTF8 converter
 * \param fileName file path
 * \param content file's conent
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool WriteFileUTF8(const wxString &fileName, const wxString &content);

/**
 * \brief delete directory using shell command
 * \param path directory path
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool RemoveDirectory(const wxString &path);

/**
 * \brief return true of id is a valid cpp identifier
 */
WXDLLIMPEXP_SDK bool IsValidCppIndetifier(const wxString &id);

/**
 * \brief return true of word is a C++ keyword
 * \param word
 * \return
 */
WXDLLIMPEXP_SDK bool IsCppKeyword(const wxString &word);

/**
 * \brief return true of id is a valid cpp file
 */
WXDLLIMPEXP_SDK bool IsValidCppFile(const wxString &id);

/**
 * [DEPRECATED] DONT USE THIS METHOD ANYMORE - USE IMacroManager
 * Expand variables to their real value, if expanding fails
 * the return value is same as input. The variable is expanded
 * in the project context
 */
WXDLLIMPEXP_SDK wxString ExpandVariables(const wxString &expression, ProjectPtr proj, IEditor *editor);

/**
 * * [DEPRECATED] DONT USE THIS METHOD ANYMORE - USE IMacroManager
 * \brief accepts expression string and expand all known marcos (e.g. $(ProjectName))
 * \param expression expression
 * \param projectName project name (to be used for $(ProjectName) macro)
 * \param fileName file name, to help expand the $(CurrentFile) macro family
 * \return an expanded string. If a macro is unknown it is replaced by empty string
 */
WXDLLIMPEXP_SDK wxString ExpandAllVariables(const wxString &expression, Workspace *workspace, const wxString &projectName, const wxString &selConf, const wxString &fileName);

/**
 * \brief copy entire directory content (recursievly) from source to target
 * \param src source path
 * \param target target path
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool CopyDir(const wxString& src, const wxString& target);

/**
 * \brief create a directory
 * \param path directory path
 */
WXDLLIMPEXP_SDK void Mkdir(const wxString &path);

/**
 * \brief write file content with optinal backup
 * \param file_name
 * \param content
 * \param backup
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool WriteFileWithBackup(const wxString &file_name, const wxString &content, bool backup);

/**
 * \brief copy text to the clipboard
 * \return true on success false otherwise
 */
WXDLLIMPEXP_SDK bool CopyToClipboard(const wxString &text);

/**
 * \brief make colour lighter
 * \param color
 * \param level
 * \return modified colour
 */
WXDLLIMPEXP_SDK wxColour MakeColourLighter(wxColour color, float level);

/**
 * @brief return true if filename is readonly false otherwise
 */
WXDLLIMPEXP_SDK bool IsFileReadOnly(const wxFileName &filename);

/**
 * \brief fill an array with a semi-colon separated string
 * \param arr [out] the array to fill
 * \param str the string to split
 */
WXDLLIMPEXP_SDK void FillFromSmiColonString(wxArrayString &arr, const wxString &str);

/**
 * \brief return a string semi-colon separated of the given array
 */
WXDLLIMPEXP_SDK wxString ArrayToSmiColonString(const wxArrayString &array);

/**
 * \brief Remove all semi colons of the given string
 */
WXDLLIMPEXP_SDK void StripSemiColons(wxString &str);

/**
 * \brief Normalize the given path (change all \ by /)
 */
WXDLLIMPEXP_SDK wxString NormalizePath(const wxString &path);

/**
 * \brief Returns the file modification time in seconds after the epoch.
 */
WXDLLIMPEXP_SDK time_t GetFileModificationTime(const wxString &filename);
WXDLLIMPEXP_SDK time_t GetFileModificationTime(const wxFileName &filename);

/**
 * @brief wrap a given command in the shell command (e.g. cmd /c "command")
 */
WXDLLIMPEXP_SDK void WrapInShell(wxString &cmd);

/**
 * @brief return the current user name without any special characters
 * @return
 */
WXDLLIMPEXP_SDK wxString clGetUserName();

/**
 * @brief return list of projects available based on the installed tempaltes
 * @param list list of projects
 * @param imageMap when provided, returns the image index (set in the lstImages) mapped to the project type
 * @param lstImages wxImageList allocated on the heap for the projects
 */
WXDLLIMPEXP_SDK void GetProjectTemplateList( IManager *manager, std::list<ProjectPtr> &list, std::map<wxString,int> *imageMap = NULL, wxImageList **lstImages = NULL);

/**
 * @brief extract file from Zip and place it under targetDir
 * @param zipPath path to the Zip file (fullpath)
 * @param filename the file name to search in the archive
 * @param targetDir where to place the extracted file
 * @param targetFileName the path of the file that was actually extracted
 * @return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool ExtractFileFromZip(const wxString &zipPath, const wxString& filename, const wxString &targetDir, wxString &targetFileName);

#endif //GLOBALS_H

