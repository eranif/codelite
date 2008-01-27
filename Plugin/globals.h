#ifndef GLOBALS_H
#define GLOBALS_H

#include "wx/string.h"
#include "wx/arrstr.h"
class wxListCtrl;

extern void SendCmdEvent(int eventId, void *clientData = NULL);
extern void PostCmdEvent(int eventId, void *clientData = NULL);
extern void SetColumnText (wxListCtrl *list, long indx, long column, const wxString &rText );
extern wxString GetColumnText(wxListCtrl *list, long index, long column);
extern long AppendListCtrlRow(wxListCtrl *list);
extern bool ReadFileWithConversion(const wxString &fileName, wxString &content);
extern bool RemoveDirectory(const wxString &path);
extern bool IsValidCppIndetifier(const wxString &id);
extern bool IsValidCppFile(const wxString &id);
	
#endif //GLOBALS_H

