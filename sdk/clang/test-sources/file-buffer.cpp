#include "globals.h"
#include "rsssettings.h"
#include "accountmanager.h"
#include "credentialsdlg.h"
#include "downloadspanel.h"
#include "preferencesdlg.h"
#include "trayicon.h"
#include <wx/xrc/xmlres.h>
#include "jobqueue.h"
#include "event_ids.h"
#include "configtool.h"
#include "nzbdetailsdlg.h"
#include <wx/dir.h>
#include <wx/mimetype.h>
#include "rssparser.h"
#include <wx/stdpaths.h>
#include "rulesdlg.h"
#include <wx/msgdlg.h>
#include "nzbtracker_frame.h"
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/sizer.h>
#include <wx/menu.h> //wxMenuBar
#include "rssreader.h"
#include "threadmessage.h"
#include "nzbdownloaderthread.h"

#ifndef __WXMSW__
# include "images/agent.xpm"
# include "images/agent.xpm.alpha"
#endif

#ifdef LoadBitmap
#undef LoadBitmap
#endif

BEGIN_EVENT_TABLE(NzbTrackerFrame, wxFrame)
	EVT_CLOSE(NzbTrackerFrame::OnClose)
	EVT_MENU(wxID_EXIT, NzbTrackerFrame::OnQuit)
	EVT_MENU(Clear_History_Id, NzbTrackerFrame::OnClear)
	EVT_MENU(Clear_Log_Id, NzbTrackerFrame::OnClearLog)
	EVT_MENU(Open_Nzb_Rules_Id, NzbTrackerFrame::OnOpenNzbRules)
	EVT_MENU(Edit_Credentials_Id, NzbTrackerFrame::OnEditCredentials)
	EVT_MENU(Minimize_To_Tray_Id, NzbTrackerFrame::OnMinimizeToTray)
	EVT_MENU(Rss_Settings_Id, NzbTrackerFrame::OnPreferences)

	EVT_TIMER(Frame_One_Shot_Timer_Id, NzbTrackerFrame::OnOneShotTimer)
	EVT_TIMER(Frame_Timer_Id, NzbTrackerFrame::OnTimer)

	EVT_UPDATE_UI(Download_Nzb_Id, NzbTrackerFrame::OnSelectionExist)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_JOB_STATUS_VOID_PTR, NzbTrackerFrame::OnWorkerThreadMessage)
	EVT_COMMAND(Download_Nzb_Id, wxEVT_COMMAND_MENU_SELECTED, NzbTrackerFrame::OnDownloadNzb)
	EVT_COMMAND(Download_Scanned_Nzb_Id, wxEVT_COMMAND_MENU_SELECTED, NzbTrackerFrame::OnDownloadScannedNzb)
END_EVENT_TABLE()

NzbTrackerFrame::NzbTrackerFrame(wxWindow* parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style)
		: wxFrame(parent, id, title, pos, size, style)
{
	this->
	Initialize();
	m_oneShotTimer = new wxTimer(this, Frame_One_Shot_Timer_Id);
	m_timer = new wxTimer(this, Frame_Timer_Id);
	m_oneShotTimer->Start(100, true);
}

NzbTrackerFrame::~NzbTrackerFrame()
{
	if (m_trayIcon)
	{
		delete m_trayIcon;
	}
}

void NzbTrackerFrame::Initialize()
{
	ConfigTool::Get()->Load();
	AccountManager::Get().Load();
	RSSManager::Get().Load();

#ifdef __WXMSW__
	SetIcon(wxICON(aaaaa));
#else
	wxImage img(agent_xpm);
	img.SetAlpha(agent_alpha, true);
	wxBitmap bmp(img);
	wxIcon icon;
	icon.CopyFromBitmap(bmp);
	SetIcon(icon);
#endif

	wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sz);

	wxPanel *panel = new wxPanel(this);
	sz->Add(panel, 1, wxEXPAND);

	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(m_mainSizer);

	//Create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(1);
	SetStatusBar(statusBar);
	GetStatusBar()->SetStatusText(_("Ready"));

	// Create a menu bar
	CreateMenuBar();

	// Create the toolbar
	CreateToolBar(this);

	// create tray bar icon
	m_trayIcon = new TrayIcon(this);

	m_mainPanel = new MainPanel(panel);
	m_mainSizer->Add(m_mainPanel, 1, wxEXPAND|wxALL);

	// start the threads
	JobQueueSingleton::Get()->Start(11);

	sz->Layout();
	m_mainSizer->Layout();
}

