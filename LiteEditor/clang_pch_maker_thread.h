#ifndef CLANGPCHMAKERTHREAD_H
#define CLANGPCHMAKERTHREAD_H

#include "worker_thread.h" // Base class: ThreadRequest
#include "clangpch_cache.h"

extern const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED ;
extern const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED   ;

class ClangPchCreateTask : public ThreadRequest
{
public:
	enum {
	    CreatePch
	};

private:
	wxString      current_buffer;  // Input
	wxString      file_name;       // Input
	int           task_type;       // Input
	wxString      compilationArgs; // Input
	wxString      clang_binary;    // Input
	wxString      project_path;    // Input
	wxArrayString pchHeaders;      // Internally used
	wxArrayString includesRemoved; // Internally used
	wxArrayString projectPaths;

public:
	ClangPchCreateTask()
		: task_type(CreatePch)
	{}

	virtual ~ClangPchCreateTask() {}

	wxArrayString& GetProjectPaths() {
		return projectPaths;
	}
	
	void SetProjectPath(const wxString& project_path) {
		this->project_path = project_path.c_str();
	}
	
	const wxString& GetProjectPath() const {
		return project_path;
	}
	void SetCurrentBuffer(const wxString& current_buffer) {
		this->current_buffer = current_buffer.c_str();
	}
	void SetFileName(const wxString& file_name) {
		this->file_name = file_name.c_str();
	}
	const wxString& GetCurrentBuffer() const {
		return current_buffer;
	}
	const wxString& GetFileName() const {
		return file_name;
	}
	void SetTaskType(int task_type) {
		this->task_type = task_type;
	}
	int GetTaskType() const {
		return task_type;
	}
	void SetClangBinary(const wxString& clang_binary) {
		this->clang_binary = clang_binary.c_str();
	}
	void SetCompilationArgs(const wxString& compilationArgs) {
		this->compilationArgs = compilationArgs.c_str();
	}
	const wxString& GetClangBinary() const {
		return clang_binary;
	}
	const wxString& GetCompilationArgs() const {
		return compilationArgs;
	}
	wxArrayString& GetPchHeaders() {
		return pchHeaders;
	}
	wxArrayString& GetIncludesRemoved() {
		return includesRemoved;
	}
};

class ClangPchMakerThread : public WorkerThread
{
protected:
	wxCriticalSection m_cs;
	ClangPCHCache     m_cache;

public:
	ClangPchMakerThread();
	virtual ~ClangPchMakerThread();

protected:
	void     DoCreatePch(ClangPchCreateTask *task);
	void     DoRemoveAllIncludeStatements(ClangPchCreateTask *task, wxString &buffer);
	wxString DoGetPchHeaderFile(const wxString &filename);
	wxString DoGetPchOutputFileName(const wxString &filename);
	void     DoFilterIncludeFilesFromPP(ClangPchCreateTask *task);
	bool     ShouldInclude(ClangPchCreateTask *task, const wxString& header, std::set<wxString> &includesMatched);
	void     DoPrepareCommand(wxString &command);
	void     DoCacheResult(ClangPchCreateTask *task, const wxArrayString &output);

public:
	virtual void ProcessRequest(ThreadRequest* request);
	bool         findEntry(const wxString &filename, const wxArrayString &includes);
	void         ClearCache();
	bool         IsCacheEmpty();
};

#endif // CLANGPCHMAKERTHREAD_H
