#ifndef __svncommitmsgsmgr__
#define __svncommitmsgsmgr__
#include "wx/arrstr.h"

class SvnCommitMsgsMgr {

	static SvnCommitMsgsMgr* ms_instance;
	wxArrayString m_messages;
	
public:
	static SvnCommitMsgsMgr* Instance();
	static void Release();

private:
	SvnCommitMsgsMgr();
	~SvnCommitMsgsMgr();
	
public:
	void AddMessage(const wxString &message);
	void GetAllMessages(wxArrayString &messages);
	
};
#endif // __svncommitmsgsmgr__
