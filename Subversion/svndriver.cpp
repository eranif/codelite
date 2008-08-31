//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : svndriver.cpp
//
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
#include "globals.h"
#include "svndriver.h"
#include <wx/filedlg.h>
#include "svntab.h"
#include "svnpostcmdaction.h"
#include "wx/tokenzr.h"
#include "dirsaver.h"
#include "svndlg.h"
#include "wx/ffile.h"
#include "wx/filefn.h"
#include "svnhandler.h"
#include "subversion.h"
#include "svnlogdlg.h"
#include "procutils.h"
#include "svnadditemsdlg.h"
#include "svnxmlparser.h"
#include "logindlg.h"

#define ENTER_SVN_AND_SELECT() {\
		if(m_cmd){\
			return;\
		}\
		SelectSvnTab();\
	}

#define ENTER_SVN() {\
		if(m_cmd){\
			return;\
		}\
	}

static const wxChar* commandSeparator = wxT("----\n");

BEGIN_EVENT_TABLE(SvnDriver, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE, SvnDriver::OnSvnProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED, SvnDriver::OnSvnProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, SvnDriver::OnSvnProcess)
END_EVENT_TABLE()

SvnDriver::SvnDriver(SubversionPlugin *plugin, IManager *mgr)
		: m_cmd(NULL)
		, m_manager(mgr)
		, m_curHandler(NULL)
		, m_plugin(plugin)
		, m_commitWithPass(false)
{
}

SvnDriver::~SvnDriver()
{
	if (m_cmd) {
		delete m_cmd;
		m_cmd = NULL;
	}
}

void SvnDriver::SetCommitWithPassword(bool need)
{
	m_commitWithPass = need;
}

void SvnDriver::Shutdown()
{
	if (m_cmd) {
		m_cmd->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(SvnDriver::OnSvnProcessTerminated), NULL, this);
		m_cmd->Stop();
	}
}

void SvnDriver::SelectSvnTab()
{
	Notebook *book = m_manager->GetOutputPaneNotebook();
	wxString curSel = book->GetPageText((size_t)book->GetSelection());
	if (curSel == wxT("Subversion")) {
		return;
	}

	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("Subversion")) {
			book->SetSelection(i);
			break;
		}
	}

	// make sure the Ouput view is visible
	wxAuiPaneInfo &info = m_manager->GetDockingManager()->GetPane(wxT("Output View"));
	if (info.IsOk() && !info.IsShown()) {
		info.Show();
		m_manager->GetDockingManager()->Update();
	}
}

void SvnDriver::OnSvnProcessTerminated(wxProcessEvent &event)
{
	m_cmd->ProcessEnd(event);
	m_cmd->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(SvnDriver::OnSvnProcessTerminated), NULL, this);
	delete m_cmd;
	m_cmd = NULL;

	bool callRefresh(false);

	wxString cmd = m_curHandler->GetCmd();
	TreeItemInfo item;
	SvnCommitCmdHandler *handler = dynamic_cast<SvnCommitCmdHandler*>(m_curHandler);
	if (handler) {
		item = handler->GetItem();
		callRefresh = true;
	}

	SvnPostCmdAction *postCmd = m_curHandler->GetPostCmdAction();
	delete m_curHandler;
	m_curHandler = NULL;

	if (m_commitWithPass) {
		//Commit failed due to authentication error, try again with the same paramters, but this time prompt the user
		//for username and password
		m_commitWithPass = false;

		//first we need to perform clean
		wxString command;
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("cleanup ");

		DirSaver ds;
		wxSetWorkingDirectory(item.m_fileName.GetPath());
		wxArrayString output;

		PrintMessage(wxT("Performning cleanup...\n"));
		ProcUtils::ExecuteCommand(command, output);
		PrintMessage(wxT("Done\n"));
		CommitWithAuth(cmd, item);

	} else {
		//operation completed successfully
		if (postCmd) {
			postCmd->DoCommand();
			delete postCmd;
		}
	}
}

