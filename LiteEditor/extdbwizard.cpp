#include "extdbwizard.h"
#include <wx/xrc/xmlres.h>

ExtDbWizard::ExtDbWizard(wxWindow *parent, wxWindowID id)
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
	}
	return res;
}
