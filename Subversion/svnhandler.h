//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : svnhandler.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #ifndef SVNHANDLER_H
#define SVNHANDLER_H

#include "svnpostcmdaction.h"
#include "wx/treectrl.h"
#include "virtualdirtreectrl.h"
#include "map"
#include "imanager.h"

class SvnDriver;
class SvnPostCmdAction;

class SvnCmdHandler {
protected:
	SvnDriver *m_svnDriver;
	wxString m_cmd;
	SvnPostCmdAction *m_postCmd;

public:
	SvnCmdHandler(SvnDriver *driver, const wxString &cmd) : m_svnDriver(driver), m_cmd(cmd), m_postCmd(NULL) {};
	virtual ~SvnCmdHandler(){}
	const wxString &GetCmd() const {return m_cmd;}
	virtual void ProcessEvent(wxCommandEvent &event) = 0;

	void SetPostCmdAction(SvnPostCmdAction* handler) {this->m_postCmd = handler;}
	SvnPostCmdAction* GetPostCmdAction() {return m_postCmd;}
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
	bool IsConflictWasFound(wxString text);
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
	wxString m_fileName;
public:
	SvnDiffCmdHandler(SvnDriver *driver, const wxString &cmd, const wxString &fileName) : SvnCmdHandler(driver, cmd), m_fileName(fileName) {}
	virtual ~SvnDiffCmdHandler(){}
	virtual void ProcessEvent(wxCommandEvent &event);
};

enum {
	SvnChangeLog_Compact = 0x00000001,
};

class SvnChangeLogCmdHandler : public SvnCmdHandler {
	wxString m_content;
	wxString m_outputFile;
	size_t m_flags;

public:
	SvnChangeLogCmdHandler(SvnDriver *driver, const wxString &outputFile, const wxString &cmd, size_t flags)
	: SvnCmdHandler(driver, cmd)
	, m_outputFile(outputFile)
	, m_flags(flags)
	{}

	virtual ~SvnChangeLogCmdHandler(){}
	virtual void ProcessEvent(wxCommandEvent &event);
};

#endif //SVNHANDLER_H