void NzbTrackerFrame::OnClose(wxCloseEvent &e)
{
	Hide();
}

void NzbTrackerFrame::OnQuit(wxCommandEvent &e)
{
	wxUnusedVar(e);
	JobQueueSingleton::Get()->Stop();

	m_oneShotTimer->Stop();
	delete m_oneShotTimer;

	m_timer->Stop();
	delete m_timer;


	ConfigTool::Free();
	JobQueueSingleton::Free();

	Destroy();
}

void NzbTrackerFrame::CreateMenuBar()
{
	wxMenuBar *mb = new wxMenuBar();
	// File Menu

	wxMenu *menu = new wxMenu();

	menu->Append(Download_Nzb_Id, _("&Open NZB"));
	menu->AppendSeparator();

	menu->Append(Clear_History_Id, _("&Clear History"));
	menu->AppendSeparator();

	menu->Append(Clear_Log_Id, _("&Clear Log"));
	menu->AppendSeparator();

	menu->Append(wxID_EXIT);
	mb->Append(menu, _("&File"));

	menu = new wxMenu();
	menu->Append(Open_Nzb_Rules_Id, _("NZB Matching &Criteria..."));
	menu->Append(Edit_Credentials_Id, _("Cr&edentials..."));
	menu->Append(Rss_Settings_Id, _("&RSS settings..."));
	mb->Append(menu, _("&Settings"));
	SetMenuBar(mb);
}

bool NzbTrackerFrame::ConnectMatrix()
{
	wxString username, password, loginUrl;

	Account acc;
	AccountManager::Get().GetActive(acc);

	username = acc.GetUser();
	password = acc.GetPassword();
	loginUrl = acc.GetLoginUrl();

	m_mainPanel->LogMessage(wxString::Format(_("Connecting %s@%s...\n"), username.c_str(), loginUrl.c_str()));

	if (username.IsEmpty() || password.IsEmpty() || loginUrl.IsEmpty())
	{
		m_mainPanel->LogMessage(_("ERROR: No user credentials are specified, prompting user...\n"));
		return false;
	}

	if (!m_network.Login(loginUrl, loginUrl, username, password))
	{
		m_mainPanel->LogMessage(_("ERROR: Login failed !\n"));
		return false;
	}
	m_mainPanel->LogMessage(_("Login successfull\n"));
	return true;
}

void NzbTrackerFrame::OnOneShotTimer(wxTimerEvent& e)
{
	wxUnusedVar( e );

	//------------------------------------------------------------
	// set some default vlaues, if needed
	//------------------------------------------------------------
	wxString rootDir( wxStandardPaths::Get().GetUserDataDir() );

	//------------------------------------------------------------
	// Login
	//------------------------------------------------------------
	if ( !ConnectMatrix() )
	{
		if ( !DoEditCredentials() )
		{
			return;
		}
	}

	m_timer->Start(RSSManager::Get().GetPollInterval()*1000);
	LaunchThreads();
}

void NzbTrackerFrame::OnTimer(wxTimerEvent& e)
{
	wxUnusedVar( e );
	LaunchThreads();
}

void NzbTrackerFrame::LaunchThreads()
{
	// launch the threads
	wxString tmpfile;
	tmpfile << wxStandardPaths::Get().GetUserDataDir()
	<< wxFileName::GetPathSeparator()
	<< wxT("rss")
	<< wxFileName::GetPathSeparator();

	wxArrayString rssArr;
	rssArr = RSSManager::Get().GetRss();
	for (size_t i=0; i<rssArr.GetCount(); i++)
	{
		wxString filename;
		filename = wxString::Format(wxT("source_%u.rss"), i);
		filename.Prepend(tmpfile);
		JobQueueSingleton::Get()->QueueJob(new RSSReaderJob(this, rssArr.Item(i).c_str(), filename.c_str()));

	}
}

