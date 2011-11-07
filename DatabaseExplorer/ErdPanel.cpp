#include "ErdPanel.h"
#include "Art.h"
#include "databaseexplorer.h"

#include <wx/xrc/xmlres.h>

XS_IMPLEMENT_CLONABLE_CLASS(ErdInfo, xsSerializable)

ErdInfo::ErdInfo()
{
	m_adapterType = IDbAdapter::atUNKNOWN;
	
	XS_SERIALIZE_INT( m_adapterType, wxT("adapter_type") );
}

ErdInfo::ErdInfo(const ErdInfo& obj)
{
	m_adapterType = obj.m_adapterType;
	
	XS_SERIALIZE_INT( m_adapterType, wxT("adapter_type") );
}

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(ErdPanel,_ErdPanel)

BEGIN_EVENT_TABLE(ErdPanel, _ErdPanel)
	EVT_TOOL(XRCID("IDT_OPEN"), ErdPanel::OnLoad)
	EVT_TOOL(XRCID("IDT_SAVE"), ErdPanel::OnSave)
	EVT_TOOL(XRCID("IDT_ERD_COMMIT"), ErdPanel::OnCommit)
	EVT_TOOL(XRCID("IDT_ERD_SAVE_SQL"), ErdPanel::OnSaveSql)
	EVT_TOOL(XRCID("IDT_ERD_SAVE_BMP"), ErdPanel::OnSaveImg)
	EVT_TOOL(XRCID("IDT_ERD_ZOOM100"), ErdPanel::OnZoom100)
	EVT_TOOL(XRCID("IDT_ERD_ZOOMALL"), ErdPanel::OnZoomAll)
	EVT_TOOL(XRCID("IDT_PRINT"), ErdPanel::OnPrint)
	EVT_TOOL(XRCID("IDT_PREVIEW"), ErdPanel::OnPreview)
	EVT_TOOL(XRCID("IDT_COPY"), ErdPanel::OnCopy)
	EVT_TOOL(XRCID("IDT_CUT"), ErdPanel::OnCut)
	EVT_TOOL(XRCID("IDT_PASTE"), ErdPanel::OnPaste)
	EVT_TOOL(XRCID("IDT_UNDO"), ErdPanel::OnUndo)
	EVT_TOOL(XRCID("IDT_REDO"), ErdPanel::OnRedo)
	EVT_TOOL(XRCID("IDT_SELECTALL"), ErdPanel::OnSelectAll)
	EVT_TOOL(XRCID("IDT_ERD_ALIGN_CIRCLE"), ErdPanel::OnAlignCircle)
	EVT_TOOL(XRCID("IDT_ERD_ALIGN_MESH"), ErdPanel::OnAlignMesh)
	EVT_TOOL(XRCID("IDT_ERD_ALIGN_VTREE"), ErdPanel::OnAlignVTree)
	EVT_TOOL(XRCID("IDT_ERD_ALIGN_HTREE"), ErdPanel::OnAlignHTree)
	
	EVT_UPDATE_UI(XRCID("IDT_COPY"), ErdPanel::OnUpdateCopy)
	EVT_UPDATE_UI(XRCID("IDT_CUT"), ErdPanel::OnUpdateCut)
	EVT_UPDATE_UI(XRCID("IDT_PASTE"), ErdPanel::OnUpdatePaste)
	EVT_UPDATE_UI(XRCID("IDT_UNDO"), ErdPanel::OnUpdateUndo)
	EVT_UPDATE_UI(XRCID("IDT_REDO"), ErdPanel::OnUpdateRedo)
	// The following were originally EVT_*_RANGEs, but these won't work with XRCIDs in wx2.9
	// and only worked by chance in earlier versions: see http://trac.wxwidgets.org/ticket/11431
	EVT_TOOL(XRCID("IDT_ERD_TOOL"), ErdPanel::OnTool)
	EVT_TOOL(XRCID("IDT_ERD_TABLE"), ErdPanel::OnTool)
	EVT_TOOL(XRCID("IDT_ERD_LINE"), ErdPanel::OnTool)
	EVT_TOOL(XRCID("IDT_ERD_VIEW"), ErdPanel::OnTool)
	
	EVT_UPDATE_UI(XRCID("IDT_ERD_TOOL"), ErdPanel::OnToolUpdate)
	EVT_UPDATE_UI(XRCID("IDT_ERD_TABLE"), ErdPanel::OnToolUpdate)
	EVT_UPDATE_UI(XRCID("IDT_ERD_LINE"), ErdPanel::OnToolUpdate)
	EVT_UPDATE_UI(XRCID("IDT_ERD_VIEW"), ErdPanel::OnToolUpdate)
	
END_EVENT_TABLE()

ErdPanel::ErdPanel():_ErdPanel(NULL) {
}

