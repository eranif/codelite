#include <wx/app.h>
#include "clang_pch_maker_thread.h"
#include <wx/thread.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"
#include "fileextmanager.h"

#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
    static wxString PRE_PROCESS_CMD = wxT("cd \"$PROJECT_PATH\" && \"$CLANG\" -cc1 -fcxx-exceptions $ARGS -w \"$SRC_FILE\" -E 1> \"$PP_OUTPUT_FILE\" 2>&1");
    static wxString PCH_CMD         = wxT("cd \"$PROJECT_PATH\" && \"$CLANG\" -cc1 -fcxx-exceptions -x c++-header $ARGS -w \"$SRC_FILE\" -emit-pch -o \"$PCH_FILE\" 1> \"$PCH_FILE.output\" 2>&1");
#else
    static wxString PRE_PROCESS_CMD = wxT("cd \"$PROJECT_PATH\" && \"$CLANG\" -cc1 -fexceptions $ARGS -w \"$SRC_FILE\" -E 1> \"$PP_OUTPUT_FILE\" 2>&1");
    static wxString PCH_CMD         = wxT("cd \"$PROJECT_PATH\" && \"$CLANG\" -cc1 -fexceptions -x c++-header $ARGS -w \"$SRC_FILE\" -emit-pch -o \"$PCH_FILE\" 1> \"$PCH_FILE.output\" 2>&1");
#endif

const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED = XRCID("clang_pch_cache_started");
const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED   = XRCID("clang_pch_cache_ended");

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

ClangPchMakerThread::ClangPchMakerThread()
{
}

ClangPchMakerThread::~ClangPchMakerThread()
{
}

void ClangPchMakerThread::ProcessRequest(ThreadRequest* request)
{
	ClangPchCreateTask *task = dynamic_cast<ClangPchCreateTask*>( request );
	if( !task ) {
		return;
	}
	
	switch(task->GetTaskType()) {
	case ClangPchCreateTask::CreatePch:
		DoCreatePch(task);
		break;
		
	default:
		break;
	}
}

bool ClangPchMakerThread::findEntry(const wxString& filename, const wxArrayString &includes)
{
	wxCriticalSectionLocker locker(m_cs);
	const ClangPCHEntry &entry = m_cache.GetPCH(filename);
	bool isOk = entry.IsValid() && !entry.NeedRegenration(includes);
	return isOk;
}

void ClangPchMakerThread::DoCreatePch(ClangPchCreateTask *task)
{
	// first, pre-process the file
	wxString      command;
	wxString      ppOutputFile;
	wxString      currentBuffer;
	wxFileName    fileName;
	wxArrayString outputArr;
	
	// Send start event
	wxCommandEvent e(wxEVT_CLANG_PCH_CACHE_STARTED);
	e.SetClientData(new wxString(task->GetFileName().c_str()));
	wxTheApp->AddPendingEvent(e);
	
	CL_DEBUG(wxT(" ==========> [ ClangPchMakerThread ] PCH creation started <=============="));
	
	fileName = task->GetFileName();
	
	////////////////////////////////////////////////////////////////////////
	// Pre Processing
	////////////////////////////////////////////////////////////////////////
	
	// set the Pre Processing file command pattern
	command = PRE_PROCESS_CMD;
	ppOutputFile << DoGetPchHeaderFile(fileName.GetFullName()) 
				 << wxT(".1");
	
	// wxT("\"$CLANG\" -cc1 -fcxx-exceptions $ARGS -w \"$SRC_FILE\" -E 1> \"$PP_OUTPUT_FILE\" 2>&1");
	command.Replace(wxT("$CLANG"),          task->GetClangBinary());
	command.Replace(wxT("$ARGS"),           task->GetCompilationArgs());
	command.Replace(wxT("$SRC_FILE"),       fileName.GetFullPath());
	command.Replace(wxT("$PP_OUTPUT_FILE"), ppOutputFile);
	command.Replace(wxT("$PROJECT_PATH"),   task->GetProjectPath());
	
	// Wrap the command in a shell, since the output is quite large and it is better to use
	// the shell redirect rather than reading it directly into our process
	DoPrepareCommand(command);
	
	// Execute the command
	CL_DEBUG(wxT("Executing: %s"), command.c_str());
	ProcUtils::SafeExecuteCommand(command, outputArr);
	
	//////////////////////////////////////////////////////////////
	// Create PCH stage.
	// Prepare a fake header file and pre-compile it
	//////////////////////////////////////////////////////////////
	
	// Extract all include statements from the input file
	currentBuffer = task->GetCurrentBuffer();
	
	// go over the input buffer and strip any #include <..> statement from it
	// fills the member task->includesRemoved with all the includes
	// stripped and modify the currentBuffer replacing every '#include' line with 
	// a NEWLINE
	DoRemoveAllIncludeStatements(task, currentBuffer);
	
	// Go over the output from the PP stage and match any include file that appears
	// both in the task->inculdesRemoved && in the PP file
	// store them in task->pchHeaders
	// In addition, this method also writes down the file 'filename__H__.h'
	// which will use us as the PCH input file
	DoFilterIncludeFilesFromPP(task);
	
	// wxT("\"$CLANG\" -cc1 -fcxx-exceptions -x c++-header $ARGS -w \"$SRC_FILE\" -emit-pch -o \"$PCH_FILE\"");
	command = PCH_CMD;
	command.Replace(wxT("$CLANG"),          task->GetClangBinary());
	command.Replace(wxT("$ARGS"),           task->GetCompilationArgs());
	command.Replace(wxT("$PCH_FILE"),       DoGetPchOutputFileName(fileName.GetFullName()));
	command.Replace(wxT("$SRC_FILE"),       DoGetPchHeaderFile(fileName.GetFullName()));
	command.Replace(wxT("$PROJECT_PATH"),   task->GetProjectPath());
	
	DoPrepareCommand(command);
	
	// Execute the command
	CL_DEBUG(wxT("Executing: %s"), command.c_str());
	ProcUtils::SafeExecuteCommand(command, outputArr);
	DoCacheResult(task, outputArr);
	
	// Send start event
	wxCommandEvent eEnd(wxEVT_CLANG_PCH_CACHE_ENDED);
	wxTheApp->AddPendingEvent(eEnd);
}

