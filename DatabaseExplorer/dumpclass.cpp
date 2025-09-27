//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : dumpclass.cpp
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

#include "dumpclass.h"

DumpClass::DumpClass(IDbAdapter* pDbAdapter, xsSerializable* pItems, const wxString& fileName) {
	m_pDbAdapter = pDbAdapter;
	m_pItems = pItems;
	m_fileName = fileName;

}

wxString DumpClass::DumpData() {
	int totalRowCount = 0;
	int tableCount = 0;
	wxTextFile* pOutFile = new wxTextFile(m_fileName);
	if (pOutFile->Exists()) {
		pOutFile->Open();
		pOutFile->Clear();
	} else {
		pOutFile->Create();
		pOutFile->Open();
	}

	if (pOutFile->IsOpened()) {
		for (auto node = m_pItems->GetFirstChildNode(); node; node = node->GetNext()) {
			Table* pTab = wxDynamicCast(node->GetData(), Table);
			if (pTab) {
				totalRowCount += DumpTable(pOutFile, pTab);
				tableCount++;
			}
		}

		pOutFile->Write(wxTextFileType_None, wxConvUTF8);
		pOutFile->Close();
	}
	if (pOutFile)  delete pOutFile;
	return wxString::Format(wxT("Dumped %i rows in %i tables"),totalRowCount, tableCount );
}

int DumpClass::DumpTable(wxTextFile* pFile, Table* pTab) {
	int rowCount = 0;
	if ((pFile->IsOpened())&&(pTab != NULL)) {

		wxString cols = wxT("");
		for (auto node = pTab->GetFirstChildNode(); node; node = node->GetNext()){
			Column* pCol = wxDynamicCast(node->GetData(), Column);
			if (pCol){
				if (!cols.IsEmpty()) cols += wxT(", ");
				cols += pCol->GetName();
			}
		}

		wxString startLine = wxString::Format(wxT("INSERT INTO %s (%s) VALUES"), pTab->GetName().c_str(), cols.c_str());	
		int n = 0;
		bool pocatek = false;

		DatabaseLayerPtr pDbLayer = m_pDbAdapter->GetDatabaseLayer(pTab->GetParentName());
		if (pDbLayer){
			//DatabaseResultSet* pResult = pDbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT %s FROM %s.%s"), cols.c_str(),pTab->GetParentName().c_str(), pTab->GetName().c_str()));
			DatabaseResultSet* pResult = pDbLayer->RunQueryWithResults(m_pDbAdapter->GetDefaultSelect(cols ,pTab->GetParentName(), pTab->GetName()));

			while (pResult->Next()){
				if (n == 0 ) pFile->AddLine(startLine);
				rowCount++;

				int colIndex = 1;
				wxString dataLine = wxT("");
				for (auto node = pTab->GetFirstChildNode(); node; node = node->GetNext()) {
					Column* pCol = wxDynamicCast(node->GetData(), Column);
					if (pCol) {
						if (!dataLine.IsEmpty()) dataLine += wxT(", ");

						if (pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_TEXT) {
							dataLine += wxString::Format(wxT("'%s'"), pResult->GetResultString(colIndex).c_str());
						} else if (pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_DATE_TIME) {
							dataLine += wxString::Format(wxT("'%s'"), pResult->GetResultDate(colIndex).FormatDate().c_str());
						} else if (pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_DECIMAL) {
							wxString cislo = wxString::Format(wxT("%f"), pResult->GetResultDouble(colIndex));
							cislo.Replace(wxT(","),wxT("."));
							dataLine += cislo;
						} else if (pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_FLOAT) {
							wxString cislo = wxString::Format(wxT("%f"), pResult->GetResultDouble(colIndex));
							cislo.Replace(wxT(","),wxT("."));
							dataLine += cislo;
						} else if (pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_INT) {
							dataLine += wxString::Format(wxT("%i"), pResult->GetResultInt(colIndex));
						} else if (pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_BOOLEAN) {
							if (pResult->GetResultBool(colIndex)) dataLine += wxT("true");
							else dataLine += wxT("false");
						} else if (pCol->GetType()->GetUniversalType() != IDbType::dbtTYPE_OTHER) {
							dataLine += wxString::Format(wxT("%s"), pResult->GetResultString(colIndex).c_str());
						}
						colIndex++;
					}
				}

				if (n++ > 20 ) {
					pFile->AddLine(wxString::Format(wxT(",(%s);"), dataLine.c_str()));
					pocatek = false;
					n = 0;
				} else {
					if (pocatek == false){
						pocatek = true;
						pFile->AddLine(wxString::Format(wxT("(%s)"), dataLine.c_str()));
					} else {
						pFile->AddLine(wxString::Format(wxT(",(%s)"), dataLine.c_str()));
					}
				}
			}
			if (rowCount > 0) pFile->AddLine(wxT(";"));
			pDbLayer->CloseResultSet(pResult);
			pDbLayer->Close();
		}
	}
	return rowCount;
}