ErdPanel::ErdPanel(wxWindow *parent, IDbAdapter* dbAdapter, xsSerializable* pConnections):_ErdPanel(parent) {
	m_pErdTable = NULL;
	m_pDbAdapter = dbAdapter;
	m_pConnections = pConnections;
	Init(parent, dbAdapter);
}

ErdPanel::ErdPanel(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pConnections, DBETable* pTable):_ErdPanel(parent) {
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
		DBETable* pTable = wxDynamicCast(node->GetData(), DBETable);
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
	
	ErdInfo *pInfo = new ErdInfo();
	pInfo->SetAdapterType( m_pDbAdapter->GetAdapterType() );
	m_diagramManager.SetRootItem( pInfo );
		
	m_pFrameCanvas = new FrameCanvas(&m_diagramManager,dbAdapter,m_wxsfPanel,this, wxID_ANY);
	m_wxsfSizer->Add(m_pFrameCanvas,  1, wxEXPAND, 2);
	m_wxsfPanel->Layout();
	
	m_nToolMode = modeDESIGN;
	
	m_toolBarErd->SetToolBitmapSize(wxSize(16, 15));
	m_toolBarErd->AddTool(XRCID("IDT_OPEN"), _("Open"), wxBitmap(fileopen_xpm),  _("Open diagram"));
	m_toolBarErd->AddTool(XRCID("IDT_SAVE"), _("Save"), wxBitmap(filesave_xpm),  _("Save diagram"));
	m_toolBarErd->AddTool(XRCID("IDT_ERD_SAVE_SQL"), _("Save SQL"), wxBitmap(export_sql_xpm),_("Save SQL"));
	m_toolBarErd->AddTool(XRCID("IDT_ERD_COMMIT"), _("Commit ERD"), wxBitmap(export_db_xpm),_("Commit ERD"));	
	m_toolBarErd->AddTool(XRCID("IDT_ERD_SAVE_BMP"), _("Save BMP"), wxBitmap(export_img_xpm),_("Save BMP"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(XRCID("IDT_PRINT"), _("Print"), wxBitmap(fileprint_xpm),  _("Print diagram"));
	m_toolBarErd->AddTool(XRCID("IDT_PREVIEW"), _("Preview"), wxBitmap(filepreview_xpm),  _("Print preview"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(XRCID("IDT_COPY"), _("Copy"), wxBitmap(editcopy_xpm),  _("Copy item"));
	m_toolBarErd->AddTool(XRCID("IDT_CUT"), _("Cut"), wxBitmap(editcut_xpm),  _("Cut item"));
	m_toolBarErd->AddTool(XRCID("IDT_PASTE"), _("Paste"), wxBitmap(editpaste_xpm),  _("Paste item"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(XRCID("IDT_UNDO"), _("Undo"), DatabaseExplorer::GetManager()->GetStdIcons()->LoadBitmap(wxT("toolbars/16/standard/undo")), _("Undo"));
	m_toolBarErd->AddTool(XRCID("IDT_REDO"), _("Redo"), DatabaseExplorer::GetManager()->GetStdIcons()->LoadBitmap(wxT("toolbars/16/standard/redo")), _("Redo"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddRadioTool(XRCID("IDT_ERD_TOOL"), _("Tool"), wxBitmap(Tool_xpm), wxNullBitmap, _("Design tool"));
	m_toolBarErd->AddRadioTool(XRCID("IDT_ERD_TABLE"), _("DBETable"), DatabaseExplorer::GetManager()->GetStdIcons()->LoadBitmap(wxT("db-explorer/16/table")),wxNullBitmap, _("Database table"));
	m_toolBarErd->AddRadioTool(XRCID("IDT_ERD_VIEW"), _("View"), DatabaseExplorer::GetManager()->GetStdIcons()->LoadBitmap(wxT("toolbars/16/search/find")),wxNullBitmap, _("Database view"));
	m_toolBarErd->AddRadioTool(XRCID("IDT_ERD_LINE"), _("Constraint 1:N"), wxXmlResource::Get()->LoadBitmap(wxT("link_editor")),wxNullBitmap, _("Foreign key connection"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(XRCID("IDT_ERD_ALIGN_CIRCLE"), _("Align into circle"), wxBitmap(AlignCircle_xpm),  _("Align into circle"));
	m_toolBarErd->AddTool(XRCID("IDT_ERD_ALIGN_MESH"), _("Align into mesh"), wxBitmap(AlignMesh_xpm),  _("Align into mesh"));
	m_toolBarErd->AddTool(XRCID("IDT_ERD_ALIGN_VTREE"), _("Align into vertical tree"), wxBitmap(AlignVTree_xpm),  _("Align into vertical tree"));
	m_toolBarErd->AddTool(XRCID("IDT_ERD_ALIGN_HTREE"), _("Align into horizontal tree"), wxBitmap(AlignHTree_xpm),  _("Align into horizontal tree"));
	m_toolBarErd->AddSeparator();
	m_toolBarErd->AddTool(XRCID("IDT_ERD_ZOOM100"), _("Zoom 100%"), wxBitmap(Zoom100_xpm),  _("Zoom 100%"));
	m_toolBarErd->AddTool(XRCID("IDT_ERD_ZOOMALL"), _("Zoom to all"), wxBitmap(ZoomAll_xpm),  _("Zoom to all"));
	m_toolBarErd->Realize();
	
	wxAcceleratorEntry entries[4];
	entries[0].Set(wxACCEL_CTRL,  (int) 'C', XRCID("IDT_COPY"));
	entries[1].Set(wxACCEL_CTRL,  (int) 'X', XRCID("IDT_CUT"));
	entries[2].Set(wxACCEL_CTRL, (int) 'V',  XRCID("IDT_PASTE"));
	entries[3].Set(wxACCEL_CTRL, (int) 'A', XRCID("IDT_SELECTALL"));
	wxAcceleratorTable accel(4, entries);
	SetAcceleratorTable(accel);
}

void ErdPanel::OnTool(wxCommandEvent& event) {
	if (event.GetId() == XRCID("IDT_ERD_TOOL")) m_nToolMode = modeDESIGN;
	else if (event.GetId() == XRCID("IDT_ERD_TABLE"))	m_nToolMode = modeTABLE;
	else if (event.GetId() == XRCID("IDT_ERD_LINE")) m_nToolMode = modeLine;
	else if (event.GetId() == XRCID("IDT_ERD_VIEW")) m_nToolMode = modeVIEW;
}

void ErdPanel::OnToolUpdate(wxUpdateUIEvent& event) {
	if (event.GetId() == XRCID("IDT_ERD_TOOL"))	event.Check(m_nToolMode == modeDESIGN);
	else if (event.GetId() == XRCID("IDT_ERD_TABLE")) event.Check(m_nToolMode == modeTABLE);
	else if (event.GetId() == XRCID("IDT_ERD_LINE")) event.Check(m_nToolMode == modeLine);
	else if (event.GetId() == XRCID("IDT_ERD_VIEW")) event.Check(m_nToolMode == modeVIEW);
	else event.Skip();
}

void ErdPanel::OnLoad(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, _("Load canvas from file..."), wxGetCwd(), wxT(""), wxT("ERD Files (*.erd)|*.erd"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(dlg.ShowModal() == wxID_OK) {
		
		LoadERD( dlg.GetPath() );
	}
}

void ErdPanel::OnSave(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, _("Save canvas to file..."), wxGetCwd(), wxT(""), wxT("ERD Files (*.erd)|*.erd"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if(dlg.ShowModal() == wxID_OK) {

		SaveERD( dlg.GetPath() );
	}
}

void ErdPanel::OnSaveSql(wxCommandEvent& event) {
	wxFileDialog dlg(this, _("Save SQL create query..."), wxGetCwd(), wxT(""), wxT("SQL Files (*.sql)|*.sql"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

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
		wxMessageBox(wxString::Format(_("The SQL script has been saved to '%s'."), dlg.GetPath().GetData()), _("DatabaseExplorer"));
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
	wxFileDialog dlg(this, _("Export canvas to BMP..."), wxGetCwd(), wxT(""), wxT("BMP Files (*.bmp)|*.bmp"), wxFD_SAVE);

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
bool ErdPanel::LoadERD(const wxString& path)
{
	m_diagramManager.GetRootItem()->RemoveChildren();
	
	if( m_diagramManager.DeserializeFromXml( path ) )
	{
		ErdInfo *pInfo = wxDynamicCast( m_diagramManager.GetRootItem(), ErdInfo );
		if( pInfo && (pInfo->GetAdapterType() == m_pDbAdapter->GetAdapterType()) )
		{
			m_pFrameCanvas->UpdateERD();
			m_pFrameCanvas->Refresh();
			
			return true;
		}
		else
		{
			m_diagramManager.GetRootItem()->RemoveChildren();
			wxMessageBox( _("ERD type doesn't match current database adapter."), _("DB Error"), wxICON_ERROR | wxOK );
			
			m_pFrameCanvas->Refresh();
			
			return false;
		}
	}
	
	return false;
}

bool ErdPanel::SaveERD(const wxString& path)
{
	if( m_diagramManager.SerializeToXml( path, xsWITH_ROOT ) )
	{
		wxMessageBox(wxString::Format(_("The chart has been saved to '%s'."), path.c_str()), _("DatabaseExplorer"));
		
		return true;
	}
	else
		return false;
}

void ErdPanel::OnSelectAll(wxCommandEvent& evt)
{
	m_pFrameCanvas->SelectAll();
}

