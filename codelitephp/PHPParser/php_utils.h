#ifndef PHP_UTILS_H
#define PHP_UTILS_H

#include <wx/wx.h>
#include <ieditor.h>

/**
 * @brief return true of the editor holds a file of type PHP
 */
bool IsPHPFile(IEditor* editor);
bool IsPHPSection(int styleAtPos );
bool IsPHPCommentOrString(int styleAtPos);

wxMemoryBuffer ReadFileContent(const wxString& filename);

wxString GetResourceDirectory();
wxString GetCCResourceDirectory();

#define FILE_SCHEME "file://"

/**
 * @brief convert file in the URI format: file:///path/to/file to -> /path/to/file
 */
wxString URIToFileName(const wxString &uriFileName);

/**
 * @brief convert filename to URI file format 
 */
wxString FileNameToURI(const wxString &filename);

/**
 * @brief encode a string into base64
 */
wxString Base64Encode(const wxString &str);
#endif // PHP_UTILS

