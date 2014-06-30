//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : imageexportdialog.h
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

#ifndef IMAGEEXPORTDIALOG_H
#define IMAGEEXPORTDIALOG_H

#include "GUI.h" // Base class: _ImageExportDialog

class ImageExportDialog : public _ImageExportDialog {

public:
	ImageExportDialog(wxWindow *parent);
	virtual ~ImageExportDialog();

	virtual void OnOKClick(wxCommandEvent& event);
	virtual void OnUpdateCustomScale(wxUpdateUIEvent& event);
	virtual void OnBowseClick(wxCommandEvent& event);
	virtual void OnInit(wxInitDialogEvent& event);

	void SetBitmapType(const wxBitmapType& BitmapType) {this->m_BitmapType = BitmapType;}
	void SetPath(const wxString& Path) {this->m_Path = Path;}
	const wxBitmapType& GetBitmapType() const {return m_BitmapType;}
	const wxString& GetPath() const {return m_Path;}
	double GetScale() const {return m_Scale;}
	bool GetExportCanvas() const {return m_ExportCanvas;}
	
protected:
	wxString m_Path;
	wxBitmapType m_BitmapType;
	double m_Scale;
	bool m_ExportCanvas;
};

#endif // IMAGEEXPORTDIALOG_H