void ClangPchMakerThread::DoCacheResult(ClangPchCreateTask *task, const wxArrayString &output)
{
	// read the output...
	wxString pchfilename = DoGetPchOutputFileName(task->GetFileName());
	pchfilename.Append(wxT(".output"));
	wxString strOutput;
	wxFFile of(pchfilename, wxT("rb"));
	if(of.IsOpened()) {
		of.ReadAll( &strOutput );
		of.Close();
	}
	
	wxRemoveFile(pchfilename);
	CL_DEBUG1(wxT("[ ClangPchMakerThread ] %s"), strOutput.c_str());
	
	// Cache the result (incase of no errors were made)
	if(strOutput.Find(wxT("error :")) != wxNOT_FOUND) {
		// failed to create the PCH
		CL_DEBUG(wxT(" ==========> [ ClangPchMakerThread ] PCH creation ended with error <=============="));
		return;
	}
	
	// Cache it
	{
		wxCriticalSectionLocker locker(m_cs);
		m_cache.AddPCH( task->GetFileName(), 
						DoGetPchOutputFileName(task->GetFileName()), 
						task->GetIncludesRemoved(), 
						task->GetPchHeaders());
		CL_DEBUG(wxT("caching PCH file: %s for file %s"), DoGetPchOutputFileName(task->GetFileName()).c_str(), task->GetFileName().c_str());
		wxRemoveFile(DoGetPchHeaderFile(task->GetFileName()));
		CL_DEBUG(wxT(" ==========> [ ClangPchMakerThread ] PCH creation ended successfully <=============="));
	}
}

void ClangPchMakerThread::DoRemoveAllIncludeStatements(ClangPchCreateTask *task, wxString &buffer)
{
	static wxRegEx reIncludeFile(wxT("^[ \\t]*#[ \\t]*include[ \\t]*[\"\\<]{1}([a-zA-Z0-9_/\\\\: \\.\\+\\-]+)[\"\\>]{1}"));
	
	task->GetIncludesRemoved().Clear();
	wxArrayString lines = wxStringTokenize(buffer, wxT("\n"), wxTOKEN_RET_DELIMS);
	CL_DEBUG(wxT("Calling DoRemoveAllIncludeStatements()"));
	buffer.Clear();
	
	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString curline  = lines.Item(i);
		wxString trimLine = lines.Item(i);
		trimLine.Trim().Trim(false);

		// Optimize: dont use regex unless we are sure that this is a preprocessor line
		if(!trimLine.IsEmpty() && trimLine.GetChar(0) == wxT('#') && reIncludeFile.Matches(curline)) {

			wxString filename = reIncludeFile.GetMatch(curline, 1);
			wxFileName fn(filename);
			task->GetIncludesRemoved().Add(fn.GetFullPath().c_str());

			reIncludeFile.ReplaceAll(&curline, wxT(""));
			buffer << curline;

		} else {
			buffer << curline;
		}
	}

	for(size_t i=0; i<task->GetIncludesRemoved().GetCount(); i++) {
		CL_DEBUG(wxT("Stripping include: %s"), task->GetIncludesRemoved().Item(i).c_str());
	}
	CL_DEBUG(wxT("Calling DoRemoveAllIncludeStatements()- ENDED"));
}

