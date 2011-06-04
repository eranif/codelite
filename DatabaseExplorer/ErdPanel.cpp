#include "ErdPanel.h"
IMPLEMENT_DYNAMIC_CLASS(ErdPanel,_ErdPanel)

BEGIN_EVENT_TABLE(ErdPanel, _ErdPanel)
	EVT_TOOL(wxID_OPEN, ErdPanel::OnLoad)
	EVT_TOOL(wxID_SAVE, ErdPanel::OnSave)
	EVT_TOOL(IDS_ERD_COMMIT, ErdPanel::OnCommit)
	EVT_TOOL(IDS_ERD_SAVE_SQL, ErdPanel::OnSaveSql)
	EVT_TOOL(IDS_ERD_SAVE_BMP, ErdPanel::OnSaveImg)
	EVT_TOOL(IDT_ERD_ZOOM100, ErdPanel::OnZoom100)
	EVT_TOOL(IDT_ERD_ZOOMALL, ErdPanel::OnZoomAll)
	EVT_TOOL(wxID_PRINT, ErdPanel::OnPrint)
	EVT_TOOL(wxID_PREVIEW, ErdPanel::OnPreview)
	EVT_TOOL(wxID_COPY, ErdPanel::OnCopy)
	EVT_TOOL(wxID_CUT, ErdPanel::OnCut)
	EVT_TOOL(wxID_PASTE, ErdPanel::OnPaste)
	EVT_TOOL(wxID_UNDO, ErdPanel::OnUndo)
	EVT_TOOL(wxID_REDO, ErdPanel::OnRedo)
	EVT_TOOL(IDT_ERD_ALIGN_CIRCLE, ErdPanel::OnAlignCircle)
	EVT_TOOL(IDT_ERD_ALIGN_MESH, ErdPanel::OnAlignMesh)
	EVT_TOOL(IDT_ERD_ALIGN_VTREE, ErdPanel::OnAlignVTree)
	EVT_TOOL(IDT_ERD_ALIGN_HTREE, ErdPanel::OnAlignHTree)
	EVT_UPDATE_UI(wxID_COPY, ErdPanel::OnUpdateCopy)
	EVT_UPDATE_UI(wxID_CUT, ErdPanel::OnUpdateCut)
	EVT_UPDATE_UI(wxID_PASTE, ErdPanel::OnUpdatePaste)
	EVT_UPDATE_UI(wxID_UNDO, ErdPanel::OnUpdateUndo)
	EVT_UPDATE_UI(wxID_REDO, ErdPanel::OnUpdateRedo)
	EVT_TOOL_RANGE(IDT_ERD_FIRST, IDT_ERD_LAST, ErdPanel::OnTool)
	EVT_UPDATE_UI_RANGE(IDT_ERD_FIRST, IDT_ERD_LAST, ErdPanel::OnToolUpdate)
	
END_EVENT_TABLE()

ErdPanel::ErdPanel():_ErdPanel(NULL) {
}

ErdPanel::ErdPanel(wxWindow *parent, IDbAdapter* dbAdapter, xsSerializable* pConnections):_ErdPanel(parent) {
	m_pErdTable = NULL;
	m_pDbAdapter = dbAdapter;
	m_pConnections = pConnections;
	Init(parent, dbAdapter);
}

ErdPanel::ErdPanel(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pConnections, Table* pTable):_ErdPanel(parent) {
	m_pErdTable = NULL;
	m_pDbAdapter = dbAdapter;
	m_pConnections = pConnections;
	Init(parent, dbAdapter);
	if (pTable) {
		ErdTable* pErdTab = new ErdTable(pTable);
		m_diagramManager.AddShape(pErdTab, NULL, wxPoint( 10,10), sfINITIALIZE, sfDONT_SAVE_STATE);
		pErdTab->UpdateColumns();
	}
}

