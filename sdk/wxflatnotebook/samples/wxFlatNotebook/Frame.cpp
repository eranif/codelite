#include "Frame.h"
#include <wx/xrc/xmlres.h> 
#include "wx/colordlg.h"
#include <wx/wxFlatNotebook/renderer.h>

#define MENU_EDIT_DELETE_ALL				10000
#define MENU_EDIT_ADD_PAGE					10001
#define MENU_EDIT_DELETE_PAGE				10002
#define MENU_EDIT_SET_SELECTION				10003
#define MENU_EDIT_ADVANCE_SELECTION_FWD		10004
#define MENU_EDIT_ADVANCE_SELECTION_BACK	10005
#define MENU_SET_ALL_TABS_SHAPE_ANGLE		10006
#define MENU_SHOW_IMAGES					10007
#define MENU_USE_VC71_STYLE					10008
#define MENU_USE_DEFULT_STYLE				10009
#define MENU_USE_FANCY_STYLE				10010
#define MENU_SELECT_GRADIENT_COLOR_FROM		10011
#define MENU_SELECT_GRADIENT_COLOR_TO		10012
#define MENU_SELECT_GRADIENT_COLOR_BORDER	10013
#define MENU_SET_PAGE_IMAGE_INDEX			10014
#define MENU_HIDE_X							10015
#define MENU_HIDE_NAV_BUTTONS				10016
#define MENU_USE_MOUSE_MIDDLE_BTN			10017
#define MENU_DRAW_BORDER					10018
#define MENU_USE_BOTTOM_TABS				10019
#define MENU_ENABLE_TAB						10020
#define MENU_DISABLE_TAB					10021
#define MENU_ENABLE_DRAG_N_DROP				10022
#define MENU_DCLICK_CLOSES_TAB				10023
#define MENU_USE_VC8_STYLE					10024

#define MENU_SET_ACTIVE_TEXT_COLOR			10027
#define MENU_DRAW_TAB_X						10028
#define MENU_SET_ACTIVE_TAB_COLOR			10029
#define MENU_SET_TAB_AREA_COLOR				10030
#define MENU_SELECT_NONACTIVE_TEXT_COLOR	10031
#define MENU_GRADIENT_BACKGROUND			10032
#define MENU_COLORFULL_TABS					10033
#define MENU_SMART_TABS						10034
#define MENU_USE_DROP_ARROW_BUTTON			10035
#define MENU_ALLOW_FOREIGN_DND				10036
#define MENU_EDIT_INSERT_PAGE				10037
#define MENU_EDIT_INSERT_BEFORE_PAGE		10038
#define MENU_EDIT_SELECT_PAGE_TO_DELETE		10039

BEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_MENU_RANGE(MENU_HIDE_X, MENU_USE_BOTTOM_TABS, Frame::OnStyle)
EVT_MENU(wxID_EXIT, Frame::OnQuit)
EVT_MENU(MENU_EDIT_DELETE_ALL, Frame::OnDeleteAll)
EVT_MENU(MENU_EDIT_ADD_PAGE, Frame::OnAddPage)
EVT_MENU(MENU_EDIT_DELETE_PAGE, Frame::OnDeletePage)
EVT_MENU(MENU_EDIT_SET_SELECTION, Frame::OnSetSelection)
EVT_MENU(MENU_EDIT_ADVANCE_SELECTION_FWD, Frame::OnAdvanceSelectionFwd)
EVT_MENU(MENU_EDIT_ADVANCE_SELECTION_BACK, Frame::OnAdvanceSelectionBack)
EVT_MENU(MENU_SET_ALL_TABS_SHAPE_ANGLE, Frame::OnSetAllPagesShapeAngle)
EVT_MENU(MENU_SHOW_IMAGES, Frame::OnShowImages)
EVT_MENU_RANGE(MENU_SELECT_GRADIENT_COLOR_FROM, MENU_SELECT_GRADIENT_COLOR_BORDER, Frame::OnSelectColor)
EVT_MENU(MENU_USE_VC71_STYLE, Frame::OnVC71Style)
EVT_MENU(MENU_USE_VC8_STYLE, Frame::OnVC8Style)
EVT_MENU(MENU_USE_DEFULT_STYLE, Frame::OnDefaultStyle)
EVT_MENU(MENU_USE_FANCY_STYLE, Frame::OnFancyStyle)
EVT_MENU(MENU_SET_PAGE_IMAGE_INDEX, Frame::OnSetPageImageIndex)
EVT_MENU(MENU_ENABLE_TAB, Frame::OnEnableTab)
EVT_MENU(MENU_DISABLE_TAB, Frame::OnDisableTab)
EVT_MENU(MENU_ENABLE_DRAG_N_DROP, Frame::OnEnableDrag)
EVT_MENU(MENU_SELECT_NONACTIVE_TEXT_COLOR, Frame::OnSelectColor)
EVT_MENU(MENU_SET_ACTIVE_TAB_COLOR, Frame::OnSelectColor)
EVT_MENU(MENU_SET_TAB_AREA_COLOR, Frame::OnSelectColor)
EVT_MENU(MENU_SET_ACTIVE_TEXT_COLOR, Frame::OnSelectColor)
EVT_MENU(MENU_DRAW_TAB_X, Frame::OnDrawTabX)
EVT_MENU(MENU_DCLICK_CLOSES_TAB, Frame::OnDClickCloseTab)
EVT_MENU(MENU_GRADIENT_BACKGROUND, Frame::OnGradientBack)
EVT_MENU(MENU_COLORFULL_TABS, Frame::OnColorfullTabs)
EVT_MENU(MENU_SMART_TABS, Frame::OnSmartTabs)
EVT_MENU(MENU_USE_DROP_ARROW_BUTTON, Frame::OnDropDownArrow)
EVT_MENU(MENU_ALLOW_FOREIGN_DND, Frame::OnAllowForeignDnd)

EVT_UPDATE_UI(MENU_USE_DROP_ARROW_BUTTON, Frame::OnDropDownArrowUI)
EVT_UPDATE_UI(MENU_HIDE_NAV_BUTTONS, Frame::OnHideNavigationButtonsUI)
EVT_UPDATE_UI(MENU_ALLOW_FOREIGN_DND, Frame::OnAllowForeignDndUI)

EVT_FLATNOTEBOOK_PAGE_CHANGING(-1, Frame::OnPageChanging)
EVT_FLATNOTEBOOK_PAGE_CHANGED(-1, Frame::OnPageChanged)
EVT_FLATNOTEBOOK_PAGE_CLOSING(-1, Frame::OnPageClosing)

EVT_MENU(MENU_EDIT_INSERT_PAGE, Frame::OnInsertPage)
EVT_MENU(MENU_EDIT_INSERT_BEFORE_PAGE, Frame::OnInsertBeforeGivenPage)
EVT_MENU(MENU_EDIT_SELECT_PAGE_TO_DELETE, Frame::OnDeleteGivenPage)
END_EVENT_TABLE()

Frame::Frame(wxWindow* WXUNUSED(parent), const wxChar *title)
: m_fileMenu(NULL)
, m_editMenu(NULL)
, m_visualMenu(NULL)
, m_menuBar(NULL)
#ifdef DEVELOPMENT
, m_logTargetOld(NULL)
#endif
, book(NULL)
, secondBook(NULL)
, m_bShowImages(false)
, m_bVCStyle(false)
, newPageCounter(0)
{
	m_ImageList.push_back(wxXmlResource::Get()->LoadBitmap(wxT("book_red")));
	m_ImageList.push_back(wxXmlResource::Get()->LoadBitmap(wxT("book_green")));
	m_ImageList.push_back(wxXmlResource::Get()->LoadBitmap(wxT("book_blue")));
	Create(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE |wxNO_FULL_REPAINT_ON_RESIZE);
	Maximize();
}

Frame::~Frame(void)
{
#ifdef DEVELOPMENT
	delete wxLog::SetActiveTarget(m_logTargetOld);
#endif
	wxFNBRendererMgrST::Free();
}

