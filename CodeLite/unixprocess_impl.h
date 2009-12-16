#ifndef __unixprocessimpl__
#define __unixprocessimpl__

#if defined(__WXMAC__)||defined(__WXGTK__)
#include "asyncprocess.h"
#include "processreaderthread.h"

class UnixProcessImpl : public IProcess
{
	int                  m_readHandle;
	int                  m_writeHandle;
	ProcessReaderThread *m_thr;

private:
	void StartReaderThread();

public:
	UnixProcessImpl(wxEvtHandler *parent);
	virtual ~UnixProcessImpl();

	static IProcess *Execute(wxEvtHandler *parent, const wxString &cmd, const wxString &workingDirectory = wxEmptyString);

	void SetReadHandle(const int& readHandle) {
		this->m_readHandle = readHandle;
	}
	void SetWriteHandler(const int& writeHandler) {
		this->m_writeHandle = writeHandler;
	}
	const int& GetReadHandle() const {
		return m_readHandle;
	}
	const int& GetWriteHandle() const {
		return m_writeHandle;
	}

public:
	virtual void Cleanup();
	virtual bool IsAlive();
	virtual bool Read(wxString& buff);
	virtual bool Write(const wxString& buff);
	virtual void Terminate();
	
};
#endif //#if defined(__WXMAC )||defined(__WXGTK__)
#endif // __unixprocessimpl__
