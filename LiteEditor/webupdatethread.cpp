// temporarly this is supported only under Windows & GTK
#if defined (__WXGTK__) || defined (__WXMSW__)

#include "webupdatethread.h"
#include <curl/curl.h>

WebUpdateThread::WebUpdateThread()
{
}

WebUpdateThread::~WebUpdateThread()
{
}

void WebUpdateThread::Process(wxThread* thread)
{
	CURL *curl(NULL);
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
	}
	curl_global_cleanup();
}

#endif
