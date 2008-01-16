#ifndef SVNHANDLER_H
#define SVNHANDLER_H

#include "wx/treectrl.h"
#include "virtualdirtreectrl.h"
#include "map"
#include "imanager.h"

class SvnDriver;

class SvnCmdHandler {
protected:
	SvnDriver *m_svnDriver;
	wxString m_cmd;

public:
	SvnCmdHandler(SvnDriver *driver, const wxString &cmd) : m_svnDriver(driver), m_cmd(cmd) {};
	virtual ~SvnCmdHandler(){}
	const wxString &GetCmd() const {return m_cmd;}
	virtual void ProcessEvent(wxCommandEvent &event) = 0;
};

////////////////////////////////////////////////////
// Command handlers
////////////////////////////////////////////////////
class SvnDefaultCmdHandler : public SvnCmdHandler {
	bool m_needLf;

protected:
	bool IsVerificationNeeded(wxString text);
	bool IsAuthFailed(wxString text);
	bool IsUsernameRequired(wxString text);
	
public:
	SvnDefaultCmdHandler(SvnDriver *driver, const wxString &cmd) : SvnCmdHandler(driver, cmd), m_needLf(false) {}
	virtual ~SvnDefaultCmdHandler(){}
	virtual void ProcessEvent(wxCommandEvent &event);
};

class SvnCommitCmdHandler : public SvnDefaultCmdHandler {
	TreeItemInfo m_item;
public:
	SvnCommitCmdHandler(SvnDriver *driver, const wxString &cmd, const TreeItemInfo &item) 
		: SvnDefaultCmdHandler(driver, cmd)
		, m_item(item)
	{}

	virtual ~SvnCommitCmdHandler(){}
	virtual void ProcessEvent(wxCommandEvent &event);
	wxString GetCwd() const {return m_item.m_fileName.GetPath();}
	const TreeItemInfo& GetItem() const {return m_item;}
};

class SvnDiffCmdHandler : public SvnCmdHandler {
	wxString m_content;
public:
	SvnDiffCmdHandler(SvnDriver *driver, const wxString &cmd) : SvnCmdHandler(driver, cmd) {}
	virtual ~SvnDiffCmdHandler(){}
	virtual void ProcessEvent(wxCommandEvent &event);
};

class SvnChangeLogCmdHandler : public SvnCmdHandler {
	wxString m_content;
	wxString m_outputFile;
public:
	SvnChangeLogCmdHandler(SvnDriver *driver, const wxString &outputFile, const wxString &cmd) 
	: SvnCmdHandler(driver, cmd) 
	, m_outputFile(outputFile)
	{}
	
	virtual ~SvnChangeLogCmdHandler(){}
	virtual void ProcessEvent(wxCommandEvent &event);
};

#endif //SVNHANDLER_H
