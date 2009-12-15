#ifndef COMMITMESSAGESCACHE_H
#define COMMITMESSAGESCACHE_H

#include <wx/arrstr.h>

class CommitMessagesCache
{
	wxArrayString m_messages;
protected:
	wxString FormatMessage(const wxString &message);
	
public:
	CommitMessagesCache() {
	}
	
	virtual ~CommitMessagesCache() {
	}
	
	/**
	 * @brief return the messages 
	 * @param messages array containing the messages
	 * @param previews array containing preview (first line) of the messages
	 */
	void GetMessages(wxArrayString &messages, wxArrayString &previews);
	/**
	 * @brief add message to the cache. duplicate messages are ignored
	 * @param message
	 */
	void AddMessage (const wxString &message);
};

#endif // COMMITMESSAGESCACHE_H

