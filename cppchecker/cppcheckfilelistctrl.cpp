#include "cppcheckfilelistctrl.h"
#include "drawingutils.h"
#include "cppcheckreportpage.h"

BEGIN_EVENT_TABLE(CppCheckFileListCtrl, wxScintilla)
	EVT_SCI_HOTSPOT_CLICK(wxID_ANY,          CppCheckFileListCtrl::OnHotspotClicked)
END_EVENT_TABLE()

CppCheckFileListCtrl::CppCheckFileListCtrl(wxWindow * window)
		: wxScintilla(window)
		, m_report(NULL)
{
	
#ifdef __WXMAC__
	StyleSetSize(wxSCI_STYLE_DEFAULT, 12  );
	wxFont font(12, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
#else
	StyleSetSize(wxSCI_STYLE_DEFAULT, 8   );
	wxFont font(8 , wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
#endif

	SetReadOnly( true );
	SetLexer(wxSCI_LEX_NULL);

	// Set TELETYPE font (monospace)
	StyleSetFont(wxSCI_STYLE_DEFAULT, font);
	

	SetMarginWidth(0, 0);
	SetMarginWidth(1, 0);
	SetMarginWidth(2, 0);
	SetMarginWidth(3, 0);
	SetMarginWidth(4, 0);

	StyleClearAll();

	MarkerDefine       (7, wxSCI_MARK_BACKGROUND);
	MarkerDefine       (8, wxSCI_MARK_BACKGROUND);

	MarkerSetBackground(7, DrawingUtils::LightColour(wxT("BLUE"), 9));
	MarkerSetBackground(8, DrawingUtils::LightColour(wxT("BLUE"), 7));

	StyleSetHotSpot( 0,   true );

	SetHotspotActiveUnderline (true);
	SetHotspotActiveForeground(true, wxT("BLUE"));
}

CppCheckFileListCtrl::~CppCheckFileListCtrl()
{
}

void CppCheckFileListCtrl::AddFile(const wxString& filename)
{
	SetReadOnly( false );
	bool updateView ( false );

	// do not add duplicate files
	if ( m_files.find(filename) != m_files.end() ) {
		return;
	}

	if ( GetLength() == 0 ) {
		updateView = true;
	}

	if ( GetLineCount() % 2 != 0 ) {
		MarkerAdd(GetLineCount() - 1, 7);
	}

	InsertText(GetLength(), filename + wxT("\n") );

	if ( updateView && m_report ) {
		m_report->FileSelected( filename );
		MarkerAdd      ( 0, 8 );
	}

	// keep track of files we have added so far
	m_files.insert(filename);
	SetReadOnly( true  );
}

void CppCheckFileListCtrl::Clear()
{
	SetReadOnly( false );
	ClearAll();
	m_files.clear();
	SetReadOnly( true  );
}

void CppCheckFileListCtrl::OnHotspotClicked(wxScintillaEvent& e)
{
	// get the file name that was clicked
	int line = LineFromPosition( e.GetPosition() );

	MarkerDeleteAll( 8       );
	MarkerAdd      ( line, 8 );

	wxString filename = GetLine( line );
	filename.Trim();

	if ( m_report ) {
		// update the report details
		m_report->FileSelected( filename );
	}
}

void CppCheckFileListCtrl::SetReportPage(CppCheckReportPage* report)
{
	m_report = report;
}
