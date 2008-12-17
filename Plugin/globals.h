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

#include "wx/string.h"
#include <wx/colour.h>
#include "wx/arrstr.h"
class wxListCtrl;
class Workspace;

/**
 * \brief send command event to the application (wxTheApp),
 * \param eventId
 * \param clientData allocated data on the heap. Must be freed by the handler
 */
void SendCmdEvent(int eventId, void *clientData = NULL);

/**
 * \brief post command event to the application (wxTheApp),
 * \param eventId
 * \param clientData allocated data on the heap. Must be freed by the handler
 */
void PostCmdEvent(int eventId, void *clientData = NULL);

/**
 * \brief set column text 
 * \param list the list
 * \param indx row ID
 * \param column column ID
 * \param rText the text
 * \param imgId image id
 */
void SetColumnText (wxListCtrl *list, long indx, long column, const wxString &rText, int imgId = wxNOT_FOUND );

/**
 * \brief return column's text
 * \param list the list control
 * \param index the row ID
 * \param column the column ID
 * \return the column's text
 */
wxString GetColumnText(wxListCtrl *list, long index, long column);

/**
 * \brief append row to list control
 * \param list the list
 * \return new row index
 */
long AppendListCtrlRow(wxListCtrl *list);

/**
 * \brief read file from disk using appropriate file conversion 
 * \param fileName file name
 * \param content output string
 * \return true on success, false otherwise
 */
bool ReadFileWithConversion(const wxString &fileName, wxString &content);

/**
 * \brief write file using UTF8 converter
 * \param fileName file path 
 * \param content file's conent
 * \return true on success, false otherwise
 */
bool WriteFileUTF8(const wxString &fileName, const wxString &content);

/**
 * \brief delete directory using shell command
 * \param path directory path
 * \return true on success, false otherwise
 */
bool RemoveDirectory(const wxString &path);

/**
 * \brief return true of id is a valid cpp identifier
 */
bool IsValidCppIndetifier(const wxString &id);

/**
 * \brief return true of id is a valid cpp file 
 */ 
bool IsValidCppFile(const wxString &id);

/**
 * \brief accepts expression string and expand all known marcos (e.g. $(ProjectName))
 * \param expression expression 
 * \param projectName project name (to be used for $(ProjectName) macro)
 * \param fileName file name, to help expand the $(CurrentFile) macro family
 * \return an expanded string. If a macro is unknown it is replaced by empty string
 */
wxString ExpandAllVariables(const wxString &expression, Workspace *workspace, const wxString &projectName, const wxString &selConf, const wxString &fileName);

/**
 * \brief copy entire directory content (recursievly) from source to target
 * \param src source path
 * \param target target path
 * \return true on success, false otherwise
 */
bool CopyDir(const wxString& src, const wxString& target);

/**
 * \brief create a directory
 * \param path directory path
 */
void Mkdir(const wxString &path);

/**
 * \brief write file content with optinal backup
 * \param file_name
 * \param content
 * \param backup
 * \return true on success, false otherwise
 */
bool WriteFileWithBackup(const wxString &file_name, const wxString &content, bool backup);

/**
 * \brief copy text to the clipboard
 * \return true on success false otherwise
 */
bool CopyToClipboard(const wxString &text);

/**
 * \brief make colour lighter
 * \param color
 * \param level
 * \return modified colour
 */
wxColour MakeColourLighter(wxColour color, float level);

#endif //GLOBALS_H

