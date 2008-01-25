#include "wx/tokenzr.h"
#include "globals.h"
#include "winprocess.h"
#include "wx/app.h"
#include "wx/window.h"
#include "wx/listctrl.h"
#include "wx/ffile.h"
#include "procutils.h"

void SendCmdEvent(int eventId, void *clientData)
{
	wxCommandEvent e(eventId);
	if (clientData) {
		e.SetClientData(clientData);
	}
	wxTheApp->ProcessEvent(e);
}

void PostCmdEvent(int eventId, void *clientData)
{
	wxCommandEvent e(eventId);
	if (clientData) {
		e.SetClientData(clientData);
	}
	wxTheApp->AddPendingEvent(e);
}

void SetColumnText (wxListCtrl *list, long indx, long column, const wxString &rText )
{
//	list->Freeze();
	wxListItem list_item;
	list_item.SetId ( indx );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	list_item.SetText ( rText );
	list->SetItem ( list_item );
//	list->Thaw();
}

wxString GetColumnText(wxListCtrl *list, long index, long column)
{
	wxListItem list_item;
	list_item.SetId ( index );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	list->GetItem ( list_item );
	return list_item.GetText();
}

bool ReadFileWithConversion(const wxString &fileName, wxString &content)
{
	wxFFile file(fileName, wxT("rb"));

	//first try the Utf8
	file.ReadAll(&content, wxConvUTF8);
	if (content.IsEmpty()) {
		//try local
		file.Seek(0);

		file.ReadAll(&content, wxConvLocal);
		if (content.IsEmpty()) {
			file.Seek(0);
			file.ReadAll(& content, wxConvLibc);
		}
	}
	return content.IsEmpty() == false;
}

bool RemoveDirectory(const wxString &path)
{
	wxString cmd;
	if (wxGetOsVersion() & wxOS_WINDOWS) {
		//any of the windows variants
		cmd << wxT("rmdir /S /Q ") << wxT("\"") << path << wxT("\"");
	} else {
		cmd << wxT("\rm -fr ") << wxT("\"") << path << wxT("\"");
	}
	wxArrayString output;
	return wxShell(cmd);
}

bool IsValidCppIndetifier(const wxString &id)
{
	if (id.IsEmpty()) {
		return false;
	}
	//first char can be only _A-Za-z
	wxString first( id.Mid(0, 1) );
	if (first.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")) != wxString::npos ) {
		return false;
	}
	//make sure that rest of the id contains only a-zA-Z0-9_
	if (id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
		return false;
	}
	return true;
}

long AppendListCtrlRow(wxListCtrl *list)
{
	long item;
	wxListItem info;
	// Set the item display name
	info.SetColumn(0);
	item = list->InsertItem(info);

#ifdef __WXMAC__
	item = list->GetItemCount()-1;
#endif
	return item;
}

/**
 * \brief a safe function that executes 'command' and returns its output. This function
 * is safed to be called from secondary thread 
 * \param command
 * \param output
 */
void SafeExecuteCommand(const wxString &command, wxArrayString &output)
{
#ifdef __WXMSW__
	wxString errMsg;
	WinProcess *proc = WinProcess::Execute(command, errMsg);
	if (!proc) {
		return;
	}
	
	// wait for the process to terminate
	wxString tmpbuf;
	wxString buff;
	static int maxRetries (1000);
	
	int retries(0);
	while (proc->IsAlive() && retries < maxRetries) {
		proc->Read(tmpbuf);
		buff << tmpbuf;
		wxThread::Sleep(100);
		retries++;
	}
	
	tmpbuf.Empty();
	proc->Read(tmpbuf);
	while( tmpbuf.IsEmpty() == false && retries < maxRetries) {
		buff << tmpbuf;
		tmpbuf.Empty();
		proc->Read(tmpbuf);
		retries++;
	}
	
	//convert buff into wxArrayString
	output = wxStringTokenize(buff, wxT("\n"));
	proc->Cleanup();
	delete proc;

#else
	ProcUtils::ExecuteCommand(command, output);
#endif
}
bool IsValidCppFile(const wxString &id)
{
	if (id.IsEmpty()) {
		return false;
	}
	
	//make sure that rest of the id contains only a-zA-Z0-9_
	if (id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
		return false;
	}
	return true;	
}

