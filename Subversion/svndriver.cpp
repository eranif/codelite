#include "svndriver.h"
#include "wx/tokenzr.h"
#include "dirsaver.h"
#include "svndlg.h"
#include "wx/ffile.h"
#include "svnhandler.h"
#include "subversion.h"
#include "svnlogdlg.h"
#include "procutils.h"
#include "svnadditemsdlg.h"
#include "svnxmlparser.h"
#include "logindlg.h"

#define TRYENTERSVN() {if(m_cmd){return;} SelectSvnTab();}

BEGIN_EVENT_TABLE(SvnDriver, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE, SvnDriver::OnSvnProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED, SvnDriver::OnSvnProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, SvnDriver::OnSvnProcess)
END_EVENT_TABLE()

static void StripComments(wxString &comment)
{
	wxStringTokenizer tok(comment, wxT("\n"), wxTOKEN_STRTOK);
	comment.Clear();
	while(tok.HasMoreTokens()){
		wxString line = tok.GetNextToken();
		line = line.Trim().Trim(false);
		if(!line.StartsWith(wxT("#"))){
			comment << line << wxT("\n");
		}
	}
}

SvnDriver::SvnDriver(SubversionPlugin *plugin, IManager *mgr)
: m_cmd(NULL)
, m_manager(mgr)
, m_curHandler(NULL)
, m_plugin(plugin)
, m_commitWithPass(false)
{
}

SvnDriver::~SvnDriver() {
	if (m_cmd) {
		delete m_cmd;
		m_cmd = NULL;
	}
}

void SvnDriver::SetCommitWithPassword(bool need)
{
	m_commitWithPass = need;
}

void SvnDriver::Shutdown(){
	if(m_cmd){
		m_cmd->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(SvnDriver::OnSvnProcessTerminated), NULL, this);	
		m_cmd->Stop();
	}
}

void SvnDriver::SelectSvnTab() {
	wxFlatNotebook *book = m_manager->GetOutputPaneNotebook();
	wxString curSel = book->GetPageText((size_t)book->GetSelection());
	if (curSel == wxT("SVN")) {
		return;
	}

	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("SVN")) {
			book->SetSelection(i);
			break;
		}
	}
}

void SvnDriver::OnSvnProcessTerminated(wxProcessEvent &event) {
	m_cmd->ProcessEnd(event);
	m_cmd->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(SvnDriver::OnSvnProcessTerminated), NULL, this);
	delete m_cmd;
	m_cmd = NULL;

	bool callRefresh(false);

	wxString cmd = m_curHandler->GetCmd();
	TreeItemInfo item;
	SvnCommitCmdHandler *handler = dynamic_cast<SvnCommitCmdHandler*>(m_curHandler);
	if(handler){
		item = handler->GetItem();
		callRefresh = true;
	}

	delete m_curHandler;
	m_curHandler = NULL;

	if(m_commitWithPass){
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

	}
}

void SvnDriver::DisplayDiffFile(const wxString &content) {
	//Load the output file into the editor
	wxString tmpFile = wxFileName::GetTempDir();
	tmpFile << wxT("/Diff");

	wxFFile file(tmpFile, wxT("w+"));
	if (file.IsOpened()) {
		file.Write(content);
		file.Close();

		m_manager->OpenFile(tmpFile, wxEmptyString);
	}
}

void SvnDriver::DisplayLog(const wxString &outputFile, const wxString &content) {
	//Load the output file into the editor
	wxString tmpFile(outputFile);
	if (tmpFile.IsEmpty()) {
		tmpFile = wxFileName::GetTempDir();
		tmpFile << wxT("ChangeLog.txt");
	}

	wxFFile file(tmpFile, wxT("w+"));
	if (file.IsOpened()) {
		file.Write(content);
		file.Close();
		m_manager->OpenFile(tmpFile, wxEmptyString, wxNOT_FOUND);
	}
}

void SvnDriver::ExecCommand(const wxString &cmd) {
	//execute the command line
	//the async command is a one time executable object,
	m_cmd = new AsyncExeCmd(this);
	m_cmd->Execute(cmd);
	if (m_cmd->GetProcess()) {
		m_cmd->GetProcess()->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(SvnDriver::OnSvnProcessTerminated), NULL, this);
	}
}

void SvnDriver::Update() {
	TRYENTERSVN()

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
	ExecCommand(command);
}

