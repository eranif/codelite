#include "clang_driver.h"
#include "clang_code_completion.h"
#include <wx/regex.h>
#include "file_logger.h"
#include "pluginmanager.h"
#include "includepathlocator.h"
#include "environmentconfig.h"
#include "tags_options_data.h"
#include "ctags_manager.h"
#include <wx/tokenzr.h>
#include "processreaderthread.h"
#include "manager.h"
#include "project.h"
#include "configuration_mapping.h"
#include "procutils.h"
#include "localworkspace.h"
#include "fileextmanager.h"
#include "globals.h"

#define MAX_LINE_TO_SCAN_FOR_HEADERS 500


#ifdef __WXMSW__
static wxString CC_CMD = wxT("cd \"$PROJECT_PATH\" && \"$CLANG\" -cc1 -fcxx-exceptions $ARGS -w -fsyntax-only -include-pch \"$PCH_FILE\" -code-completion-at=$LOCATION \"$SRC_FILE\" 2>&1 1> \"$PCH_FILE.code-completion\"");
#else
static wxString CC_CMD = wxT("cd \"$PROJECT_PATH\" && \"$CLANG\" -cc1 -fexceptions $ARGS -w -fsyntax-only -include-pch \"$PCH_FILE\" -code-completion-at=$LOCATION \"$SRC_FILE\" 2>&1 1> \"$PCH_FILE.code-completion\"");
#endif

BEGIN_EVENT_TABLE(ClangDriver, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  ClangDriver::OnClangProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, ClangDriver::OnClangProcessTerminated)
END_EVENT_TABLE()

static bool wxIsWhitespace(wxChar ch) {
	return ch == wxT(' ') || ch == wxT('\t') || ch == wxT('\r') || ch == wxT('\n');
}

static bool WriteFileLatin1(const wxString &file_name, const wxString &content)
{
	wxFFile file(file_name, wxT("wb"));
	if (file.IsOpened() == false) {
		// Nothing to be done
		return false;
	}

	// write the new content
	file.Write(content);
	file.Close();
	return true;
}

#ifdef __WXMSW__
static wxString MSWGetDefaultClangBinary()
{
	static bool initialized = false;
	static wxString defaultClang;

	if(!initialized) {
		initialized = true;
		// try to locate the default binary
		wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
		defaultClang = exePath.GetPath();
		defaultClang << wxFileName::GetPathSeparator() << wxT("clang++.exe");

		if(!wxFileName::FileExists(defaultClang)) {
			defaultClang.Clear();
		}

		if(defaultClang.IsEmpty() == false) {
			CL_SYSTEM(wxT("Located default clang binary: %s"), defaultClang.c_str());

		} else {
			CL_SYSTEM(wxT("Could not locate default clang binary"));

		}
	}

	return defaultClang;
}
#endif

ClangDriver::ClangDriver()
	: m_process(NULL)
	, m_activationPos(wxNOT_FOUND)
{
	m_pchMakerThread.Start();
}

ClangDriver::~ClangDriver()
{
	m_pchMakerThread.Stop();
}

void ClangDriver::CodeCompletion(IEditor* editor)
{
	if(m_process)
		return;
	
	CL_DEBUG(wxT(" ==========> ClangDriver::CodeCompletion() started <=============="));
	
	if(!editor) {
		CL_WARNING(wxT("ClangDriver::CodeCompletion() called with NULL editor!"));
		return;
	}
	
	// Start clean..
	DoCleanup();
	wxString buffer_unmodified;
	
	wxArrayString removedIncludes;
	wxString current_buffer = editor->GetTextRange(0, DoGetHeaderScanLastPos(editor));
	buffer_unmodified = current_buffer;
	DoRemoveAllIncludeStatements(current_buffer, removedIncludes);
	
	if(m_pchMakerThread.findEntry(editor->GetFileName().GetFullPath(), removedIncludes)) {
		if(GetContext() != CTX_CachePCH) {
			// Calltip / CodeCompletion request
			DoRunCommand(editor);
		} else {
			// context is CTX_CachePCH and we found a valid entry -> skip
			CL_DEBUG(wxT("Valid PCH cache entry already exists for file : %s, request ignored."), editor->GetFileName().GetFullName().c_str());
		}
		return;
	}
	
	// If we reached here, it means that there is no valid entry in the cache for
	// the file, queue a cache request and return
		
	// queue task to creae a new PCH file
	wxString projectPath;
	ClangPchCreateTask *task = new ClangPchCreateTask();
	task->SetClangBinary    ( DoGetClangBinary() );
	task->SetCompilationArgs( DoPrepareCompilationArgs(editor->GetProjectName(), projectPath));
	task->SetFileName       ( editor->GetFileName().GetFullPath());
	task->SetCurrentBuffer  ( buffer_unmodified );
	task->SetProjectPath    ( projectPath );
	m_pchMakerThread.Add( task );
	
	CL_DEBUG(wxT("Queued request to create PCH for file: %s"), editor->GetFileName().GetFullName().c_str());
}