void SvnDriver::DisplayDiffFile(const wxString &fileName, const wxString &content)
{
	bool hasExternalDiff = !m_plugin->GetOptions().GetDiffCmd().empty();
	if ( !hasExternalDiff || ( hasExternalDiff && ((m_plugin->GetOptions().GetFlags() & SvnCaptureDiffOutput) != 0) ) ) {
		//Load the output file into the editor
		wxString tmpFile = wxFileName::GetTempDir();

		wxFileName fn(fileName);
		tmpFile << wxT("/") << fn.GetFullName() << wxT(".diff");

		wxFFile file(tmpFile, wxT("w+"));
		if (file.IsOpened()) {
			file.Write(content);
			file.Close();

			m_manager->OpenFile(tmpFile, wxEmptyString);
		}
	}
}

void SvnDriver::DisplayLog(const wxString &outputFile, const wxString &content)
{
	//Load the output file into the editor
	wxString tmpFile(outputFile);
	if (tmpFile.IsEmpty()) {
		tmpFile = wxFileName::GetTempDir();
#ifdef __WXGTK__
		tmpFile << wxT("/");
#endif
		tmpFile << wxT("ChangeLog.txt");
	}

	wxFFile file(tmpFile, wxT("w+"));
	if (file.IsOpened()) {
		file.Write(content);
		file.Close();
		m_manager->OpenFile(tmpFile, wxEmptyString, wxNOT_FOUND);
	}
}

void SvnDriver::ExecCommand(const wxString &cmd, bool hide)
{
	//execute the command line
	//the async command is a one time executable object,
	m_cmd = new AsyncExeCmd(this);
	m_cmd->Execute(cmd, hide);
	if (m_cmd->GetProcess()) {
		m_cmd->GetProcess()->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(SvnDriver::OnSvnProcessTerminated), NULL, this);
	}
}

void SvnDriver::Update(SvnPostCmdAction *handler)
{
	ENTER_SVN_AND_SELECT()

	wxString command;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DirSaver ds;
	wxString fileName;
	wxSetWorkingDirectory(item.m_fileName.GetPath());
	if (!item.m_fileName.IsDir()) {
		//we got a file name
		fileName = item.m_fileName.GetFullName();
	}

#ifdef __WXMSW__
	fileName.Prepend(wxT("\""));
	fileName.Append(wxT("\""));
#endif

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("update ") << fileName ;
	m_curHandler = new SvnDefaultCmdHandler(this, command);
	m_curHandler->SetPostCmdAction(handler);
	ExecCommand(command);
}

void SvnDriver::CommitWithAuth(const wxString &cmd, const TreeItemInfo &item)
{
	ENTER_SVN_AND_SELECT();
	wxString command(cmd);
	LoginDialog *dlg = new LoginDialog(m_manager->GetTheApp()->GetTopWindow());
	if (dlg->ShowModal() == wxID_OK) {

		wxString username = dlg->GetUsername();
		wxString password = dlg->GetPassword();

		command << wxT(" --username ") << username;
		command << wxT(" --password ") << password;

		m_curHandler = new SvnCommitCmdHandler(this, cmd, item);
		DirSaver ds;
		wxSetWorkingDirectory(item.m_fileName.GetPath());

		ExecCommand(command);
	}
	dlg->Destroy();
}

