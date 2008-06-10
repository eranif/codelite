#include "precompiled_header.h"
#include <wx/tokenzr.h>
#include "webupdatethread.h"
#include <curl/curl.h>

extern const wxChar *SvnRevision;

const wxEventType wxEVT_CMD_NEW_VERSION_AVAILABLE = wxNewEventType();
WebUpdateJob::WebUpdateJob(wxEvtHandler *parent)
: Job(parent)
{
}

WebUpdateJob::~WebUpdateJob()
{
}

void WebUpdateJob::Process(wxThread* thread)
{
	CURL *curl(NULL);
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		// set the URL to the packages list
		curl_easy_setopt(curl, CURLOPT_URL, "http://codelite.org/packages.txt");
		// set callback for the write operation
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
		// pass this object to the callback
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		// switch off verbose mode
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		// fetch!
		res = curl_easy_perform(curl);
		
		if(res == CURLE_OK){
			// compare the version
			ParseFile();
		}
	}
	curl_global_cleanup();
}

size_t WebUpdateJob::WriteData(void* buffer, size_t size, size_t nmemb, void* obj)
{
	WebUpdateJob *job = reinterpret_cast<WebUpdateJob*>(obj);
	if(job) {
		char *data = new char[size*nmemb+1];
		memcpy(data, buffer, size*nmemb);
		data[size*nmemb] = 0;
		
		job->m_dataRead.Append(_U(data));
		delete [] data;
		return size * nmemb;
	}
	return static_cast<size_t>(-1);
}

void WebUpdateJob::ParseFile()
{
	wxString packageName(wxT("MSW"));
#if defined(__WXGTK__)
	packageName = wxT("GTK");
#elif defined(__WXMAC__)
	packageName = wxT("MAC");
#endif
	
	wxArrayString lines = wxStringTokenize(m_dataRead, wxT("\n"));
	for(size_t i=0; i<lines.GetCount(); i++){
		wxString line = lines.Item(i);
		line = line.Trim().Trim(false);
		if(line.StartsWith(wxT("#"))){
			//comment line
			continue;
		}
		
		// parse the line
		wxArrayString tokens = wxStringTokenize(line, wxT("|"));
		if(tokens.GetCount() > 2){
			// find the entry with our package name
			if(tokens.Item(0).Trim().Trim(false) == packageName){
				wxString url = tokens.Item(2).Trim().Trim(false);
				wxString rev = tokens.Item(1).Trim().Trim(false);
				long currev;
				long webrev(0);
				
				// convert strings to long
				wxString sCurRev(SvnRevision);
				sCurRev.ToLong(&currev);
				
				wxString sUrlRev(rev);
				sUrlRev.ToLong(&webrev);
				
				if( webrev > currev ) {
					// notify the user that a new version is available
					wxCommandEvent e(wxEVT_CMD_NEW_VERSION_AVAILABLE);
					e.SetClientData(new WebUpdateJobData(url.c_str(), currev, webrev));
					wxPostEvent(m_parent, e);
				}
			}
		}
	}
}



