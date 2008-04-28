#include <map>
#include "ctags_manager.h"
#include "cl_calltip.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

clCallTip::clCallTip(const std::vector<wxString> &tips)
		: m_tips(tips)
		, m_curr(0)
{
}

clCallTip::clCallTip(const clCallTip& rhs)
{
	*this = rhs;
}

clCallTip& clCallTip::operator =(const clCallTip& rhs)
{
	if (this == &rhs)
		return *this;
	m_tips = rhs.m_tips;
	return *this;
}

wxString clCallTip::First()
{
	m_curr = 0;
	if (m_tips.empty())
		return wxEmptyString;
	return TipAt(0);
}

wxString clCallTip::TipAt(int at)
{
	wxString tip;
	if ( m_tips.size() > 1 )
		tip << _T("\n\001 ") << static_cast<int>(m_curr)+1 << _T(" of ") << static_cast<int>(m_tips.size()) << _T(" \002 ")
		<< m_tips.at(at) << _T("\n");
	else
		tip << _T("\n") << m_tips.at( 0 ) << _T("\n");
	return tip;
}

wxString clCallTip::Next()
{
	// format a tip string and return it
	wxString tip;
	if ( m_tips.empty() )
		return wxEmptyString;

	m_curr++;
	if ( m_curr >= (int)m_tips.size() ) {
		m_curr = 0;
	} // if( m_curr >= m_tips.size() )

	return TipAt(m_curr);
}

wxString clCallTip::Prev()
{
	// format a tip string and return it
	wxString tip;
	if ( m_tips.empty() )
		return wxEmptyString;

	m_curr--;
	if (m_curr < 0) {
		m_curr = (int)m_tips.size()-1;
	}
	return TipAt(m_curr);
}

int clCallTip::Count() const
{
	return (int)m_tips.size();
}

wxString clCallTip::All()
{
	wxString tip;
	std::map<wxString, wxString> mymap;
	for (size_t i=0; i< m_tips.size(); i++) {
		wxString tmp_raw_sig = m_tips.at(i);
		wxString raw_sig(tmp_raw_sig);
		if (tmp_raw_sig.StartsWith(wxT("function:"), &raw_sig)) {
			
			bool hasDefaultValues = (raw_sig.Find(wxT("=")) != wxNOT_FOUND);
			wxString normalizedSig = TagsManagerST::Get()->NormalizeFunctionSig(raw_sig);
			
			if (hasDefaultValues) {
				//incase default values exist in this prototype, 
				//make it the tip instead of the existing one
				mymap[normalizedSig] = raw_sig.Trim().Trim(false);
			}			
			
			//make sure we dont add duplicates
			if ( mymap.find(normalizedSig) == mymap.end() ) {
				//add it
				wxString sig = TagsManagerST::Get()->NormalizeFunctionSig(raw_sig, true );
				mymap[normalizedSig] = sig.Trim().Trim(false);;
			}
			
		} else if( tmp_raw_sig.StartsWith(wxT("macro:"), &raw_sig) ) {
		
			//for macros, dont parse them since it will probably fail, 
			//just add them to the map
			if ( mymap.find(raw_sig) == mymap.end() ) {
				//add it
				mymap[raw_sig] = raw_sig;
			}
		}else{
			//simply add it
			if ( mymap.find(raw_sig) == mymap.end() ) {
				//add it
				mymap[raw_sig] = raw_sig;
			}
		}
	}
	
	std::map<wxString, wxString>::iterator iter = mymap.begin();
	for( ; iter != mymap.end(); iter++ ) {
		tip << iter->second << wxT("\n");
	}
	tip = tip.BeforeLast(wxT('\n'));
	return tip;
}