void ClangDriver::OnClangProcessOutput(wxCommandEvent& e)
{
	ProcessEventData *ped = (ProcessEventData*) e.GetClientData();
	if(ped) {
		m_output << ped->GetData();
		delete ped;
	}
	e.Skip();
}

void ClangDriver::OnClangProcessTerminated(wxCommandEvent& e)
{
	CL_DEBUG(wxT("ClangDriver::OnClangProcessTerminated() called !"));
	ProcessEventData *ped = (ProcessEventData*) e.GetClientData();
	delete ped;
	
	OnCodeCompletionCompleted();
}

void ClangDriver::DoRunCommand(IEditor* editor)
{
	wxString clangBinary = DoGetClangBinary();
	
	// Prepare the compilation arguments and store them in m_compilationArgs
	wxString projectPath;
	wxString compilationArgs = DoPrepareCompilationArgs(editor->GetProjectName(), projectPath);

	// Select the pattern
	wxString command;
	command = CC_CMD;

	// wxT("\"$CLANG\" -cc1 -fexceptions $ARGS -w -fsyntax-only -include-pch \"$PCH_FILE\" -code-completion-at=$LOCATION \"$SRC_FILE\"");
	m_ccfilename = DoGetPchOutputFileName(editor->GetFileName().GetFullPath());
	command.Replace(wxT("$CLANG"),          clangBinary);
	command.Replace(wxT("$ARGS"),           compilationArgs);
	command.Replace(wxT("$PCH_FILE"),       m_ccfilename);
	command.Replace(wxT("$PROJECT_PATH"),   projectPath);
	
	wxString completefileName, location;
	wxString filterWord;
	if(!DoProcessBuffer(editor, location, completefileName, filterWord)) {
		DoCleanup();
		return;
	}
	
	command.Replace(wxT("$SRC_FILE"), completefileName);
	command.Replace(wxT("$LOCATION"), location);
	command.Replace(wxT("$FILTER"),   filterWord);
	
	WrapInShell(command);

	// Remove any white spaces from the command
	command.Replace(wxT("\n"), wxT(" "));
	command.Replace(wxT("\r"), wxT(" "));

	// Print the command if logging is enabled
	CL_DEBUG(wxT("ClangDriver::DoRunCommand(): %s"), command.c_str());

	// Launch the process
	m_process = CreateAsyncProcess(this, command, IProcessCreateDefault, editor->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME));
	if(! m_process ) {
		CL_ERROR(wxT("Failed to start process: %s"), command.c_str());
		DoCleanup();
		return ;
	}
	m_filename = editor->GetFileName().GetFullPath();
}

void ClangDriver::DoCleanup()
{
	if(m_process) {
		delete m_process;
	}
	
	m_process = NULL;
	m_ccfilename.Clear();
	// remove the temporary file
	if(m_tmpfile.IsEmpty() == false) {
#ifndef __WXMSW__
		::unlink( m_tmpfile.mb_str(wxConvUTF8).data() );
#endif
		wxRemoveFile( m_tmpfile );
		m_tmpfile.Clear();
	}
	m_filename.Clear();
	m_output.Clear();
}