void SvnDriver::Commit()
{
	ENTER_SVN_AND_SELECT()
	wxString command, comment;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DirSaver ds;
	wxString fileName;
	wxSetWorkingDirectory(item.m_fileName.GetPath());
	if (!item.m_fileName.IsDir()) {
		//we got a file name
		fileName = item.m_fileName.GetFullName();
	}

#ifdef __WXMSW__
	fileName.Prepend(wxT("\""));
	fileName.Append(wxT("\""));
#endif

	//get the comment to enter
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" status -q ") << fileName;

	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);

	wxString text;
	for (size_t i=0; i< output.GetCount(); i++) {
		wxString file_name(output.Item(i));
		file_name = file_name.Trim().Trim(false);
		text << wxT("# ") << file_name << wxT("\n");
	}

	//Get Log message from user
	SvnDlg *dlg = new SvnDlg(NULL);
	text.Prepend(dlg->GetValue()+wxT("# Svn status:\n"));
	dlg->SetValue(text);

	if (dlg->ShowModal() == wxID_OK) {
		comment = dlg->GetValue();
		command.Clear();
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("commit ") << fileName << wxT(" -m \"") << comment << wxT("\"");
		m_curHandler = new SvnCommitCmdHandler(this, command, item);
		ExecCommand(command);
	}
	dlg->Destroy();
}

void SvnDriver::ResolveConflictedFile(const wxFileName& filename, SvnPostCmdAction* handler)
{
	ENTER_SVN_AND_SELECT()
	wxString command;
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" resolved \"") << filename.GetFullPath() << wxT("\"");

	// set the post command
	if (handler) {
		m_curHandler = new SvnDefaultCmdHandler(this, command);
		m_curHandler->SetPostCmdAction(handler);
	}
	ExecCommand(command);
}

void SvnDriver::CommitFile(const wxString &fileName, SvnPostCmdAction *handler)
{
	ENTER_SVN_AND_SELECT()
	wxString command, comment;

	//get the comment to enter
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" status -q ") << fileName;

	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);

	wxString text;
	for (size_t i=0; i< output.GetCount(); i++) {
		wxString file_name(output.Item(i));
		file_name = file_name.Trim().Trim(false);
		text << wxT("# ") << file_name << wxT("\n");
	}

	//Get Log message from user
	SvnDlg *dlg = new SvnDlg(NULL);
	text.Prepend(dlg->GetValue()+wxT("# Svn status:\n"));
	dlg->SetValue(text);
	TreeItemInfo dummy;
	if (dlg->ShowModal() == wxID_OK) {
		comment = dlg->GetValue();
		command.Clear();
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("commit ") << fileName << wxT(" -m \"") << comment << wxT("\"");
		m_curHandler = new SvnCommitCmdHandler(this, command, dummy);
		m_curHandler->SetPostCmdAction(handler);
		ExecCommand(command);
	}
	dlg->Destroy();
}

void SvnDriver::UpdateFile(const wxString &fileName, SvnPostCmdAction *handler)
{
	ENTER_SVN_AND_SELECT()

	wxString command;
	wxString file_name(fileName);
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("update ") << file_name ;
	m_curHandler = new SvnDefaultCmdHandler(this, command);
	m_curHandler->SetPostCmdAction(handler);
	ExecCommand(command);
}

