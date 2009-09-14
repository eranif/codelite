#include "svncopydlg.h"
#include "iconfigtool.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>

static void _EscapeComment(wxString &comment)
{
	// first remove the comment section of the text
	wxStringTokenizer tok(comment, wxT("\n"), wxTOKEN_STRTOK);
	comment.Clear();
	while (tok.HasMoreTokens()) {
		wxString line = tok.GetNextToken();
		line = line.Trim().Trim(false);
		if (!line.StartsWith(wxT("#"))) {
			comment << line << wxT("\n");
		}
	}

	// SVN does not like any quotation marks in the comment -> escape them
	comment.Replace(wxT("\""), wxT("\\\""));
}

SvnCopyDlg::SvnCopyDlg( wxWindow* parent, IConfigTool *conf )
		: SvnCopyBaseDlg( parent )
		, m_conf(conf)
{
	GetSizer()->Fit(this);
	WindowAttrManager::Load(this, wxT("SvnCopyDlg"), m_conf);
}

SvnCopyDlg::~SvnCopyDlg()
{
	WindowAttrManager::Save(this, wxT("SvnCopyDlg"), m_conf);
}

wxString SvnCopyDlg::GetComment()
{
	wxString comment( m_textCtrlLog->GetValue() );
	_EscapeComment( comment );
	return comment;
}

wxString SvnCopyDlg::GetSourceURL()
{
	return m_textCtrlSourceURL->GetValue().Trim();
}

wxString SvnCopyDlg::GetTargetURL()
{
	return m_textCtrlTargetURL->GetValue().Trim();
}

void SvnCopyDlg::SetComment(const wxString& txt)
{
	m_textCtrlLog->SetValue( txt );
}

void SvnCopyDlg::SetSourceURL(const wxString& url)
{
	m_textCtrlSourceURL->SetValue( url );
}

void SvnCopyDlg::SetTargetURL(const wxString& url)
{
	m_textCtrlTargetURL->SetValue( url );
}
