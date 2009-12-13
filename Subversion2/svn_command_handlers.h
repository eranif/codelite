#ifndef SVNCOMMITHANDLER_H
#define SVNCOMMITHANDLER_H

#include "svn_default_command_handler.h" // Base class

//----------------------------------------------------
// Commit Handler
//----------------------------------------------------

class SvnCommitHandler : public SvnDefaultCommandHandler {
	wxEvtHandler *m_owner;
public:
	SvnCommitHandler(Subversion2 *plugin, wxEvtHandler *owner) : SvnDefaultCommandHandler(plugin), m_owner(owner) {};
	virtual ~SvnCommitHandler(){};

public:
	virtual void Process(const wxString &output);
};

//----------------------------------------------------
// Update Handler
//----------------------------------------------------

class SvnUpdateHandler : public SvnDefaultCommandHandler {

public:
	SvnUpdateHandler(Subversion2 *plugin) : SvnDefaultCommandHandler(plugin) {};
	virtual ~SvnUpdateHandler(){};

public:
	virtual void Process(const wxString &output);
};

//----------------------------------------------------
// Diff Handler
//----------------------------------------------------

class SvnDiffHandler : public SvnDefaultCommandHandler {

public:
	SvnDiffHandler(Subversion2 *plugin) : SvnDefaultCommandHandler(plugin) {};
	virtual ~SvnDiffHandler(){};

public:
	virtual void Process(const wxString &output);
};

//----------------------------------------------------
// Patch Handler
//----------------------------------------------------

class SvnPatchHandler : public SvnDefaultCommandHandler {

public:
	SvnPatchHandler(Subversion2 *plugin) : SvnDefaultCommandHandler(plugin) {};
	virtual ~SvnPatchHandler(){};

public:
	virtual void Process(const wxString &output);
};

//----------------------------------------------------
// Dry-Run Patch Handler
//----------------------------------------------------

class SvnPatchDryRunHandler : public SvnDefaultCommandHandler {

public:
	SvnPatchDryRunHandler(Subversion2 *plugin) : SvnDefaultCommandHandler(plugin) {};
	virtual ~SvnPatchDryRunHandler(){};

public:
	virtual void Process(const wxString &output);
};
#endif // SVNCOMMITHANDLER_H