wxString ClangPchMakerThread::DoGetPchHeaderFile(const wxString& filename)
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

wxString ClangPchMakerThread::DoGetPchOutputFileName(const wxString& filename)
{
	return DoGetPchHeaderFile(filename) + wxT(".pch");
}

void ClangPchMakerThread::DoFilterIncludeFilesFromPP(ClangPchCreateTask *task)
{
	task->GetPchHeaders().Clear();
	wxString tmpfilename = DoGetPchHeaderFile(task->GetFileName());
	tmpfilename << wxT(".1");

	wxString content;
	wxFileName filename(task->GetFileName());
	wxString basedir = filename.GetPath();

	wxFFile fp(tmpfilename, wxT("rb"));
	if(fp.IsOpened()) {
		fp.ReadAll(&content);
	}
	fp.Close();
	wxRemoveFile(tmpfilename);

	wxArrayString includes;
	wxArrayString lines = wxStringTokenize(content, wxT("\n\r"), wxTOKEN_STRTOK);
	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString curline = lines.Item(i);

		curline.Trim().Trim(false);
		if(curline.IsEmpty())
			continue;

		if(curline.GetChar(0) != wxT('#'))
			continue;

		// an example for a valid line:
		// # 330 "c:\\Users\\eran\\software\\mingw-4.4.1\\include/stdio.h"

		// Trim the dots
		static wxRegEx reIncludeFile(wxT("^#[ \\t]*[0-9]+[ \\t]*\"([a-zA-Z0-9_/\\\\: \\.\\+\\-]+)\""));

		if(!reIncludeFile.Matches(curline))
			continue;

		curline = reIncludeFile.GetMatch(curline, 1);
		curline.Replace(wxT("\\\\"), wxT("\\"));

		bool includeIt = true;
		wxFileName fn(curline);
		switch(FileExtManager::GetType(fn.GetFullName())) {
		case FileExtManager::TypeSourceC:
		case FileExtManager::TypeSourceCpp:
			if(fn.GetFullName() == filename.GetFullName()) {
				includeIt = false;
			}
			break;
		default:
			break;
		}

		if(!includeIt) continue;

		fn.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG, basedir);
		wxString fullpath = fn.GetFullPath();

		if(includes.Index(fullpath) == wxNOT_FOUND) {
			includes.Add(fullpath);
		}
	}

	wxString pchHeaderFile = DoGetPchHeaderFile(filename.GetFullPath());
	wxString pchHeaderFileContent;
	
	for(size_t i=0; i<includes.GetCount(); i++) {
		if(ShouldInclude(task, includes.Item(i))) {
			task->GetPchHeaders().Add(includes.Item(i));
			pchHeaderFileContent << wxT("#include \"") << includes.Item(i) << wxT("\"\n");
		}
	}

	WriteFileLatin1(pchHeaderFile, pchHeaderFileContent);
}

bool ClangPchMakerThread::ShouldInclude(ClangPchCreateTask *task, const wxString& header)
{
	wxFileName fnHeader(header);
	// Header is in the form of full path
	for(size_t i=0; i<task->GetIncludesRemoved().GetCount(); i++) {
		wxFileName fn(task->GetIncludesRemoved().Item(i));

		if(fn.GetFullName() == fnHeader.GetFullName() && header.EndsWith(task->GetIncludesRemoved().Item(i))) {
			return true;
		}
	}
	return false;
}

void ClangPchMakerThread::DoPrepareCommand(wxString& command)
{
	WrapInShell(command);
	
	// Remove any white spaces from the command
	command.Replace(wxT("\n"), wxT(" "));
	command.Replace(wxT("\r"), wxT(" "));
}

void ClangPchMakerThread::ClearCache()
{
	wxCriticalSectionLocker locker(m_cs);
	m_cache.Clear();
}

bool ClangPchMakerThread::IsCacheEmpty()
{
	wxCriticalSectionLocker locker(m_cs);
	return m_cache.IsEmpty();
}