bool Frame::Create(wxFrame * parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	wxFrame::Create(parent, id, caption, pos, size, style);

	// Init the menu bar
	m_menuBar  = new wxMenuBar( wxMB_DOCKABLE );
	m_fileMenu = new wxMenu();
	m_visualMenu = new wxMenu();

	wxMenuItem *item = new wxMenuItem(m_fileMenu, wxID_EXIT, wxT("E&xit"), wxT("Exit demo"), wxITEM_NORMAL);
	m_fileMenu->Append(item);
	
	m_editMenu = new wxMenu();
	item = new wxMenuItem(m_editMenu, MENU_EDIT_ADD_PAGE, wxT("New Page\tCtrl+N"), wxT("Add New Page"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_INSERT_PAGE, wxT("Insert Page\tCtrl+I"), wxT("Insert New Page"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_DELETE_PAGE, wxT("Delete Page\tCtrl+F4"), wxT("Delete Page"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_SELECT_PAGE_TO_DELETE, wxT("Select Page to Delete"), wxT("Delete Given Page"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_DELETE_ALL, wxT("Delete All Pages"), wxT("Delete All Pages"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_SET_SELECTION, wxT("Set Selection"), wxT("Set Selection"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_ADVANCE_SELECTION_FWD, wxT("Advance Selection Forward"), wxT("Advance Selection Forward"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_EDIT_ADVANCE_SELECTION_BACK, wxT("Advance Selection Backward"), wxT("Advance Selection Backward"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_visualMenu, MENU_SET_ALL_TABS_SHAPE_ANGLE, wxT("Set an inclination of tab header borders"), wxT("Set the shape of tab header"), wxITEM_NORMAL);
	m_visualMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_SET_PAGE_IMAGE_INDEX, wxT("Set image index of selected page"), wxT("Set image index"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_SHOW_IMAGES, wxT("Show Images"), wxT("Show Images"), wxITEM_CHECK);
	m_editMenu->Append(item);

	wxMenu *styleMenu = new wxMenu();
	item = new wxMenuItem(styleMenu, MENU_USE_DEFULT_STYLE, wxT("Use Default Style"), wxT("Use VC71 Style"), wxITEM_RADIO);
	styleMenu->Append(item);

	item = new wxMenuItem(styleMenu, MENU_USE_VC71_STYLE, wxT("Use VC71 Style"), wxT("Use VC71 Style"), wxITEM_RADIO);
	styleMenu->Append(item);

	item = new wxMenuItem(styleMenu, MENU_USE_VC8_STYLE, wxT("Use VC8 Style"), wxT("Use VC8 Style"), wxITEM_RADIO);
	styleMenu->Append(item);

	item = new wxMenuItem(styleMenu, MENU_USE_FANCY_STYLE, wxT("Use Fancy Style"), wxT("Use Fancy Style"), wxITEM_RADIO);
	styleMenu->Append(item);

	m_visualMenu->Append(-1, wxT("Tabs Style"), styleMenu);
	
	item = new wxMenuItem(m_visualMenu, MENU_SELECT_GRADIENT_COLOR_FROM, wxT("Select fancy tab style 'from' color"), wxT("Select fancy tab style 'from' color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);

	item = new wxMenuItem(m_visualMenu, MENU_SELECT_GRADIENT_COLOR_TO, wxT("Select fancy tab style 'to' color"), wxT("Select fancy tab style 'to' color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);

	item = new wxMenuItem(m_visualMenu, MENU_SELECT_GRADIENT_COLOR_BORDER, wxT("Select fancy tab style 'border' color"), wxT("Select fancy tab style 'border' color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);

	m_editMenu->AppendSeparator();
	item = new wxMenuItem(m_editMenu, MENU_HIDE_NAV_BUTTONS, wxT("Hide Navigation Buttons"), wxT("Hide Navigation Buttons"), wxITEM_CHECK);
	m_editMenu->Append(item);
	item->Check(true);

	item = new wxMenuItem(m_editMenu, MENU_HIDE_X, wxT("Hide X Button"), wxT("Hide X Button"), wxITEM_CHECK);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_SMART_TABS, wxT("Smart tabbing"), wxT("Smart tabbing"), wxITEM_CHECK);
	m_editMenu->Append(item);
	item->Check( false );

	item = new wxMenuItem(m_editMenu, MENU_USE_DROP_ARROW_BUTTON, wxT("Use drop down button for tab navigation"), wxT("Use drop down arrow for quick tab navigation"), wxITEM_CHECK);
	m_editMenu->Append(item);
	item->Check( true );
	m_editMenu->AppendSeparator();

	item = new wxMenuItem(m_editMenu, MENU_USE_MOUSE_MIDDLE_BTN, wxT("Use Mouse Middle Button as 'X' button"), wxT("Use Mouse Middle Button as 'X' button"), wxITEM_CHECK);
	m_editMenu->Append(item);
	
	item = new wxMenuItem(m_editMenu, MENU_DCLICK_CLOSES_TAB, wxT("Mouse double click closes tab"), wxT("Mouse double click closes tab"), wxITEM_CHECK);
	m_editMenu->Append(item);
	item->Check(false);

	m_editMenu->AppendSeparator();

	item = new wxMenuItem(m_editMenu, MENU_USE_BOTTOM_TABS, wxT("Use Bottoms Tabs"), wxT("Use Bottoms Tabs"), wxITEM_CHECK);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_ENABLE_TAB, wxT("Enable Tab"), wxT("Enable Tab"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_DISABLE_TAB, wxT("Disable Tab"), wxT("Disable Tab"), wxITEM_NORMAL);
	m_editMenu->Append(item);

	item = new wxMenuItem(m_editMenu, MENU_ENABLE_DRAG_N_DROP, wxT("Enable Drag And Drop of Tabs"), wxT("Enable Drag And Drop of Tabs"), wxITEM_CHECK);
	m_editMenu->Append(item);
	item->Check(false);

	item = new wxMenuItem(m_editMenu, MENU_ALLOW_FOREIGN_DND, wxT("Enable Drag And Drop of Tabs to foreign notebooks"), wxT("Enable Drag And Drop of Tabs to foreign notebooks"), wxITEM_CHECK);
	m_editMenu->Append(item);
	item->Check(false);

	item = new wxMenuItem(m_visualMenu, MENU_DRAW_BORDER, wxT("Draw Border around tab area"), wxT("Draw Border around tab area"), wxITEM_CHECK);
	m_visualMenu->Append(item);
	item->Check(true);

	item = new wxMenuItem(m_visualMenu, MENU_DRAW_TAB_X, wxT("Draw X button On Active Tab"), wxT("Draw X button On Active Tab"), wxITEM_CHECK);
	m_visualMenu->Append(item);
	
	item = new wxMenuItem(m_visualMenu, MENU_SET_ACTIVE_TAB_COLOR, wxT("Select Active Tab Color"), wxT("Select Active Tab Color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);

	item = new wxMenuItem(m_visualMenu, MENU_SET_TAB_AREA_COLOR, wxT("Select Tab Area Color"), wxT("Select Tab Area Color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);


	item = new wxMenuItem(m_visualMenu, MENU_SET_ACTIVE_TEXT_COLOR, wxT("Select active tab text color"), wxT("Select active tab text color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);

	item = new wxMenuItem(m_visualMenu, MENU_SELECT_NONACTIVE_TEXT_COLOR, wxT("Select NON-active tab text color"), wxT("Select NON-active tab text color"), wxITEM_NORMAL);
	m_visualMenu->Append(item);
	
	item = new wxMenuItem(m_visualMenu, MENU_GRADIENT_BACKGROUND, wxT("Use Gradient Coloring for tab area"), wxT("Use Gradient Coloring for tab area"), wxITEM_CHECK);
	m_visualMenu->Append(item);
	item->Check( false );

	item = new wxMenuItem(m_visualMenu, MENU_COLORFULL_TABS, wxT("Colorful tabs"), wxT("Colorful tabs"), wxITEM_CHECK);
	m_visualMenu->Append(item);
	item->Check( false );

	m_menuBar->Append(m_fileMenu, wxT("&File"));
	m_menuBar->Append(m_editMenu, wxT("&Edit"));
	m_menuBar->Append(m_visualMenu, wxT("&Tab Appearance"));

	SetMenuBar(m_menuBar);

	// Create a right click menu
	wxMenu *rmenu = new wxMenu();
	item = new wxMenuItem(rmenu, MENU_EDIT_DELETE_PAGE, wxT("Close Tab\tCtrl+F4"), wxT("Close Tab"), wxITEM_NORMAL);
	rmenu->Append(item);

	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	long bookStyle = 0; 
	m_bVCStyle ? bookStyle |= wxFNB_VC71 : bookStyle |= 0;
	bookStyle |= wxFNB_TABS_BORDER_SIMPLE;
	bookStyle |= wxFNB_NODRAG;
	bookStyle |= wxFNB_DROPDOWN_TABS_LIST;
	bookStyle |= wxFNB_NO_NAV_BUTTONS;
	bookStyle |= wxFNB_CUSTOM_DLG;

	book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(300, 400), bookStyle);

	// Allow the second notebook to accept foreign pages
	// from other notebooks around
	bookStyle &= ~(wxFNB_NODRAG);
	bookStyle |= wxFNB_ALLOW_FOREIGN_DND;
	secondBook = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(300, 400), bookStyle);

	// Set right click menu to the notebook
	book->SetRightClickMenu(rmenu);

	// Set the image list 
	book->SetImageList(&m_ImageList);

	// Add spacer between the books
	wxPanel *spacer = new wxPanel(this, wxID_ANY);
	spacer->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	mainSizer->Add(book, 6, wxEXPAND);
	mainSizer->Add(spacer, 0, wxALL | wxEXPAND);
	mainSizer->Add(secondBook, 2, wxEXPAND);

	// Add some pages to the second notebook
	Freeze();
	wxTextCtrl *text = new wxTextCtrl(secondBook, wxID_ANY, wxT("Second Book Page 1"), wxDefaultPosition, wxDefaultSize, 
		wxTE_MULTILINE);
	secondBook->AddPage(text,  wxT("Second Book Page 1"));

	text = new wxTextCtrl(secondBook, wxID_ANY, wxT("Second Book Page 2"), wxDefaultPosition, wxDefaultSize, 
		wxTE_MULTILINE);
	secondBook->AddPage(text,  wxT("Second Book Page 2"));

	Thaw();	

#ifdef DEVELOPMENT
	text = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                            wxDefaultPosition, wxSize(250, 250),
                            wxTE_MULTILINE | wxTE_READONLY);

    m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );
	mainSizer->Add(text, 1, wxEXPAND);
#endif

	Centre(); 
	mainSizer->Layout();
	SendSizeEvent();
	return true;
}

void Frame::OnStyle(wxCommandEvent& event)
{
	long style = book->GetWindowStyleFlag();
	switch(event.GetId())
	{
	case MENU_HIDE_NAV_BUTTONS:
		if(event.IsChecked())
		{
			// Hide the navigation buttons
			style |= wxFNB_NO_NAV_BUTTONS;
		}
		else
		{
			style &= ~(wxFNB_NO_NAV_BUTTONS);
			style &= ~(wxFNB_DROPDOWN_TABS_LIST);
		}
		book->SetWindowStyleFlag(style);
		break;
	case MENU_HIDE_X:
		if(event.IsChecked())
		{
			// Hide the X button
			style |= wxFNB_NO_X_BUTTON;
		}
		else
		{
			if(style & wxFNB_NO_X_BUTTON)
				style ^= wxFNB_NO_X_BUTTON;
		}
		book->SetWindowStyleFlag(style);
		break;
	case MENU_DRAW_BORDER:
		if(event.IsChecked())
		{
			style |= wxFNB_TABS_BORDER_SIMPLE;
		}
		else
		{
			if(style & wxFNB_TABS_BORDER_SIMPLE)
				style ^= wxFNB_TABS_BORDER_SIMPLE;
		}
		book->SetWindowStyleFlag(style);
		break;
	case MENU_USE_MOUSE_MIDDLE_BTN:
		if(event.IsChecked())
		{
			style |= wxFNB_MOUSE_MIDDLE_CLOSES_TABS;
		}
		else
		{
			if(style & wxFNB_MOUSE_MIDDLE_CLOSES_TABS)
				style ^= wxFNB_MOUSE_MIDDLE_CLOSES_TABS;
		}
		book->SetWindowStyleFlag(style);
		break;
	case MENU_USE_BOTTOM_TABS:
		if(event.IsChecked())
		{
			style |= wxFNB_BOTTOM;
		}
		else
		{
			if(style & wxFNB_BOTTOM)
				style ^= wxFNB_BOTTOM;
		}
		book->SetWindowStyleFlag(style);
		book->Refresh();
		break;
	}
}

void Frame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void Frame::OnDeleteAll(wxCommandEvent& WXUNUSED(event))
{
	book->DeleteAllPages();
}

void Frame::OnShowImages(wxCommandEvent& event)
{
	m_bShowImages = event.IsChecked();
}

void Frame::OnVC71Style(wxCommandEvent& WXUNUSED(event))
{
	long style = book->GetWindowStyleFlag();

	// remove old tabs style
	long mirror = ~(wxFNB_VC71 | wxFNB_VC8 | wxFNB_FANCY_TABS);
	style &= mirror;

	style |= wxFNB_VC71;

	book->SetWindowStyleFlag(style);
}

void Frame::OnVC8Style(wxCommandEvent& WXUNUSED(event))
{
	long style = book->GetWindowStyleFlag();

	// remove old tabs style
	long mirror = ~(wxFNB_VC71 | wxFNB_VC8 | wxFNB_FANCY_TABS);
	style &= mirror;

	// set new style
	style |= wxFNB_VC8;

	book->SetWindowStyleFlag(style);
}

void Frame::OnDefaultStyle(wxCommandEvent& WXUNUSED(event))
{
	long style = book->GetWindowStyleFlag();

	// remove old tabs style
	long mirror = ~(wxFNB_VC71 | wxFNB_VC8 | wxFNB_FANCY_TABS);
	style &= mirror;

	book->SetWindowStyleFlag(style);
}

void Frame::OnFancyStyle(wxCommandEvent& WXUNUSED(event))
{
	long style = book->GetWindowStyleFlag();

	// remove old tabs style
	long mirror = ~(wxFNB_VC71 | wxFNB_VC8 | wxFNB_FANCY_TABS);
	style &= mirror;

	style |= wxFNB_FANCY_TABS;
	book->SetWindowStyleFlag(style);
}

void Frame::OnSelectColor(wxCommandEvent& event)
{
	// Open a color dialog
	wxColourData data;
	wxColourDialog dlg(this, &data);
	if(dlg.ShowModal() == wxID_OK)
	{
		switch(event.GetId())
		{
		case MENU_SELECT_GRADIENT_COLOR_BORDER:
			book->SetGradientColorBorder(dlg.GetColourData().GetColour());
			break;
		case MENU_SELECT_GRADIENT_COLOR_FROM:
			book->SetGradientColorFrom(dlg.GetColourData().GetColour());
			break;
		case MENU_SELECT_GRADIENT_COLOR_TO:
			book->SetGradientColorTo(dlg.GetColourData().GetColour());
			break;
		case MENU_SET_ACTIVE_TEXT_COLOR:
			book->SetActiveTabTextColour(dlg.GetColourData().GetColour());
			break;
		case MENU_SELECT_NONACTIVE_TEXT_COLOR:
			book->SetNonActiveTabTextColour(dlg.GetColourData().GetColour());
			break;
		case MENU_SET_ACTIVE_TAB_COLOR:
			book->SetActiveTabColour(dlg.GetColourData().GetColour());
			break;
		case MENU_SET_TAB_AREA_COLOR:
			book->SetTabAreaColour(dlg.GetColourData().GetColour());
			break;
		}
		book->Refresh();
	}
}

void Frame::OnAddPage(wxCommandEvent& WXUNUSED(event))
{
	wxString caption;

	caption.Printf(wxT("New Page Added #%ld"), newPageCounter);
	Freeze();
	book->AddPage(CreatePage(),  caption, true, m_bShowImages ? rand() % (int)book->GetImageList()->size() : -1);	
	Thaw();	
}

wxWindow* Frame::CreatePage()
{
	wxString caption;
	caption.Printf(wxT("New Page Added #%ld"), newPageCounter++);

	return  new wxTextCtrl(book, wxID_ANY, caption, wxDefaultPosition, book->GetPageBestSize(), 
		wxTE_MULTILINE);

}

void Frame::OnInsertPage(wxCommandEvent& WXUNUSED(event))
{
	size_t index = book->GetSelection();
	Freeze();
	bool ret = InsertNotebookPage((long)index);
	wxUnusedVar(ret);
	Thaw();	
}

void Frame::OnInsertBeforeGivenPage(wxCommandEvent& WXUNUSED(event))
{
	long tabIdx = GetTabIndexFromUser (wxT("Insert Page"), wxT("Enter Tab Number to insert the page before:"));
	if (tabIdx != -1)
	{
		Freeze();
		bool ret = InsertNotebookPage(tabIdx);
		wxUnusedVar(ret);
		Thaw();	
	}
}


void Frame::OnDeletePage(wxCommandEvent& WXUNUSED(event))
{
	DeleteNotebookPage (book->GetSelection());
}

void Frame::OnDeleteGivenPage(wxCommandEvent& WXUNUSED(event))
{
	long tabIdx = GetTabIndexFromUser (wxT("Delete Page"), wxT("Enter Tab Number to delete:"));
	if (tabIdx != -1)
		DeleteNotebookPage (tabIdx);
}

void Frame::OnSetSelection(wxCommandEvent& WXUNUSED(event))
{
	long tabIdx = GetTabIndexFromUser (wxT("Enable Tab"), wxT("Enter Tab Number to enable:"));
	if (tabIdx != -1)
		book->SetSelection((size_t)tabIdx);
}

void Frame::OnEnableTab(wxCommandEvent& WXUNUSED(event))
{
	long tabIdx = GetTabIndexFromUser (wxT("Set Selection"), wxT("Enter Tab Number to select:"));
	if (tabIdx != -1)
		book->Enable((size_t)tabIdx, true);
}

void Frame::OnDisableTab(wxCommandEvent& WXUNUSED(event))
{
	long tabIdx = GetTabIndexFromUser (wxT("Disable Tab"), wxT("Enter Tab Number to disable:"));
	if (tabIdx != -1)
		book->Enable((size_t)tabIdx, false);
}

void Frame::OnEnableDrag(wxCommandEvent& event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
	{
		if(style & wxFNB_NODRAG)
			style ^= wxFNB_NODRAG;
	}
	else
	{
		style |= wxFNB_NODRAG;
	}

	book->SetWindowStyleFlag(style);
}
long Frame::GetTabIndexFromUser(const wxString &title, const wxString &prompt) const
{
	wxTextEntryDialog dlg((wxWindow*)this, prompt, title);
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString val = dlg.GetValue();
		long tabIdx;
		val.ToLong(&tabIdx);
		return tabIdx;
	}
	return -1;
}

void Frame::DeleteNotebookPage (long tabIdx)
{
	book->DeletePage (tabIdx);
}

bool Frame::InsertNotebookPage (long tabIdx)
{
	wxString caption;
	caption.Printf(wxT("New Page Inserted #%ld"), newPageCounter);
	return book->InsertPage(tabIdx, CreatePage(),  caption, true, m_bShowImages ? rand() % (int)book->GetImageList()->size() : -1);
}

void Frame::OnSetAllPagesShapeAngle(wxCommandEvent& WXUNUSED(event))
{
	
	wxTextEntryDialog dlg(this, wxT("Enter an inclination of header borders (0-15):"), wxT("Set Angle"));
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString val = dlg.GetValue();
		unsigned int angle = wxAtoi(val);		
		book->SetAllPagesShapeAngle(angle);
	}
}

void Frame::OnSetPageImageIndex(wxCommandEvent& WXUNUSED(event))
{
	
	wxTextEntryDialog dlg(this, 
		wxString::Format(wxT("Enter an image index (0-%i):"), book->GetImageList()->size()-1), 
		wxT("Set Angle"));
	if(dlg.ShowModal() == wxID_OK)
	{
		wxString val = dlg.GetValue();
		unsigned int imgindex = wxAtoi(val);		
		book->SetPageImageIndex(book->GetSelection(), imgindex);
	}
}

void Frame::OnAdvanceSelectionFwd(wxCommandEvent& WXUNUSED(event))
{
	book->AdvanceSelection(true);
}

void Frame::OnAdvanceSelectionBack(wxCommandEvent& WXUNUSED(event))
{
	book->AdvanceSelection(false);
}

void Frame::OnPageChanging(wxFlatNotebookEvent& WXUNUSED(event))
{
}

void Frame::OnPageChanged(wxFlatNotebookEvent& event)
{
#ifdef DEVELOPMENT
	wxString msg;
	msg.Printf(wxT("Page has changed, new selection is now=%ld"), event.GetSelection());
	wxLogMessage(msg);
#else
	wxUnusedVar( event );
#endif
}

void Frame::OnPageClosing(wxFlatNotebookEvent& event)
{
#ifdef DEVELOPMENT
	wxString msg;
	msg.Printf(wxT("Page is closing: selection=%ld"), event.GetSelection());
	wxLogMessage(msg);
#else
	wxUnusedVar( event );
#endif
}

void Frame::OnDrawTabX(wxCommandEvent& event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
	{
		style |= wxFNB_X_ON_TAB;
	}
	else
	{
		if(style & wxFNB_X_ON_TAB)
			style ^= wxFNB_X_ON_TAB;		
	}

	book->SetWindowStyleFlag(style);
}

void Frame::OnDClickCloseTab(wxCommandEvent& event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
	{
		style |= wxFNB_DCLICK_CLOSES_TABS;
	}
	else
	{
		style &= ~(wxFNB_DCLICK_CLOSES_TABS);		
	}

	book->SetWindowStyleFlag(style);
}


void Frame::OnGradientBack(wxCommandEvent& event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
		style |= wxFNB_BACKGROUND_GRADIENT;
	else
		style &= ~(wxFNB_BACKGROUND_GRADIENT);
	book->SetWindowStyleFlag( style );
	book->Refresh();
}

void Frame::OnColorfullTabs(wxCommandEvent& event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
		style |= wxFNB_COLORFUL_TABS;
	else
		style &= ~(wxFNB_COLORFUL_TABS);
	book->SetWindowStyleFlag( style );
	book->Refresh();
}

void Frame::OnSmartTabs(wxCommandEvent &event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
		style |= wxFNB_SMART_TABS;
	else
		style &= ~(wxFNB_SMART_TABS);
	book->SetWindowStyleFlag( style );
	book->Refresh();
}

void Frame::OnDropDownArrow(wxCommandEvent &event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
	{
		style |= wxFNB_DROPDOWN_TABS_LIST;
		style |= wxFNB_NO_NAV_BUTTONS;
	}
	else
	{
		style &= ~(wxFNB_DROPDOWN_TABS_LIST);
		style &= ~(wxFNB_NO_NAV_BUTTONS);
	}

	book->SetWindowStyleFlag( style );
	book->Refresh();
}

void Frame::OnHideNavigationButtonsUI(wxUpdateUIEvent & event)
{
	long style = book->GetWindowStyleFlag();
	event.Check( style & wxFNB_NO_NAV_BUTTONS ? true : false );
}

void Frame::OnDropDownArrowUI(wxUpdateUIEvent & event)
{
	long style = book->GetWindowStyleFlag();
	event.Check( style & wxFNB_DROPDOWN_TABS_LIST ? true : false );
}

void Frame::OnAllowForeignDnd(wxCommandEvent &event)
{
	long style = book->GetWindowStyleFlag();
	if(event.IsChecked())
		style |= wxFNB_ALLOW_FOREIGN_DND;
	else
		style &= ~(wxFNB_ALLOW_FOREIGN_DND);
	book->SetWindowStyleFlag( style );
	book->Refresh();
}

void Frame::OnAllowForeignDndUI(wxUpdateUIEvent & event )
{
	long style = book->GetWindowStyleFlag();
	event.Enable( style & wxFNB_NODRAG ? false : true );
}