void ClangDriver::OnCodeCompletionCompleted()
{
	// clang output is stored in m_output
	// "$PCH_FILE.code-completion"
	wxString output;
	wxString filename;
	filename << m_ccfilename << wxT(".code-completion");
	wxFFile fp(filename, wxT("r"));
	if(fp.IsOpened()) {
		fp.ReadAll( &output );
	}
	fp.Close();
	wxRemoveFile(filename);
	
	CL_DEBUG(wxT("ClangDriver::OnCodeCompletionCompleted() called"));
	CL_DEBUG1(wxT("ClangDriver::OnCodeCompletionCompleted():\n[%s]"), output.c_str());

	DoCleanup();
	ClangCodeCompletion::Instance()->DoParseOutput(output);
	CL_DEBUG(wxT(" ==========> ClangDriver::CodeCompletion() ENDED <=============="));
}

wxString ClangDriver::DoGetPchHeaderFile(const wxString& filename)
{
	wxFileName fn(filename);
	wxString name;
	name << ClangPCHCache::GetCacheDirectory()
	     << wxFileName::GetPathSeparator()
	     << fn.GetName()
		 << wxT("_") << fn.GetExt()
	     << wxT("__H__.h");
	return name;
}

wxString ClangDriver::DoGetPchOutputFileName(const wxString& filename)
{
	return DoGetPchHeaderFile(filename) + wxT(".pch");
}

void ClangDriver::Abort()
{
	m_activationPos = wxNOT_FOUND;
	DoCleanup();
}

void ClangDriver::DoRemoveAllIncludeStatements(wxString& buffer, wxArrayString &includesRemoved)
{
	static wxRegEx reIncludeFile(wxT("^[ \\t]*#[ \\t]*include[ \\t]*[\"\\<]{1}([a-zA-Z0-9_/\\\\: \\.\\+\\-]+)[\"\\>]{1}"));
	wxArrayString lines = wxStringTokenize(buffer, wxT("\n"), wxTOKEN_RET_DELIMS);
	CL_DEBUG(wxT("Calling DoRemoveAllIncludeStatements()"));
	buffer.Clear();
	for(size_t i=0; i<lines.GetCount(); i++) {

		if(i >= 300) break;

		wxString curline = lines.Item(i);
		wxString trimLine = lines.Item(i);
		trimLine.Trim().Trim(false);

		// Optimize: dont use regex unless we are sure that this is a preprocessor line
		if(!trimLine.IsEmpty() && trimLine.GetChar(0) == wxT('#') && reIncludeFile.Matches(curline)) {

			wxString filename = reIncludeFile.GetMatch(curline, 1);
			wxFileName fn(filename);
			includesRemoved.Add(fn.GetFullPath());

			reIncludeFile.ReplaceAll(&curline, wxT(""));
			buffer << curline;

		} else {
			buffer << curline;
		}
	}

	for(size_t i=0; i<includesRemoved.GetCount(); i++) {
		CL_DEBUG(wxT("Stripping include: %s"), includesRemoved.Item(i).c_str());
	}
	CL_DEBUG(wxT("Calling DoRemoveAllIncludeStatements()- ENDED"));
}