ErdPanel::ErdPanel(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pConnections, xsSerializable* pItems):_ErdPanel(parent) {
	m_pErdTable = NULL;
	m_pDbAdapter = dbAdapter;
	m_pConnections = pConnections;
	Init(parent, dbAdapter);
	int i = 10;
	SerializableList::compatibility_iterator node = pItems->GetFirstChildNode();
	while( node ) {
		Table* pTable = wxDynamicCast(node->GetData(), Table);
		if( pTable ) {
			ErdTable* pErdTab = new ErdTable(pTable);
			m_diagramManager.AddShape(pErdTab, NULL, wxPoint( i ,10), sfINITIALIZE, sfDONT_SAVE_STATE);
			i+= 200;
			pErdTab->UpdateColumns();
		}
		View* pView = wxDynamicCast(node->GetData(),View);
		if (pView) {
			ErdView* pErdView = new ErdView(pView);
			m_diagramManager.AddShape(pErdView, NULL, wxPoint( i ,10), sfINITIALIZE, sfDONT_SAVE_STATE);
			i+= 200;
			pErdView->UpdateView();
		}
		node = node->GetNext();
	}
	m_pFrameCanvas->UpdateERD();
	m_pFrameCanvas->UpdateVirtualSize();
}

ErdPanel::~ErdPanel() {
	delete m_pDbAdapter;
}