void SvnDriver::DiffFile(const wxFileName &fileName)
{
	ENTER_SVN_AND_SELECT()
	wxString command, comment;

	DirSaver ds;
	wxString file_name;
	wxSetWorkingDirectory(fileName.GetPath());

	//did we get a directory?
	if (fileName.IsDir()) {
		file_name = wxT(".");
	} else {
		file_name = fileName.GetFullName();
	}

	const wxString& diffCmd = m_plugin->GetOptions().GetDiffCmd();
	bool hasExternalDiffCmd = !diffCmd.empty();
	if ( !hasExternalDiffCmd ) {
#ifdef __WXMSW__
		file_name.Prepend(wxT("\""));
		file_name.Append(wxT("\""));
#endif

		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("diff ") << file_name;
	} else {
		if ( !::wxFileExists( diffCmd ) ) {
			PrintMessage( wxString::Format( wxT("'%s' is not a valid command.\n%s"), diffCmd.c_str(), commandSeparator ) );
			return;
		}

		// export BASE revision of file to tmp file
		const wxString& base = wxFileName::CreateTempFileName( wxT("svnExport"), (wxFile*)NULL );
		::wxRemoveFile( base ); // just want the name, not the file.
		wxString exportCmd;
		exportCmd << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		exportCmd << wxT("export -r BASE \"") << file_name << wxT("\" ") << base;
		wxArrayString output;
		ProcUtils::ExecuteCommand(exportCmd, output);

		// get number of Base Revision
		wxString info;
		ExecInfoCommand( fileName, info );
		wxString baseRev = SvnXmlParser::GetRevision( info );
		if ( baseRev.empty() ) {
			baseRev = wxT("base");
		} else {
			baseRev.Prepend(wxT("revision "));
		}

		// Build external diff command
		wxString args = m_plugin->GetOptions().GetDiffArgs();
		args.Replace( wxT("%base"), base );
		args.Replace( wxT("%bname"), wxString::Format( wxT("\"%s (%s)\""), file_name.c_str(), baseRev.c_str() ) );
		args.Replace( wxT("%mine"), wxString::Format( wxT("\"%s\""), file_name.c_str() ) );
		args.Replace( wxT("%mname"), wxString::Format( wxT("\"%s (working copy)\""), file_name.c_str() ) );
		command << wxT("\"") << diffCmd << wxT("\" ");
		command << args;
	}

	m_curHandler = new SvnDiffCmdHandler(this, command, fileName.GetFullPath());
	ExecCommand(command, !hasExternalDiffCmd);
}

void SvnDriver::Diff()
{
	ENTER_SVN_AND_SELECT()
	wxString command, comment;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DiffFile( item.m_fileName );
}

void SvnDriver::Delete()
{
	ENTER_SVN_AND_SELECT();
	wxString command, comment;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DirSaver ds;
	wxString fileName;
	wxSetWorkingDirectory(item.m_fileName.GetPath());

	fileName = item.m_fileName.GetFullPath();
	fileName.Replace(wxT("\\"), wxT("/"));

#ifdef __WXMSW__
	fileName.Prepend(wxT("\""));
	fileName.Append(wxT("\""));
#endif

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" del --force ") << fileName;

	//Delete is a quick operation, do it synch
	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);
	PrintMessage(output);
	PrintMessage(commandSeparator);
}

void SvnDriver::Revert()
{
	ENTER_SVN_AND_SELECT();
	wxString command, comment;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DirSaver ds;
	wxString fileName;
	wxSetWorkingDirectory(item.m_fileName.GetPath());

	fileName = item.m_fileName.GetFullPath();
	fileName.Replace(wxT("\\"), wxT("/"));

#ifdef __WXMSW__
	fileName.Prepend(wxT("\""));
	fileName.Append(wxT("\""));
#endif

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" revert --recursive ") << fileName;

	//Revert is a quick operation, do it synch
	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);
	PrintMessage(output);
	PrintMessage(commandSeparator);
}

void SvnDriver::RevertFile(const wxFileName &fileName, SvnPostCmdAction *handler)
{
	ENTER_SVN_AND_SELECT();
	wxString command, comment;

	DirSaver ds;
	wxString file_name;
	wxSetWorkingDirectory(fileName.GetPath());

	file_name = fileName.GetFullPath();
	file_name.Replace(wxT("\\"), wxT("/"));

#ifdef __WXMSW__
	file_name.Prepend(wxT("\""));
	file_name.Append(wxT("\""));
#endif

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" revert --recursive ") << file_name;

	//Revert is a quick operation, do it synch
	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);
	PrintMessage(output);
	PrintMessage(commandSeparator);

	if (handler) {
		handler->DoCommand();
		delete handler;
	}
}