void SvnDriver::CommitWithAuth(const wxString &cmd, const TreeItemInfo &item) {
	TRYENTERSVN();
	wxString command(cmd);
	LoginDialog *dlg = new LoginDialog(wxTheApp->GetTopWindow());
	if(dlg->ShowModal() == wxID_OK){

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

void SvnDriver::Commit() {
	TRYENTERSVN()
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
	for(size_t i=0; i< output.GetCount(); i++){
		text << wxT("# ") << output.Item(i) << wxT("\n");
	}
	
	//Get Log message from user
	SvnDlg *dlg = new SvnDlg(NULL);
	text.Prepend(dlg->GetValue()+wxT("# Svn status:\n"));
	dlg->SetValue(text);
	
	if (dlg->ShowModal() == wxID_OK) {
		comment = dlg->GetValue();
		StripComments(comment);
		command.Clear();
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("commit ") << fileName << wxT(" -m \"") << comment << wxT("\"");
		m_curHandler = new SvnCommitCmdHandler(this, command, item);
		ExecCommand(command);
	}
	dlg->Destroy();
}

void SvnDriver::CommitFile(const wxString &fileName) {
	TRYENTERSVN()
	wxString command, comment;

	//get the comment to enter
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT(" status -q ") << fileName;

	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);
	
	wxString text;
	for(size_t i=0; i< output.GetCount(); i++){
		text << wxT("# ") << output.Item(i) << wxT("\n");
	}
	
	//Get Log message from user
	SvnDlg *dlg = new SvnDlg(NULL);
	text.Prepend(dlg->GetValue()+wxT("# Svn status:\n"));
	dlg->SetValue(text);
	TreeItemInfo dummy;
	if (dlg->ShowModal() == wxID_OK) {
		comment = dlg->GetValue();
		StripComments(comment);
		command.Clear();
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("commit ") << fileName << wxT(" -m \"") << comment << wxT("\"");
		m_curHandler = new SvnCommitCmdHandler(this, command, dummy);
		ExecCommand(command);
	}
	dlg->Destroy();
}

void SvnDriver::UpdateFile(const wxString &fileName)
{
	TRYENTERSVN()

	wxString command;
	wxString file_name(fileName);
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("update ") << file_name ;
	m_curHandler = new SvnDefaultCmdHandler(this, command);
	ExecCommand(command);
}

void SvnDriver::DiffFile(const wxFileName &fileName) {
	TRYENTERSVN()
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
	
	
#ifdef __WXMSW__
	file_name.Prepend(wxT("\""));
	file_name.Append(wxT("\""));
#endif

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("diff ") << file_name;

	m_curHandler = new SvnDiffCmdHandler(this, command);
	ExecCommand(command);
}

void SvnDriver::Diff() {
	TRYENTERSVN()
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

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("diff ") << fileName;

	m_curHandler = new SvnDiffCmdHandler(this, command);
	ExecCommand(command);
}

void SvnDriver::Delete()
{
	TRYENTERSVN();
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
	PrintMessage(wxT("----\n"));
}

void SvnDriver::Revert()
{
	TRYENTERSVN();
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
	PrintMessage(wxT("----\n"));
}

void SvnDriver::RevertFile(const wxFileName &fileName)
{
	TRYENTERSVN();
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
	PrintMessage(wxT("----\n"));
}

void SvnDriver::ChangeLog() {
	TRYENTERSVN()
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

void SvnDriver::Add(const wxFileName &filename)
{
	TRYENTERSVN()
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
	if(filename.IsDir()){
		
		// Execute a sync command to get modified files 
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT("status --xml --non-interactive -q --no-ignore ") << fileName;
		output.Clear();
		ProcUtils::ExecuteCommand(command, output);

		wxArrayString files;
		if(GetFilesList(output, files)){
			SvnAddItemsDlg *dlg = new SvnAddItemsDlg(NULL, files, m_manager->GetConfigTool());
			if(dlg->ShowModal() == wxID_OK){
				//add the selected files
				wxString filesToAdd = dlg->GetFiles();
				if(filesToAdd.IsEmpty() == false){
					command.Clear();
					command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
					command << wxT(" add -N ") << filesToAdd << wxT("") ;
					output.Clear();
					ProcUtils::ExecuteCommand(command, output);
					PrintMessage(output);						
				}
			} else {
				PrintMessage(wxT("Operation Canceled\n"));
			}
			dlg->Destroy();
		}else{
			PrintMessage(wxT("Nothing to be added\n"));
			PrintMessage(wxT("----\n"));
		}

	} else {
		
		// Execute a sync command to get modified files 
		command.Clear();
		command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
		command << wxT(" add -N ") << fileName;
		output.Clear();
		ProcUtils::ExecuteCommand(command, output);
		PrintMessage(output);

	}
}

void SvnDriver::Add() {
	TreeItemInfo item = m_manager->GetSelectedTreeItemInfo(TreeFileExplorer);
	Add(item.m_fileName);
}

bool SvnDriver::GetFilesList(const wxArrayString& output, wxArrayString &files)
{
	wxString str;
	for(size_t i=0; i<output.GetCount(); i++){
		str << output.Item(i);
	}
	
	SvnXmlParser::GetFiles(str, files, SvnXmlParser::StateUnversioned);
	return files.IsEmpty() == false;
}

void SvnDriver::Cleanup() {
	TRYENTERSVN()
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

void SvnDriver::PrintMessage(const wxArrayString &textArr) {
	for(size_t i=0; i<textArr.GetCount(); i++){
		PrintMessage(textArr.Item(i) + wxT("\n"));
	}
}

void SvnDriver::PrintMessage(const wxString &text) {
	wxFlatNotebook *book = m_manager->GetOutputPaneNotebook();
	wxTextCtrl *svnWin (NULL);
	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("SVN")) {
			svnWin = dynamic_cast<wxTextCtrl*>(book->GetPage(i));
			break;
		}
	}
	SelectSvnTab();

	if (svnWin) {
		svnWin->AppendText(text);
		//make the appended line visible
		svnWin->ShowPosition(svnWin->GetLastPosition()); 
	}
}



void SvnDriver::OnSvnProcess(wxCommandEvent &event) {
	m_curHandler->ProcessEvent(event);
}

void SvnDriver::ExecStatusCommand(const wxString &path, wxString &output) {
	wxString command;
	DirSaver ds;
	wxSetWorkingDirectory(path);

	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml -q --non-interactive --no-ignore ");
	
	wxArrayString outputArr;
	ProcUtils::ExecuteCommand(command, outputArr);

	for(size_t i=0; i<outputArr.GetCount(); i++){
		output << outputArr.Item(i);
	}
}

void SvnDriver::Abort() {
	if (m_cmd) {
		m_cmd->Stop();
	}
}
