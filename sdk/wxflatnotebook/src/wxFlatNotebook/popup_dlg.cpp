#include <wx/wxFlatNotebook/popup_dlg.h>
#include <wx/listctrl.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/wxFlatNotebook/renderer.h>
#include <wx/listbox.h>
#include <wx/image.h>
//#include <wx/mstream.h>
#include <wx/wxFlatNotebook/fnb_resources.h>

wxBitmap wxTabNavigatorWindow::m_bmp;

wxTabNavigatorWindow::wxTabNavigatorWindow(wxWindow* parent)
: m_listBox(NULL)
, m_selectedItem(-1)
, m_panel(NULL)
{
	Create(parent);
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	GetSizer()->Layout();
	Centre();
}

wxTabNavigatorWindow::wxTabNavigatorWindow()
: wxDialog()
, m_listBox(NULL)
, m_selectedItem(-1)
, m_panel(NULL)
{
}

wxTabNavigatorWindow::~wxTabNavigatorWindow()
{
}

void wxTabNavigatorWindow::Create(wxWindow* parent)
{
	long style = 0;
	if(  !wxDialog::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style) )
		return;

	wxBoxSizer *sz = new wxBoxSizer( wxVERTICAL );
	SetSizer( sz );

	long flags = wxLB_SINGLE | wxNO_BORDER ;
	m_listBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(300, 200), 0, NULL, flags);

	static int panelHeight = 0;
	if( panelHeight == 0 )
	{
		wxMemoryDC mem_dc;

		// bitmap must be set before it can be used for anything
		wxBitmap bmp(10, 10);
		mem_dc.SelectObject(bmp);

		wxFont font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
		font.SetWeight( wxBOLD );
		mem_dc.SetFont(font);
		int w;
		mem_dc.GetTextExtent(wxT("Tp"), &w, &panelHeight);
		panelHeight += 4; // Place a spacer of 2 pixels

		// Out signpost bitmap is 24 pixels
		if( panelHeight < 24 )
			panelHeight = 24;
	}

	m_panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(300, panelHeight));

	sz->Add( m_panel );
	sz->Add( m_listBox, 1, wxEXPAND );
	
	SetSizer( sz );

	// Connect events to the list box
	m_listBox->Connect(wxID_ANY, wxEVT_KEY_UP, wxKeyEventHandler(wxTabNavigatorWindow::OnKeyUp), NULL, this); 
	//Connect(wxEVT_CHAR_HOOK, wxCharEventHandler(wxTabNavigatorWindow::OnKeyUp), NULL, this);
	m_listBox->Connect(wxID_ANY, wxEVT_NAVIGATION_KEY, wxNavigationKeyEventHandler(wxTabNavigatorWindow::OnNavigationKey), NULL, this); 
	m_listBox->Connect(wxID_ANY, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(wxTabNavigatorWindow::OnItemSelected), NULL, this);
	
	// Connect paint event to the panel
	m_panel->Connect(wxID_ANY, wxEVT_PAINT, wxPaintEventHandler(wxTabNavigatorWindow::OnPanelPaint), NULL, this);
	m_panel->Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(wxTabNavigatorWindow::OnPanelEraseBg), NULL, this);

	SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
	m_listBox->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	PopulateListControl( static_cast<wxFlatNotebook*>( parent ) );

	// Create the bitmap, only once
	if( !m_bmp.Ok() )
	{
		wxImage img(signpost_xpm);
		img.SetAlpha(signpost_alpha, true);
		m_bmp =  wxBitmap(img); 
	}
	m_listBox->SetFocus();
}

void wxTabNavigatorWindow::OnKeyUp(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_CONTROL)
	{
		CloseDialog();
	}
}

void wxTabNavigatorWindow::OnNavigationKey(wxNavigationKeyEvent &event)
{
	long selected = m_listBox->GetSelection();
	wxFlatNotebook* bk = static_cast<wxFlatNotebook*>(GetParent());
	long maxItems = bk->GetPageCount();
	long itemToSelect;
		
	if( event.GetDirection() )
	{
		// Select next page
		if (selected == maxItems - 1)
			itemToSelect = 0;
		else
			itemToSelect = selected + 1;
	}
	else
	{
		// Previous page
		if( selected == 0 )
			itemToSelect = maxItems - 1;
		else
			itemToSelect = selected - 1;
	}

	m_listBox->SetSelection( itemToSelect );
}

void wxTabNavigatorWindow::PopulateListControl(wxFlatNotebook *book)
{
	int selection = book->GetSelection();
	//int count     = book->GetPageCount();
	
	std::map<int, bool> temp;
	m_listBox->Append( book->GetPageText(static_cast<int>(selection)) );
	m_indexMap[0] = selection;
	temp[selection] = true;
	
	const wxArrayInt &arr = book->GetBrowseHistory();
	for(size_t i=0; i<arr.GetCount(); i++)
	{
		if(temp.find(arr.Item(i)) == temp.end()){ 
			m_listBox->Append( book->GetPageText(static_cast<int>(arr.Item(i))) );
			m_indexMap[(int)m_listBox->GetCount()-1] = arr.Item(i);
			temp[arr.Item(i)] = true;
		}
	}

	// Select the next entry after the current selection
	m_listBox->SetSelection( 0 );
	wxNavigationKeyEvent dummy;
	dummy.SetDirection(true);
	OnNavigationKey(dummy);
}

void wxTabNavigatorWindow::OnItemSelected(wxCommandEvent & event )
{
	wxUnusedVar( event );
	CloseDialog();
}

void wxTabNavigatorWindow::CloseDialog()
{
	wxFlatNotebook* bk = static_cast<wxFlatNotebook*>(GetParent());
	m_selectedItem = m_listBox->GetSelection();
	std::map<int, int>::iterator iter = m_indexMap.find(m_selectedItem);
	bk->SetSelection( iter->second );
	EndModal( wxID_OK );
}

void wxTabNavigatorWindow::OnPanelPaint(wxPaintEvent &event)
{
	wxUnusedVar(event);
	wxPaintDC dc(m_panel);
	wxRect rect = m_panel->GetClientRect();

	static bool first = true;
	static wxBitmap bmp( rect.width, rect.height );

	if( first )
	{
		first = false;
		wxMemoryDC mem_dc;
		mem_dc.SelectObject( bmp );

		wxColour endColour( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW) );
		wxColour startColour( wxFNBRenderer::LightColour(endColour, 50) );
		wxFNBRenderer::PaintStraightGradientBox(mem_dc, rect, startColour, endColour);

		// Draw the caption title and place the bitmap
		wxPoint bmpPt;
		wxPoint txtPt;

		// get the bitmap optimal position, and draw it
		bmpPt.y = (rect.height - m_bmp.GetHeight()) / 2;
		bmpPt.x = 3;
		mem_dc.DrawBitmap( m_bmp, bmpPt, true );

		// get the text position, and draw it
		int fontHeight(0), w(0);
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		font.SetWeight( wxBOLD );
		mem_dc.SetFont( font );
		mem_dc.GetTextExtent( wxT("Tp"), &w, &fontHeight );
		
		txtPt.x = bmpPt.x + m_bmp.GetWidth() + 4;
		txtPt.y = (rect.height - fontHeight)/2;
		mem_dc.SetTextForeground( *wxWHITE );
		mem_dc.DrawText( wxT("Opened tabs:"), txtPt );
		mem_dc.SelectObject( wxNullBitmap );
	}

	dc.DrawBitmap( bmp, 0, 0 );
}

void wxTabNavigatorWindow::OnPanelEraseBg(wxEraseEvent &event)
{
	wxUnusedVar(event);
	
}