void SvnDriver::ChangeLog()
{
	ENTER_SVN_AND_SELECT()
	wxString command, comment;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DirSaver ds;
	wxString fileName;
	wxSetWorkingDirectory(item.m_fileName.GetPath());

	//did we get a directory?
	if (item.m_fileName.IsDir()) {
		fileName = wxT(".");
	} else {
		fileName = item.m_fileName.GetFullName();
	}

#ifdef __WXMSW__
	fileName.Prepend(wxT("\""));
	fileName.Append(wxT("\""));
#endif

	SvnLogDlg *dlg = new SvnLogDlg(NULL);
	if (dlg->ShowModal() == wxID_OK) {

		wxString outputFile = dlg->GetFilePath();
		wxString fromStr = dlg->GetFromRevision();
		wxString toStr = dlg->GetToRevision();

		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT(" log ") << fileName;
		command << wxT(" -r ") << fromStr << wxT(":") << toStr;

		m_curHandler = new SvnChangeLogCmdHandler(this, outputFile, command);
		ExecCommand(command);
	}
	dlg->Destroy();
}

void SvnDriver::Add(const wxFileName &filename, SvnPostCmdAction *handler)
{
	ENTER_SVN_AND_SELECT()
	wxString command;
	DirSaver ds;
	wxString fileName;
	wxSetWorkingDirectory(filename.GetPath());

	fileName = filename.GetFullPath();
	fileName.Replace(wxT("\\"), wxT("/"));

#ifdef __WXMSW__
	fileName.Prepend(wxT("\""));
	fileName.Append(wxT("\""));
#endif

	wxArrayString output;
	if (filename.IsDir()) {

		// Execute a sync command to get modified files
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("status --xml --non-interactive -q --no-ignore ") << fileName;
		output.Clear();
		ProcUtils::ExecuteCommand(command, output);

		wxArrayString files;
		if (GetFilesList(output, files)) {
			SvnAddItemsDlg *dlg = new SvnAddItemsDlg(NULL, files, m_manager->GetConfigTool());
			if (dlg->ShowModal() == wxID_OK) {
				//add the selected files
				wxString filesToAdd = dlg->GetFiles();
				if (filesToAdd.IsEmpty() == false) {
					command.Clear();
					command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
					command << wxT(" add -N ") << filesToAdd << wxT("") ;
					output.Clear();
					ProcUtils::ExecuteCommand(command, output);
					PrintMessage(output);

					if (handler) {
						handler->DoCommand();
					}

				}
			} else {
				PrintMessage(wxT("Operation Canceled\n"));
			}
			dlg->Destroy();
		} else {
			PrintMessage(wxT("Nothing to be added\n"));
			PrintMessage(commandSeparator);
		}

	} else {

		// Execute a sync command to get modified files
		command.Clear();
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT(" add -N ") << fileName;
		output.Clear();
		ProcUtils::ExecuteCommand(command, output);
		PrintMessage(output);

		if (handler) {
			handler->DoCommand();
		}

	}
	//free the handler
	if (handler) {
		delete handler;
	}
}

void SvnDriver::Add()
{
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);
	Add(item.m_fileName);
}

bool SvnDriver::GetFilesList(const wxArrayString& output, wxArrayString &files)
{
	wxString str;
	for (size_t i=0; i<output.GetCount(); i++) {
		str << output.Item(i);
	}

	SvnXmlParser::GetFiles(str, files, SvnXmlParser::StateUnversioned);
	return files.IsEmpty() == false;
}

void SvnDriver::Cleanup()
{
	ENTER_SVN_AND_SELECT()
	wxString command;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	DirSaver ds;
	wxSetWorkingDirectory(item.m_fileName.GetPath());

	//did we get a directory?
	if (item.m_fileName.IsDir()) {
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("cleanup ");
		m_curHandler = new SvnDefaultCmdHandler(this, command);
		ExecCommand(command);
	}
}

/////////////////////////////////////////////////////
// Event handlers
/////////////////////////////////////////////////////

void SvnDriver::PrintMessage(const wxArrayString &textArr)
{
	for (size_t i=0; i<textArr.GetCount(); i++) {
		wxString text(textArr.Item(i));
		text = text.Trim().Trim(false);
		PrintMessage(text + wxT("\n"));
	}
}

