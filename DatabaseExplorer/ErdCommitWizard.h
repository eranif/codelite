//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdCommitWizard.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef ERDCOMMITWIZARD_H
#define ERDCOMMITWIZARD_H

#include "CodePreviewDialog.h"
#include "database.h"
#include "dbconnection.h"
#include "dbitem.h"
#include "dumpclass.h"
#include "view.h"

#include <wx/dblayer/include/DatabaseLayer.h>
#include <wx/dblayer/include/DatabaseLayerException.h>
#include <wx/filepicker.h>
#include <wx/imaglist.h>
#include <wx/statline.h>
#include <wx/treectrl.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/wizard.h>
#include <wx/wx.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

/*! \brief Wizard for commiting database structure from ERD diagrom to database server. */
class ErdCommitWizard : public wxWizard {

public:
	ErdCommitWizard(wxWindow* parent, xsSerializable* pConnections, const wxString& createScript);
	virtual ~ErdCommitWizard();

	wxWizardPageSimple* GetFirstPage();
	
	void SetSelectedDatabase(Database* db) { m_pSelectedDatabase = db; }
	Database* GetSelectedDatabase() { return m_pSelectedDatabase; }
	const wxString& GetCreateScript() { return m_createScript; }
	
protected:
	wxWizardPageSimple* m_pFirstPage;
	wxString m_createScript;
	xsSerializable* m_pConnections;
	
	Database* m_pSelectedDatabase;
	
};

/*! \brief Fist page of ErdCommitWizard */
class FirstPage : public wxWizardPageSimple {

public:
	FirstPage(wxWizard* parent);
	virtual ~FirstPage();	
	};

class DatabasePage : public wxWizardPageSimple{

public:

	DatabasePage(ErdCommitWizard* parent, xsSerializable* pConnections );
	virtual ~DatabasePage();	
protected:
	virtual bool TransferDataFromWindow();
    void LoadDatabases();
	
	ErdCommitWizard* m_pParentWizard;
	xsSerializable* m_pConnections;
	wxTreeCtrl* m_treeDatabases;
	wxFlexGridSizer* m_mainSizer;
	
	};
/*! \brief Second page of ErdCommitWizard */
class BackupPage : public wxWizardPageSimple{
public:
	BackupPage(ErdCommitWizard* parent);
	virtual ~BackupPage();
	
protected:
	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();
	
	void OnFileStructureUI(wxUpdateUIEvent& event);
	void OnBtnBackupUI(wxUpdateUIEvent& event);
	void OnBtnBackupClick(wxCommandEvent& event);
	
	ErdCommitWizard* m_pParentWizard;
	wxFlexGridSizer* m_mainSizer;
	
	wxFilePickerCtrl* m_pFileData;
	wxFilePickerCtrl* m_pFileStructure;
	wxCheckBox* m_pCheckStructure;
	
	wxButton* m_btnBackup;
	
	bool backuped;
	};

/*! \brief Third page of ErdCommitWizard */
class WriteStructurePage : public wxWizardPageSimple{
public:
	WriteStructurePage(ErdCommitWizard* parent);
	virtual ~WriteStructurePage();
	
	
protected:
	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();
	
	void OnBtnWriteClick(wxCommandEvent& event);
	void OnBtnShowSqlClick(wxCommandEvent& event);
	
	
	ErdCommitWizard* m_pParentWizard;	 
	 
	wxFlexGridSizer* m_mainSizer;
	
	wxTextCtrl* m_txLog;
	
	wxButton* m_btnWrite;
	wxButton* m_btnShowSql;
	
	bool commited;
	};
/*! \brief Last page of ErdCommitWizard */
class LastPage : public wxWizardPageSimple{
public:
	LastPage(wxWizard* parent);
	virtual ~LastPage();	
	
	}; 
/*! \brief Restore page of ErdCommitWizard */
class RestorePage : public wxWizardPageSimple{
public:
	RestorePage(ErdCommitWizard* parent);
	virtual ~RestorePage();

protected:
	void OnBtnRestoreUI(wxUpdateUIEvent& event);
	void OnBtnRestoreClick(wxCommandEvent& event);
	void Clear();
	void AppendText(const wxString& txt);
	void AppendComment(const wxString& txt);
	void AppendSeparator();
	
	
	wxString m_text;
	ErdCommitWizard* m_pParentWizard;
	wxFlexGridSizer* m_mainSizer;
	wxFilePickerCtrl* m_restoreFile;
	wxTextCtrl* m_txLog;
	
	wxButton* m_btnRestore;
	};

#endif // ERDCOMMITWIZARD_H
