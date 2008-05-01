#ifndef GLOBALS_H
#define GLOBALS_H

#include "wx/string.h"
#include "wx/arrstr.h"
class wxListCtrl;

void SendCmdEvent(int eventId, void *clientData = NULL);
void PostCmdEvent(int eventId, void *clientData = NULL);
void SetColumnText (wxListCtrl *list, long indx, long column, const wxString &rText, int imgId = wxNOT_FOUND );
wxString GetColumnText(wxListCtrl *list, long index, long column);
long AppendListCtrlRow(wxListCtrl *list);
bool ReadFileWithConversion(const wxString &fileName, wxString &content);
bool WriteFileUTF8(const wxString &fileName, const wxString &content);
bool RemoveDirectory(const wxString &path);
bool IsValidCppIndetifier(const wxString &id);
bool IsValidCppFile(const wxString &id);
wxString ExpandAllVariables(const wxString &expression, const wxString &projectName, const wxString &fileName);
bool CopyDir(const wxString& src, const wxString& target);

#endif //GLOBALS_H

