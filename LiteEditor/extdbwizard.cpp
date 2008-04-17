#include "extdbwizard.h"
#include <wx/busyinfo.h>
#include "editor_config.h"
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(ExtDbWizard, wxWizard)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ExtDbWizard::OnPageChanging)
	EVT_WIZARD_PAGE_CHANGED(wxID_ANY, ExtDbWizard::OnPageChanged)
END_EVENT_TABLE()

ExtDbWizard::ExtDbWizard(wxWindow *parent, wxWindowID id)
: m_busyInfoDlg(NULL)
{
	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("sym_wiz_bmp"));
	wxWizard::Create(parent, id, wxT("Create Symbols Database"), bmp);

	//create the pages
	m_page1 = new ExtDbPage1(this);
	m_page2 = new ExtDbPage2(this);
	m_page3 = new ExtDbPage3(this);

	//chain the pages
	wxWizardPageSimple::Chain(m_page1, m_page2);
	wxWizardPageSimple::Chain(m_page2, m_page3);
}

ExtDbWizard::~ExtDbWizard()
{
}

bool ExtDbWizard::Run(ExtDbData &data)
{
	wxSize sz1 = m_page1->GetSizer()->CalcMin();
	wxSize sz2 = m_page2->GetSizer()->CalcMin();
	wxSize sz3 = m_page3->GetSizer()->CalcMin();

	wxSize maxSize = sz1;
	
	if(maxSize.GetWidth() < sz2.GetWidth()) maxSize = sz2;
	if(maxSize.GetWidth() < sz3.GetWidth()) maxSize = sz3;
		
	SetPageSize(maxSize);
	bool res = RunWizard(m_page1);
	if(res){
		data.rootPath	 = ((ExtDbPage1*)m_page1)->GetPath();
		data.dbName 	 = ((ExtDbPage3*)m_page3)->GetDbName();
		data.attachDb 	 = ((ExtDbPage3*)m_page3)->AttachDb();
		((ExtDbPage2*)m_page2)->GetIncludeDirs(data.includeDirs);
		data.fileMasking = ((ExtDbPage2*)m_page2)->GetFileMask();
		
		//save the file masking to disk
		EditorConfigST::Get()->SaveStringValue(wxT("WizardFileMask"), data.fileMasking);
	}
	return res;
}

void ExtDbWizard::OnPageChanging(wxWizardEvent &e)
{
//	if(e.GetDirection()){
//		//going forward
//		if(e.GetPage() == m_page1){
//			//switching from page1 to page2, 
//			//display 
//			m_busyInfoDlg = new wxBusyInfo(wxT("Gathering file list..."));
//		}
//	}
	e.Skip();
}
void ExtDbWizard::OnPageChanged(wxWizardEvent &e)
{
//	if(e.GetPage() == m_page2){
//		//switching from page1 to page2, 
//		//remove the busy dialog
//		if(m_busyInfoDlg) {
//			delete m_busyInfoDlg;
//			m_busyInfoDlg = NULL;
//		}
//	}
	e.Skip();
}
