#include "file_logger.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <sys/time.h>
#include <wx/log.h>

static FileLogger theLogger;
static bool initialized = false;

FileLogger::FileLogger()
	: m_verbosity(FileLogger::Error)
{
}

FileLogger::~FileLogger()
{
	wxLogNull nl;
	m_fp.Close();
}

void FileLogger::AddLogLine(const wxString &msg, int verbosity)
{
	if(m_verbosity >= verbosity && m_fp.IsOpened()) {
		wxString formattedMsg;
		
		timeval tim;
		gettimeofday(&tim, NULL);
		int ms = (int)tim.tv_usec / 1000.0;
		
		wxString msStr = wxString::Format(wxT("%03d"), ms);
		
		formattedMsg << wxT("[ ") 
					 << wxDateTime::Now().FormatISOTime()
					 << wxT(":")
					 << msStr;
					 
		switch(verbosity) {
		case System:
			formattedMsg << wxT(" SYS ] ");
			break;
			
		case Error:
			formattedMsg << wxT(" ERR ] ");
			break;
			
		case Warning:
			formattedMsg << wxT(" WRN ] ");
			break;
			
		case Dbg:
			formattedMsg << wxT(" DBG ] ");
			break;
			
		case Developer:
			formattedMsg << wxT(" DVL ] ");
			break;

		}
		
		
		formattedMsg << msg;
		formattedMsg.Trim().Trim(false);
		formattedMsg << wxT("\n");
				
		m_fp.Write(formattedMsg);
		m_fp.Flush();
	}
}

FileLogger* FileLogger::Get()
{
	if(!initialized) {
		wxString filename;
		filename << wxStandardPaths::Get().GetUserDataDir() << wxFileName::GetPathSeparator() << wxT("codelite.log");
		theLogger.m_fp.Open(filename, wxT("a+"));
		initialized = true;
	}
	return &theLogger;
}

void FileLogger::SetVerbosity(int level)
{
	CL_SYSTEM(wxT("Log verbosity is now set to %s"), FileLogger::GetVerbosityAsString(level).c_str());
	m_verbosity = level;
}

int FileLogger::GetVerbosityAsNumber(const wxString& verbosity)
{
	if (verbosity == wxT("Debug")) {
		return FileLogger::Dbg;
		
	} else if(verbosity == wxT("Error")) {
		return FileLogger::Error;
		
	} else if(verbosity == wxT("Warning")) {
		return FileLogger::Warning;
		
	} else if(verbosity == wxT("System")) {
		return FileLogger::System;
		
	} else if(verbosity == wxT("Developer")) {
		return FileLogger::Developer;
		
	} else {
		return FileLogger::Error;
	}
}

wxString FileLogger::GetVerbosityAsString(int verbosity)
{
	switch(verbosity) {
	case FileLogger::Dbg:
		return wxT("Debug");
		
	case FileLogger::Error:
		return wxT("Error");

	case FileLogger::Warning:
		return wxT("Warning");
		
	case FileLogger::Developer:
		return wxT("Developer");
		
	default:
		return wxT("Error");
	}
}

void FileLogger::SetVerbosity(const wxString& verbosity)
{
	SetVerbosity( FileLogger::GetVerbosityAsNumber(verbosity) );
}
