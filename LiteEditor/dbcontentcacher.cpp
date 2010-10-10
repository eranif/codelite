#include "dbcontentcacher.h"
#include <wx/ffile.h>
#include <wx/xrc/xmlres.h>
#include <wx/stopwatch.h>

const wxEventType wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED = XRCID("wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED");

DbContentCacher::DbContentCacher(wxEvtHandler* parent, const wxChar* dbfilename)
	: Job(parent)
	, m_filename(dbfilename)
{
}

DbContentCacher::~DbContentCacher()
{
}

void DbContentCacher::Process(wxThread* thread)
{
	wxStopWatch sw;
	sw.Start();
	
	wxFFile dbFile(m_filename, wxT("rb"));
	if(dbFile.IsOpened()) {
		wxString fileContent;
		wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
		dbFile.ReadAll(&fileContent, fontEncConv);
		dbFile.Close();
	}
	
	if (m_parent) {
		wxCommandEvent e(wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED);
		e.SetString(wxString::Format(wxT("Symbols file loaded into OS file system cache (%ld seconds)"), sw.Time()/1000).c_str());
		m_parent->AddPendingEvent(e);
	}
}
