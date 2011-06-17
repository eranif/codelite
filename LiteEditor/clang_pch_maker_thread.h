#ifndef CLANGPCHMAKERTHREAD_H
#define CLANGPCHMAKERTHREAD_H

#include "worker_thread.h" // Base class: ThreadRequest
#include "clangpch_cache.h"

class ClangPchCreateTask : public ThreadRequest
{
public:
	enum {
		CreatePch
	};

private:
	wxString  current_buffer;
	int       position;
	wxString  file_name;
	int       task_type;
	wxString  compilationArgs;

public:
	ClangPchCreateTask()
		: position(wxNOT_FOUND)
		, task_type(CreatePch)
	{}

	virtual ~ClangPchCreateTask() {}

	void SetCurrentBuffer(const wxString& current_buffer) {
		this->current_buffer = current_buffer;
	}
	void SetFileName(const wxString& file_name) {
		this->file_name = file_name;
	}
	void SetPosition(int position) {
		this->position = position;
	}
	const wxString& GetCurrentBuffer() const {
		return current_buffer;
	}
	const wxString& GetFileName() const {
		return file_name;
	}
	int GetPosition() const {
		return position;
	}
	void SetTaskType(int task_type) {
		this->task_type = task_type;
	}
	int GetTaskType() const {
		return task_type;
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
	void DoCreatePch(ClangPchCreateTask *task);

public:
	virtual void ProcessRequest(ThreadRequest* request);
	bool         findEntry(const wxString &filename, const wxArrayString &includes);
};

#endif // CLANGPCHMAKERTHREAD_H