void ErdPanel::Init(wxWindow* parent, IDbAdapter* dbAdapter) {
	m_pFrameCanvas = new FrameCanvas(&m_diagramManager,dbAdapter,m_wxsfPanel,this, wxID_ANY);
	m_wxsfSizer->Add(m_pFrameCanvas,  1, wxEXPAND, 2);
	m_wxsfPanel->Layout();

	m_toolBarErd->SetToolBitmapSize(wxSize(16, 15));
	m_toolBarErd->AddTool(wxID_OPEN, wxT("Open"), wxBitmap(fileopen_xpm),  wxT("Open diagram"));
	m_toolBarErd->AddTool(wxID_SAVE, wxT("Save"), wxBitmap(filesave_xpm),  wxT("Save diagram"));
	m_toolBarErd->AddTool(IDS_ERD_SAVE_SQL, wxT("Save SQL"), wxBitmap(export_sql_xpm),wxT("Save SQL"));
	m_toolBarErd->AddTool(IDS_ERD_COMMIT, wxT("Commit ERD"), wxBitmap(export_db_xpm),wxT("Commit ERD"));	
	m_toolBarErd->AddTool(IDS_ERD_SAVE_BMP, wxT("Save BMP"), wxBitmap(export_img_xpm),wxT("Save BMP"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(wxID_PRINT, wxT("Print"), wxBitmap(fileprint_xpm),  wxT("Print diagram"));
	m_toolBarErd->AddTool(wxID_PREVIEW, wxT("Preview"), wxBitmap(filepreview_xpm),  wxT("Print preview"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(wxID_COPY, wxT("Copy"), wxBitmap(editcopy_xpm),  wxT("Copy item"));
	m_toolBarErd->AddTool(wxID_CUT, wxT("Cut"), wxBitmap(editcut_xpm),  wxT("Cut item"));
	m_toolBarErd->AddTool(wxID_PASTE, wxT("Paste"), wxBitmap(editpaste_xpm),  wxT("Paste item"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(wxID_UNDO, wxT("Undo"), wxBitmap(undo_xpm),  wxT("Undo"));
	m_toolBarErd->AddTool(wxID_REDO, wxT("Redo"), wxBitmap(redo_xpm),  wxT("Redo"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddRadioTool(IDT_ERD_TOOL, wxT("Tool"), wxBitmap(Tool_xpm), wxNullBitmap, wxT("Design tool"));
	m_toolBarErd->AddRadioTool(IDT_ERD_TABLE, wxT("Table"), wxBitmap(form_blue_xpm),wxNullBitmap, wxT("Database table"));
	m_toolBarErd->AddRadioTool(IDT_ERD_VIEW, wxT("View"), wxBitmap(form_yellow_xpm),wxNullBitmap, wxT("Database view"));
	m_toolBarErd->AddRadioTool(IDT_ERD_LINE, wxT("Constraint 1:N"), wxBitmap(link_editor_xpm),wxNullBitmap, wxT("Foreign key connection"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(IDT_ERD_ALIGN_CIRCLE, wxT("Align into circle"), wxBitmap(AlignCircle_xpm),  wxT("Align into circle"));
	m_toolBarErd->AddTool(IDT_ERD_ALIGN_MESH, wxT("Align into mesh"), wxBitmap(AlignMesh_xpm),  wxT("Align into mesh"));
	m_toolBarErd->AddTool(IDT_ERD_ALIGN_VTREE, wxT("Align into vertical tree"), wxBitmap(AlignVTree_xpm),  wxT("Align into vertical tree"));
	m_toolBarErd->AddTool(IDT_ERD_ALIGN_HTREE, wxT("Align into horizontal tree"), wxBitmap(AlignHTree_xpm),  wxT("Align into horizontal tree"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(IDT_ERD_ZOOM100, wxT("Zoom 100%"), wxBitmap(Zoom100_xpm),  wxT("Zoom 100%"));
	m_toolBarErd->AddTool(IDT_ERD_ZOOMALL, wxT("Zoom to all"), wxBitmap(ZoomAll_xpm),  wxT("Zoom to all"));
	m_toolBarErd->Realize();
}

void ErdPanel::OnTool(wxCommandEvent& event) {
	if (event.GetId() == IDT_ERD_TOOL) m_nToolMode = modeDESIGN;
	else if (event.GetId() ==  IDT_ERD_TABLE)	m_nToolMode = modeTABLE;
	else if (event.GetId() == IDT_ERD_LINE) m_nToolMode = modeLine;
	else if (event.GetId() == IDT_ERD_VIEW) m_nToolMode = modeVIEW;
}

void ErdPanel::OnToolUpdate(wxUpdateUIEvent& event) {
	if (event.GetId() == IDT_ERD_TOOL)	event.Check(m_nToolMode == modeDESIGN);
	else if (event.GetId() == IDT_ERD_TABLE) event.Check(m_nToolMode == modeTABLE);
	else if (event.GetId() == IDT_ERD_LINE) event.Check(m_nToolMode == modeLine);
	else if (event.GetId() == IDT_ERD_VIEW) event.Check(m_nToolMode == modeVIEW);
	else event.Skip();


}
void ErdPanel::OnLoad(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, wxT("Load canvas from file..."), wxGetCwd(), wxT(""), wxT("ERD Files (*.erd)|*.erd"), wxOPEN | wxFD_FILE_MUST_EXIST);

	if(dlg.ShowModal() == wxID_OK) {
		m_pFrameCanvas->LoadCanvas(dlg.GetPath());
		m_pFrameCanvas->UpdateERD();
	}
}

void ErdPanel::OnSave(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, wxT("Save canvas to file..."), wxGetCwd(), wxT(""), wxT("ERD Files (*.erd)|*.erd"), wxSAVE | wxFD_OVERWRITE_PROMPT);

	if(dlg.ShowModal() == wxID_OK) {
		m_pFrameCanvas->SaveCanvas(dlg.GetPath());

		wxMessageBox(wxString::Format(wxT("The chart has been saved to '%s'."), dlg.GetPath().GetData()), wxT("DatabaseExplorer"));
	}
}

void ErdPanel::OnSaveSql(wxCommandEvent& event) {
	wxFileDialog dlg(this, wxT("Save SQL create query..."), wxGetCwd(), wxT(""), wxT("SQL Files (*.sql)|*.sql"), wxSAVE | wxFD_OVERWRITE_PROMPT);

	if(dlg.ShowModal() == wxID_OK) {
		wxTextFile file(dlg.GetPath());
		if (!file.Exists()) file.Create();
		file.Open();
		if (file.IsOpened()) {
			file.Clear();
			file.AddLine(wxT("-- SQL script created by DatabaseExplorer "));
			file.AddLine(wxT(""));
			file.AddLine(m_pFrameCanvas->GetSqlScript());
			file.Write();
			file.Close();
		}
		wxMessageBox(wxString::Format(wxT("The SQL script has been saved to '%s'."), dlg.GetPath().GetData()), wxT("DatabaseExplorer"));
	}
}

void ErdPanel::OnZoom100(wxCommandEvent& event) {
	m_pFrameCanvas->SetScale( 1 );
	m_pFrameCanvas->Refresh( false );
}

void ErdPanel::OnZoomAll(wxCommandEvent& event) {
	m_pFrameCanvas->SetScaleToViewAll();
	m_pFrameCanvas->Refresh( false );
}

void ErdPanel::OnPreview(wxCommandEvent& event) {
	m_pFrameCanvas->PrintPreview();
}

void ErdPanel::OnPrint(wxCommandEvent& event) {
	m_pFrameCanvas->Print();
}

void ErdPanel::OnCopy(wxCommandEvent& event) {
	m_pFrameCanvas->Copy();
}

void ErdPanel::OnCut(wxCommandEvent& event) {
	m_pFrameCanvas->Cut();
}

void ErdPanel::OnPaste(wxCommandEvent& event) {
	m_pFrameCanvas->Paste();
	m_pFrameCanvas->UpdateERD();
}

void ErdPanel::OnUpdateCopy(wxUpdateUIEvent& event) {
	event.Enable( m_pFrameCanvas->CanCopy() );
}

void ErdPanel::OnUpdateCut(wxUpdateUIEvent& event) {
	event.Enable( m_pFrameCanvas->CanCut() );
}

void ErdPanel::OnUpdatePaste(wxUpdateUIEvent& event) {
	event.Enable( m_pFrameCanvas->CanPaste() );
}

void ErdPanel::OnRedo(wxCommandEvent& event) {
	m_pFrameCanvas->Redo();
	m_pFrameCanvas->UpdateERD();
}

void ErdPanel::OnUndo(wxCommandEvent& event) {
	m_pFrameCanvas->Undo();
	m_pFrameCanvas->UpdateERD();
}

void ErdPanel::OnUpdateRedo(wxUpdateUIEvent& event) {
	event.Enable( m_pFrameCanvas->CanRedo() );
}

void ErdPanel::OnUpdateUndo(wxUpdateUIEvent& event) {
	event.Enable( m_pFrameCanvas->CanUndo() );
}

void ErdPanel::OnSaveImg(wxCommandEvent& event) {
	wxFileDialog dlg(this, wxT("Export canvas to BMP..."), wxGetCwd(), wxT(""), wxT("BMP Files (*.bmp)|*.bmp"), wxSAVE);

	if(dlg.ShowModal() == wxID_OK) {
		m_pFrameCanvas->SaveCanvasToBMP(dlg.GetPath());
	}
}

void ErdPanel::OnAlignCircle(wxCommandEvent& event) {
	wxSFAutoLayout layout;
	layout.Layout( m_pFrameCanvas, wxT("Circle") );
	m_pFrameCanvas->SaveCanvasState();
}

void ErdPanel::OnAlignHTree(wxCommandEvent& event) {
	wxSFAutoLayout layout;
	layout.Layout( m_pFrameCanvas, wxT("Horizontal Tree") );
	m_pFrameCanvas->SaveCanvasState();
}

void ErdPanel::OnAlignMesh(wxCommandEvent& event) {
	wxSFAutoLayout layout;
	layout.Layout( m_pFrameCanvas, wxT("Mesh") );
	m_pFrameCanvas->SaveCanvasState();
}

void ErdPanel::OnAlignVTree(wxCommandEvent& event) {
	wxSFAutoLayout layout;
	layout.Layout( m_pFrameCanvas, wxT("Vertical Tree") );
	m_pFrameCanvas->SaveCanvasState();
}

void ErdPanel::OnCommit(wxCommandEvent& event)
{
	if (m_pConnections){
		ErdCommitWizard wizard(this, m_pConnections, m_pFrameCanvas->GetSqlScript()); 
		wizard.RunWizard(wizard.GetFirstPage());
	}
}

void ErdPanel::OnPageClosing(NotebookEvent& event)
{
	wxMessageBox(wxT("Event Veto"));
	event.Veto();
	
}