wxString ClangDriver::DoPrepareCompilationArgs(const wxString& projectName, wxString &projectPath)
{
	wxString compilationArgs;
	wxArrayString args;
	wxString      errMsg;
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	if(!matrix) {
		return wxT("");
	}

	wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

	// Now that we got the selected workspace configuration, extract the related project configuration
	ProjectPtr proj =  WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if(!proj) {
		return wxT("");
	}

	wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, proj->GetName());
	BuildConfigPtr dependProjbldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), projectSelConf);

	// no build config?
	if(!dependProjbldConf)
		return wxT("");
	
	projectPath = proj->GetFileName().GetPath();
	
	// for non custom projects, take the settings from the build configuration
	if(!dependProjbldConf->IsCustomBuild()) {

		// Get the include paths and add them
		wxString projectIncludePaths = dependProjbldConf->GetIncludePath();
		wxArrayString projectIncludePathsArr = wxStringTokenize(projectIncludePaths, wxT(";"), wxTOKEN_STRTOK);
		for(size_t i=0; i<projectIncludePathsArr.GetCount(); i++) {
			args.Add( wxString::Format(wxT("-I%s"), projectIncludePathsArr[i].c_str()) );
		}

		// get the compiler options and add them
		wxString projectCompileOptions = dependProjbldConf->GetCompileOptions();
		wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);
		for(size_t i=0; i<projectCompileOptionsArr.GetCount(); i++) {

			wxString cmpOption (projectCompileOptionsArr.Item(i));
			cmpOption.Trim().Trim(false);

			// expand backticks, if the option is not a backtick the value remains
			// unchanged
			cmpOption = DoExpandBacktick(cmpOption);
			args.Add( cmpOption );
		}

		// get the compiler preprocessor and add them as well
		wxString projectPreps = dependProjbldConf->GetPreprocessor();
		wxArrayString projectPrepsArr = wxStringTokenize(projectPreps, wxT(";"), wxTOKEN_STRTOK);
		for(size_t i=0; i<projectPrepsArr.GetCount(); i++) {
			args.Add( wxString::Format(wxT("-D%s"), projectPrepsArr[i].c_str()) );
		}
	}
	
	const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();

	///////////////////////////////////////////////////////////////////////
	// add global clang include paths
	wxString strGlobalIncludes = options.GetClangSearchPaths();
	wxArrayString globalIncludes = wxStringTokenize(strGlobalIncludes, wxT("\n\r"), wxTOKEN_STRTOK);
	for(size_t i=0; i<globalIncludes.GetCount(); i++) {
		compilationArgs << wxT(" -I\"") << globalIncludes.Item(i).Trim().Trim(false) << wxT("\" ");
	}
	
	///////////////////////////////////////////////////////////////////////
	// Workspace setting additional flags 
	///////////////////////////////////////////////////////////////////////
	
	// Include paths
	wxArrayString workspaceIncls, dummy;
	LocalWorkspaceST::Get()->GetParserPaths(workspaceIncls, dummy);
	for(size_t i=0; i<workspaceIncls.GetCount(); i++) {
		compilationArgs << wxT(" -I\"") << workspaceIncls.Item(i).Trim().Trim(false) << wxT("\" ");
	}
	
	// Options
	wxString strWorkspaceCmpOptions;
	LocalWorkspaceST::Get()->GetParserOptions(strWorkspaceCmpOptions);
	
	wxArrayString workspaceCmpOptions = wxStringTokenize(strWorkspaceCmpOptions, wxT("\n\r"), wxTOKEN_STRTOK);
	for(size_t i=0; i<workspaceCmpOptions.GetCount(); i++) {
		compilationArgs << DoExpandBacktick(workspaceCmpOptions.Item(i).Trim().Trim(false)) << wxT(" ");
	}
	
	// Macros
	wxString strWorkspaceMacros;
	LocalWorkspaceST::Get()->GetParserMacros(strWorkspaceMacros);
	wxArrayString workspaceMacros = wxStringTokenize(strWorkspaceMacros, wxT("\n\r"), wxTOKEN_STRTOK);
	for(size_t i=0; i<workspaceMacros.GetCount(); i++) {
		compilationArgs << wxT(" -D") << workspaceMacros.Item(i).Trim().Trim(false) << wxT(" ");
	}

	for(size_t i=0; i<args.size(); i++) {
		compilationArgs << wxT(" ") << args.Item(i);
	}

	// Remove some of the flags which are known to cause problems to clang
	compilationArgs.Replace(wxT("-fno-strict-aliasing"), wxT(""));
	compilationArgs.Replace(wxT("-mthreads"),            wxT(""));
	compilationArgs.Replace(wxT("-pipe"),                wxT(""));
	compilationArgs.Replace(wxT("-fmessage-length=0"),   wxT(""));
	compilationArgs.Replace(wxT("-fPIC"),                wxT(""));
	return compilationArgs;
}

wxString ClangDriver::DoExpandBacktick(const wxString& backtick)
{
	wxString tmp;
	wxString cmpOption = backtick;
	// Expand backticks / $(shell ...) syntax supported by codelite
	if(cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
		cmpOption = tmp;
		tmp.Clear();
		if(cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) {
			cmpOption = tmp;
		}
		
		if(m_backticks.find(cmpOption) == m_backticks.end()) {
			
			// Expand the backticks into their value
			wxString expandedValue = wxShellExec(cmpOption);
			m_backticks[cmpOption] = expandedValue;
			cmpOption = expandedValue;
			
		} else {
			cmpOption = m_backticks.find(cmpOption)->second;
		}
	}
	return cmpOption;
}

