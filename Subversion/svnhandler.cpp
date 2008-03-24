#include "svnhandler.h"
#include "async_executable_cmd.h"
#include "svndriver.h"
#include "wx/tokenzr.h"
#include "subversion.h"
#include "svnxmlparser.h"

void SvnDefaultCmdHandler::ProcessEvent(wxCommandEvent &event)
{
	wxString text(event.GetString());
	if (event.GetEventType() == wxEVT_ASYNC_PROC_STARTED) {
		text = wxEmptyString;
	} else if (event.GetEventType() == wxEVT_ASYNC_PROC_ENDED) {
		text = wxT("----\n");
	}

	text = text.Trim();
	text = text.Trim(false);

	if (text != wxT(".")) {
		if (m_needLf) {
			m_needLf = false;
			//previous text entered was '.'
			//so we probably will need a new Line Feed
			text.Prepend(wxT("\n"));
		}
		text << wxT("\n");
	} else {
		m_needLf = true;
	}

	m_svnDriver->PrintMessage(text);
	if (IsVerificationNeeded(text)) {
		wxString message, answer;
		message << wxT("SVN: Error validating server certificate\n")
		<< wxT("You can choose to accept the server certification permanently by clicking 'Yes',\n")
		<< wxT("Or you can reject it by clicking 'No'");
		int res = wxMessageBox(message, wxT("Accept Server Certification?"), wxYES_NO | wxICON_QUESTION | wxCANCEL);
		if (res == wxYES) {
			answer = wxT("p");
		} else if (res == wxNO) {
			answer = wxT("R");
		} else {
			answer = wxEmptyString;
		}

		if (answer.IsEmpty() == false) {
			m_svnDriver->Svn()->GetProcess()->Write(answer + wxT("\n"));
		}
	}
#ifdef __WXMSW__
	if (IsAuthFailed(text)) {
		m_svnDriver->PrintMessage(wxT("Authentication requires\nplease wait for the login dialog ...\n"));
		m_svnDriver->SetCommitWithPassword(true);
		m_svnDriver->Svn()->GetProcess()->Terminate();
	}
#else
	if (IsAuthFailed(text)) {
		wxString password = wxGetPasswordFromUser(wxT("SVN Password:"), wxT("Password:"));
		if (password.IsEmpty() == false) {
			m_svnDriver->Svn()->GetProcess()->Write(password + wxT("\n"));
		}
	}
	
	if (IsUsernameRequired(text)) {
		wxString username = wxGetTextFromUser(wxT("SVN Username:"), wxT("Username:"));
		if (username.IsEmpty() == false) {
			m_svnDriver->Svn()->GetProcess()->Write(username + wxT("\n"));
		}
	}
	
#endif
}

bool SvnDefaultCmdHandler::IsUsernameRequired(wxString text)
{
	if(text.MakeLower().Contains(wxT("username:"))){
		return true;
	}
	return false;
}

bool SvnDefaultCmdHandler::IsAuthFailed(wxString text)
{
#ifdef __WXMSW__
	if (text.MakeLower().Contains(wxT("authentication realm:"))) {
		return true;
	}
	return false;
#else
	if (text.MakeLower().Contains(wxT("password for '"))) {
		return true;
	}
	return false;
#endif
}

bool SvnDefaultCmdHandler::IsVerificationNeeded(wxString text)
{
	if (text.MakeLower().Contains(wxT("(r)eject, accept (t)emporarily or accept (p)ermanently"))) {
		return true;
	}
	return false;
}

void SvnCommitCmdHandler::ProcessEvent(wxCommandEvent &e)
{
	//call default handling
	SvnDefaultCmdHandler::ProcessEvent(e);
}


void SvnDiffCmdHandler::ProcessEvent(wxCommandEvent &event)
{
	wxString text(event.GetString());
	if (event.GetEventType() == wxEVT_ASYNC_PROC_STARTED) {
		text = wxT("----\n");
	} else if (event.GetEventType() == wxEVT_ASYNC_PROC_ENDED) {
		text = wxT("----\n");
	}

	if (event.GetEventType() == wxEVT_ASYNC_PROC_ADDLINE) {
		m_content << event.GetString();
		m_content = m_content.Trim().Trim(false);
		m_content << wxT("\n");
	} else {
		m_svnDriver->PrintMessage(text);
		if (event.GetEventType() == wxEVT_ASYNC_PROC_ENDED) {
			//Create a diff file and open it in the editor
			m_svnDriver->DisplayDiffFile(m_fileName, m_content);
		}
	}
}

void SvnChangeLogCmdHandler::ProcessEvent(wxCommandEvent &event)
{
	wxString text(event.GetString());
	if (event.GetEventType() == wxEVT_ASYNC_PROC_STARTED) {
		text = wxT("----\n");
	} else if (event.GetEventType() == wxEVT_ASYNC_PROC_ENDED) {
		text = wxT("----\n");
	}

	if (event.GetEventType() == wxEVT_ASYNC_PROC_ADDLINE) {
		m_content << event.GetString();
		m_content = m_content.Trim().Trim(false);
		m_content << wxT("\n");
	} else {
		m_svnDriver->PrintMessage(text);
		if (event.GetEventType() == wxEVT_ASYNC_PROC_ENDED) {
			//Create a diff file and open it in the editor
			m_svnDriver->DisplayLog(m_outputFile, m_content);
		}
	}
}
