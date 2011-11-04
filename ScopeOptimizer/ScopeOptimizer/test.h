PHPEditorContextMenu* PHPEditorContextMenu::ms_instance = 0;

enum PhpEditorEventIds {
    wxID_COMMENT_LINE = 10100,
    wxID_UNCOMMENT_LINE,
	wxID_COMMENT_SELECTION,
	wxID_UNCOMMENT_SELECTION,
	wxID_UNCOMMENT,
    wxID_OPEN_PHP_FILE
};

PHPEditorContextMenu::PHPEditorContextMenu()
	: m_manager(NULL)
{
	m_comment_line_1 = _("//");
	m_comment_line_2 = _("#");
	m_start_comment = _("/*");
	m_close_comment = _("*/");
}

PHPEditorContextMenu::~PHPEditorContextMenu()
{
	wxTheApp->Disconnect(wxEVT_CMD_EDITOR_CONTEXT_MENU,        wxCommandEventHandler(PHPEditorContextMenu::OnContextMenu),       NULL, this);
	wxTheApp->Disconnect(wxEVT_CMD_EDITOR_MARGIN_CONTEXT_MENU, wxCommandEventHandler(PHPEditorContextMenu::OnMarginContextMenu), NULL, this);
}

void PHPEditorContextMenu::ConnectEvents()
{
	wxTheApp->Connect(wxEVT_CMD_EDITOR_CONTEXT_MENU,        wxCommandEventHandler(PHPEditorContextMenu::OnContextMenu),       NULL, this);
	wxTheApp->Connect(wxEVT_CMD_EDITOR_MARGIN_CONTEXT_MENU, wxCommandEventHandler(PHPEditorContextMenu::OnMarginContextMenu), NULL, this);
}

PHPEditorContextMenu* PHPEditorContextMenu::Instance()
{
	if(ms_instance == 0) {
		ms_instance = new PHPEditorContextMenu();
	}
	return ms_instance;
}

void Language::CheckForTemplateAndTypedef(wxString& typeName, wxString& typeScope)
{
	bool typedefMatch;
	bool templateMatch;
	int  retry(0);
	wxString _oldTypeName;
	
	do {
		typedefMatch = false;
		wxString completeTypedefResolved;
		wxArrayString tokens = wxStringTokenize(typeName, wxT(":"), wxTOKEN_STRTOK);
		