bool ClangDriver::DoProcessBuffer(IEditor* editor, wxString &location, wxString &completefileName, wxString &filterWord)
{
	// First, we need to find the currently active workspace configuration
	wxString currentBuffer = editor->GetTextRange(0, editor->GetCurrentPosition());
	if(currentBuffer.IsEmpty()) {
		return false;
	}
	
	filterWord.Clear();
	
	// Move backward until we found our -> or :: or .
	for(size_t i=currentBuffer.Length() - 1; i>0; i--) {
		// Context word complete and we found a whitespace - break the search
		if(GetContext() == CTX_WordCompletion && wxIsWhitespace(currentBuffer.at(i))) {
			break;
		}
		
		if(currentBuffer.EndsWith(wxT("->")) || currentBuffer.EndsWith(wxT(".")) || currentBuffer.EndsWith(wxT("::"))) {
			break;
		} else {
			filterWord.Prepend(currentBuffer.GetChar(i));
			currentBuffer.Truncate(currentBuffer.Length() - 1);
		}
	}

	// Get the current line's starting pos
	int lineStartPos = editor->PosFromLine( editor->GetCurrentLine() );
	int column       = editor->GetCurrentPosition() - lineStartPos  + 1;
	int line         = editor->GetCurrentLine() + 1;
	int where        = currentBuffer.Find(wxT('\n'), true);
	
	if(where != wxNOT_FOUND) {
		CL_DEBUG1(wxT("clang code completion has been invoked for the line: %s"), currentBuffer.Mid(where).c_str());
	}
	
	column -= (int)filterWord.Length();

	// Create temp file
	m_tmpfile.clear();
	m_tmpfile << editor->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME) << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");
	
	CL_DEBUG(wxT("Preparing input file for clang..."));
	// Prepare the file name
	FileExtManager::FileType type = FileExtManager::GetType(editor->GetFileName().GetFullPath());
	if(type == FileExtManager::TypeSourceC || type == FileExtManager::TypeSourceCpp) {

		if(!WriteFileLatin1(m_tmpfile, currentBuffer)) {
			CL_ERROR(wxT("Failed to write temp file: %s"), m_tmpfile.c_str());
			return false;
		}

		completefileName << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");
		location << completefileName << wxT(":") << line << wxT(":") << column << wxT(" ");

	} else {
		wxString implFile;
		implFile << wxT("#include <") << editor->GetFileName().GetFullName() << wxT(">\n");
		if(!WriteFileLatin1(m_tmpfile, implFile)) {
			CL_ERROR(wxT("Failed to write temp file: %s"), implFile.c_str());
			return false;
		}

		completefileName << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");
		location << editor->GetFileName().GetFullPath() << wxT(":") << line << wxT(":") << column;
	}
	m_activationPos = lineStartPos + column - 1;
	return true;
}

int ClangDriver::DoGetHeaderScanLastPos(IEditor* editor)
{
	// determine how many lines we should scan for searching for include files
	int last_line = editor->LineFromPos(editor->GetLength());
	int num_lines_to_scan = last_line > MAX_LINE_TO_SCAN_FOR_HEADERS ? MAX_LINE_TO_SCAN_FOR_HEADERS : last_line;
	return editor->PosFromLine(num_lines_to_scan);
}

wxString ClangDriver::DoGetClangBinary()
{
	const TagsOptionsData &options = TagsManagerST::Get()->GetCtagsOptions();
	// Obtain the clang binary name
	wxString clangBinary = options.GetClangBinary();
	clangBinary.Trim().Trim(false);

	// Determine which clang binary we use
	if(clangBinary.IsEmpty()) {
#ifdef __WXMSW__
		clangBinary = MSWGetDefaultClangBinary();
#else
		clangBinary = wxT("clang");
#endif
	}
	return clangBinary;
}

void ClangDriver::ClearCache()
{
	m_pchMakerThread.ClearCache();
}

bool ClangDriver::IsCacheEmpty()
{
	return m_pchMakerThread.IsCacheEmpty();
}