void NzbTrackerFrame::OnWorkerThreadMessage(wxCommandEvent& e)
{
	ThreadMessage *msg = (ThreadMessage*) e.GetClientData();

	switch (msg->GetKind())
	{
	case Msg_Nzb_Download_Started:
	{
		m_mainPanel->SetPage(1);
		m_mainPanel->GetDownloadPage()->UpdateDownload(msg->GetMessage(), 0);
	}
	break;
	case Msg_Nzb_Download_Completed:
	{
		// update the download page
		m_mainPanel->GetDownloadPage()->UpdateDownloadCompleted(msg->GetMessage());

		// try to open the file
		wxMimeTypesManager *mgr = wxTheMimeTypesManager;
		wxFileName fn(msg->GetNzbFile());
		wxFileType *type = mgr->GetFileTypeFromExtension(fn.GetExt());
		if ( type )
		{
			wxString cmd = type->GetOpenCommand(fn.GetFullPath());
			delete type;

			if ( cmd.IsEmpty() == false )
			{
				wxExecute(cmd);
			}
			else
			{
				LogMessage(wxString::Format(_("Failed to open NZB file in default application\n")));
			}
		}
	}
	break;
	case Msg_Nzb_Download_Progress:
	{
		m_mainPanel->GetDownloadPage()->UpdateDownload(msg->GetMessage(), msg->GetProgress());
	}
	break;
	case Msg_Rss_Fetched:
	{
		RSSParser parser(this);
		parser.Parse(msg->GetRssUrl(), msg->GetDownloadedRssFile());

		std::vector<RSSEntry> scanned = parser.GetScannedEntries();
		for (size_t i=0; i<scanned.size(); i++)
		{
			m_mainPanel->AddEntry(scanned.at(i), List_Scanned);
		}

		std::vector<RSSEntry> matches = parser.GetMatchedEntries();
		for (size_t i=0; i<matches.size(); i++)
		{
			LogMessage(wxString::Format(_("Wooha! got a match [%s]\n"), matches.at(i).GetTitle().c_str()));
			m_mainPanel->AddEntry(matches.at(i), List_Matched);
		}

		std::vector<RSSEntry> autoDlMatches = parser.GetAutoDownloadEntries();
		for (size_t i=0; i<autoDlMatches.size(); i++)
		{
			LogMessage(wxString::Format(_("Wooha! got a match [%s]\n"), autoDlMatches.at(i).GetTitle().c_str()));
			long item = m_mainPanel->AddEntry(autoDlMatches.at(i), List_Matched);
			if(item != wxNOT_FOUND) {
				m_mainPanel->MarkItemAsDownloaded(item);
			}
		}

		// Start downloading the entries that are marked as 'auto download'
		DoDownloadEntries( autoDlMatches );

	}
	break;
	default:
		m_mainPanel->LogMessage(msg->GetMessage());
		break;
	}

	delete msg;
}

void NzbTrackerFrame::LogMessage(const wxString& message)
{
	m_mainPanel->LogMessage(message);
}

void NzbTrackerFrame::OnClear(wxCommandEvent& e)
{
	TrackerDatabase database;
	if (database.OpenDatabase()) {
		database.ClearDownloadHistory();
	}
	m_mainPanel->Clear(List_Matched);
	m_mainPanel->Clear(List_Scanned);

	// scann the RSS feeds
	LaunchThreads();
}

void NzbTrackerFrame::OnDownloadNzb(wxCommandEvent& e)
{
	std::vector<RSSEntry> entries;
	m_mainPanel->GetSelection(entries, List_Matched);

	DoDownloadEntries( entries );
	m_mainPanel->MarkSelectionAsDownloaded();
}

void NzbTrackerFrame::OnOpenNzbRules(wxCommandEvent& e)
{
	wxUnusedVar(e);
	RulesDlg dlg(this);
	dlg.ShowModal();
}