void SvnDriver::PrintMessage(const wxString &text)
{
	Notebook *book = m_manager->GetOutputPaneNotebook();
	SvnTab *svnWin (NULL);
	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("Subversion")) {
			svnWin = dynamic_cast< SvnTab *>(book->GetPage(i));
			break;
		}
	}
	if (svnWin) {
		svnWin->AppendText(text);
	}
}



void SvnDriver::OnSvnProcess(wxCommandEvent &event)
{
	m_curHandler->ProcessEvent(event);
}

void SvnDriver::ExecStatusCommand(const wxString &path, wxString &output)
{
	wxString command;
	DirSaver ds;
	wxSetWorkingDirectory(path);

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml -q --non-interactive --no-ignore ");

	wxArrayString outputArr;
	ProcUtils::ExecuteCommand(command, outputArr);

	for (size_t i=0; i<outputArr.GetCount(); i++) {
		output << outputArr.Item(i);
	}
}

void SvnDriver::ExecInfoCommand(const wxFileName &filename, wxString &output)
{
	wxString command;
	DirSaver ds;
	wxSetWorkingDirectory(filename.GetPath());

	//did we get a directory?
	wxString file_name;
	if (filename.IsDir()) {
		file_name = wxT(".");
	} else {
		file_name = filename.GetFullName();
	}

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("info --xml --non-interactive \"") << file_name << wxT("\"");

	wxArrayString outputArr;
	ProcUtils::ExecuteCommand(command, outputArr);

	for (size_t i=0; i<outputArr.GetCount(); i++) {
		output << outputArr.Item(i);
	}
}

void SvnDriver::Abort()
{
	if (m_cmd) {
		m_cmd->Stop();
	}
}

void SvnDriver::ApplyPatch(SvnPostCmdAction *handler)
{
	// pass the SVN guard
	ENTER_SVN_AND_SELECT()
	
	wxString command;
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);

	if (item.m_fileName.IsDir()) {
		DirSaver ds;
		wxSetWorkingDirectory(item.m_fileName.GetPath());

		// open a file selector to select the patch file
		const wxString ALL(	wxT("Patch files (*.patch)|*.patch|")
							wxT("Diff files (*.diff)|*.diff|")
		                    wxT("All Files (*)|*"));

		wxFileDialog fdlg(m_manager->GetTheApp()->GetTopWindow(),
		                                      wxT("Select a patch file"),
		                                      item.m_fileName.GetPath(),
		                                      wxEmptyString,
		                                      ALL,
		                                      wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		                                      wxDefaultPosition);
		if(fdlg.ShowModal() == wxID_OK){
			// try to load and convert the file into the platform line ending
			wxString fileContent, convertedContent;
			wxString eol(wxT("\n"));
			
#if defined(__WXMSW__)
			eol = wxT("\r\n");
#endif
			if(!ReadFileWithConversion(fdlg.GetPath(), fileContent)){
				PrintMessage(wxString::Format(wxT("Failed to read patch file '%s'"), fdlg.GetPath().c_str()));
				return;
			}
			
			wxArrayString lines = wxStringTokenize(fileContent, wxT("\r\n"), wxTOKEN_STRTOK);
			for(size_t i=0; i<lines.GetCount(); i++){
				convertedContent << lines.Item(i) << eol;
			}
			
			wxString tmpFileName(fdlg.GetPath()+wxT(".tmp"));
			if(!WriteFileWithBackup(tmpFileName, convertedContent, false)){
				// failed to write the temporary file
				PrintMessage(wxString::Format(wxT("Failed to convert patch file EOL mode '%s'"), tmpFileName.c_str()));
				return;
			}
			
			// execute the command
			command << wxT("patch -p0 -i \"") << tmpFileName << wxT("\"");
			m_curHandler = new SvnDefaultCmdHandler(this, command);
			m_curHandler->SetPostCmdAction(handler);
			
			ExecCommand(command);
		}
	}
}
