#include <wx/tokenzr.h>
#include "globals.h"
#include "singleinstancethreadjob.h"
#include "dirsaver.h"
#include <wx/filename.h>
#include <wx/dir.h>

const wxEventType wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES = wxNewEventType();
const wxEventType wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP = wxNewEventType();

SingleInstanceThreadJob::SingleInstanceThreadJob(wxEvtHandler *parent, const wxString &path)
		: Job(parent)
		, m_path(path)
{
}

SingleInstanceThreadJob::~SingleInstanceThreadJob()
{
}

void SingleInstanceThreadJob::Process(wxThread* thread)
{
	if ( wxFileName::DirExists(m_path) == false ) {
		return;
	}

	Mkdir(m_path + wxT("/ipc"));
	wxString cmd_file(m_path + wxT("/ipc/command.msg"));
	wxString cmd_file_inuse(m_path + wxT("/ipc/command.msg_inuse"));
	
	// loop until the thread is requested to exit
	while ( thread->TestDestroy() == false ) {

		// client will place a file named: message.msg
		
		if ( wxFileName::FileExists(cmd_file) ) {
			// a command file was found, rename it to a temporary name
			if ( wxRenameFile(cmd_file, cmd_file_inuse) ) {

				wxString content;
				ReadFileWithConversion(cmd_file_inuse, content);
				if ( content.IsEmpty() == false ) {
					ProcessFile(content);
				} else {
					// file was found but it is empty
					// just make this app active 
					wxCommandEvent e(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP);
					wxPostEvent(m_parent, e);
				}
				
				// remove it
				wxRemoveFile(cmd_file_inuse);
			}
		}
		wxThread::Sleep(100);
	}
}

void SingleInstanceThreadJob::ProcessFile(const wxString& fileContent)
{
	// each line in the file content contains a file name to be opened, pass it to the main thread
	wxCommandEvent e(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES);
	
	wxArrayString *arr = new wxArrayString();
	wxArrayString a = wxStringTokenize(fileContent, wxT("\n"));
	
	for(size_t i=0; i<a.GetCount(); i++){
		// since we are sending an event between threads, use c_str() to avoid ref-counting
		arr->Add(a.Item(i).c_str());
	}
	
	e.SetClientData(arr);
	wxPostEvent(m_parent, e);
}