void NzbTrackerFrame::CreateToolBar(wxWindow* parent)
{
	// create the toolbar here
	wxToolBar *tb = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	tb->SetToolBitmapSize( wxSize( 48,48 ) );

	tb->AddTool(Rss_Settings_Id, _("RSS Settings..."), wxXmlResource::Get()->LoadBitmap(wxT("rss_settings")), _("RSS Settings..."));
	tb->AddTool(Open_Nzb_Rules_Id, _("NZB Matching Criteria..."), wxXmlResource::Get()->LoadBitmap(wxT("configure_rules")), _("NZB Matching Criteria..."));

	tb->AddSeparator();
	tb->AddTool(Download_Nzb_Id, _("Download"), wxXmlResource::Get()->LoadBitmap(wxT("download_nzb")), _("Download"));

	tb->AddSeparator();
	tb->AddTool(Edit_Credentials_Id, _("Credentials..."), wxXmlResource::Get()->LoadBitmap(wxT("credentials")), _("Credentials..."));

	tb->AddSeparator();
	tb->AddTool(Clear_History_Id, _("Clear History..."), wxXmlResource::Get()->LoadBitmap(wxT("clean_history")), _("Clear History..."));

	tb->AddSeparator();
	tb->AddTool(Minimize_To_Tray_Id, _("Minimize to Tray"), wxXmlResource::Get()->LoadBitmap(wxT("minimize")), _("Minimize to Tray"));

	tb->Realize();
	SetToolBar(tb);
	//m_mainSizer->Add(tb, 0, wxEXPAND|wxALL, 5);
}

void NzbTrackerFrame::OnSelectionExist(wxUpdateUIEvent& e)
{
	std::vector<RSSEntry> entries;
	m_mainPanel->GetSelection(entries, List_Matched);
	e.Enable( entries.empty() == false );
}

void NzbTrackerFrame::OnEditCredentials(wxCommandEvent& e)
{
	wxUnusedVar(e);
	DoEditCredentials();
}

bool NzbTrackerFrame::DoEditCredentials()
{
	while ( true )
	{
		CredentialsDlg dlg(this);
		if (dlg.ShowModal() == wxID_OK)
		{
			if ( ConnectMatrix() )
			{
				return true;
			}
		}
		else
		{
			return ConnectMatrix();
		}
	}
}

void NzbTrackerFrame::OnClearLog(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_mainPanel->CleanLog();
}

void NzbTrackerFrame::OnMinimizeToTray(wxCommandEvent& e)
{
	wxUnusedVar(e);
	Hide();
}

void NzbTrackerFrame::OnPreferences(wxCommandEvent& e)
{
	PreferencesDlg dlg(this);
	if (dlg.ShowModal() == wxID_OK)
	{
		// update the timer
		m_timer->Stop();
		m_timer->Start(RSSManager::Get().GetPollInterval() * 1000);
		LogMessage(wxString::Format(_("Updated RSS retrieval interval to %d seconds\n"), RSSManager::Get().GetPollInterval()));
	}
}

void NzbTrackerFrame::OnDownloadScannedNzb(wxCommandEvent& e)
{
	std::vector<RSSEntry> entries;
	m_mainPanel->GetSelection(entries, List_Scanned);

	for(size_t i=0; i<entries.size(); i++) {
		RSSEntry entry = entries.at(i);

		LogMessage(wxString::Format(_("Downloading '%s'\n"), entry.GetTitle().c_str()));

		Account account;
		AccountManager::Get().GetActive(account);

		NzbDownloaderThread *job = new NzbDownloaderThread(this, entry.GetLink().c_str(), new Account(account));
		job->SetNzbTitle(entry.GetTitle());

		JobQueueSingleton::Get()->QueueJob(job);
	}
}

void NzbTrackerFrame::DoDownloadEntries(const std::vector<RSSEntry>& entries)
{
	if(entries.empty() == false) {
		for(size_t i=0; i<entries.size(); i++) {

			RSSEntry entry = entries.at(i);
			LogMessage(wxString::Format(_("Downloading '%s'\n"), entry.GetTitle().c_str()));

			Account account;
			AccountManager::Get().GetActive(account);

			NzbDownloaderThread *job = new NzbDownloaderThread(this, entry.GetLink().c_str(), new Account(account));
			job->SetNzbTitle(entry.GetTitle());

			JobQueueSingleton::Get()->QueueJob(job);
		}
	}
}
