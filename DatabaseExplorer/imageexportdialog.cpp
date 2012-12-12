#include "imageexportdialog.h"
#include <wx/msgdlg.h>

ImageExportDialog::ImageExportDialog(wxWindow *parent) : _ImageExportDialog(parent)
{
	m_Scale = -1;
}

ImageExportDialog::~ImageExportDialog()
{
}

void ImageExportDialog::OnInit(wxInitDialogEvent& event)
{
	m_textCtrlPath->SetValue( m_Path );
}

void ImageExportDialog::OnOKClick(wxCommandEvent& event)
{
	m_ExportCanvas = m_checkBoxBackground->IsChecked();
	
	if( m_Path.IsEmpty() )
	{
		wxMessageBox( _("Image path cannot be empty."), _("Export image"), wxOK | wxICON_ERROR );
		m_textCtrlPath->SetFocus();
		
		return;
	}
	
	if( m_radioBtnScaleCustom->GetValue() )
	{
		if( !m_textCtrlScale->GetValue().ToDouble( &m_Scale ) )
		{
			wxMessageBox( _("Canvas scale must be decimal value."), _("Export image"), wxOK | wxICON_ERROR );
			m_textCtrlScale->SetFocus();
			
			return;
		}
	}
	else
		m_Scale = -1;
	
	EndModal( wxID_OK );
}

void ImageExportDialog::OnUpdateCustomScale(wxUpdateUIEvent& event)
{
	event.Enable( m_radioBtnScaleCustom->GetValue() );
}

void ImageExportDialog::OnBowseClick(wxCommandEvent& event)
{	
	wxFileName path( m_Path );
	wxFileDialog dlg(this, _("Export ERD to image..."), path.GetPath(), path.GetFullName(), wxT("BMP Files (*.bmp)|*.bmp|GIF Files (*.gif)|(*.gif)|XPM Files (*.xpm)|*.xpm|PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	switch( m_BitmapType )
	{
		case wxBITMAP_TYPE_BMP:
			dlg.SetFilterIndex( 0 );
			break;
		case wxBITMAP_TYPE_GIF:
			dlg.SetFilterIndex( 1 );
			break;
		case wxBITMAP_TYPE_XPM:
			dlg.SetFilterIndex( 2 );
			break;
		case wxBITMAP_TYPE_PNG:
			dlg.SetFilterIndex( 3 );
			break;
		case wxBITMAP_TYPE_JPEG:
			dlg.SetFilterIndex( 4 );
			break;
		default:
			break;
	}
	
	if(dlg.ShowModal() == wxID_OK)
	{
		m_Path = dlg.GetPath();
		m_BitmapType = wxBITMAP_TYPE_BMP;
		
		switch( dlg.GetFilterIndex() )
		{
			case 0:
				m_BitmapType = wxBITMAP_TYPE_BMP;
				break;
			case 1:
				m_BitmapType = wxBITMAP_TYPE_GIF;
				break;
			case 2:
				m_BitmapType = wxBITMAP_TYPE_XPM;
				break;
			case 3:
				m_BitmapType = wxBITMAP_TYPE_PNG;
				break;
			case 4:
				m_BitmapType = wxBITMAP_TYPE_JPEG;
				break;
		}
		
		m_textCtrlPath->SetValue( m_Path );
	}
}
