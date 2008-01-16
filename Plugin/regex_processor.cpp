#include "regex_processor.h"
#include "macros.h"
#include "wx/regex.h"

RegexProcessor::RegexProcessor(const wxString &reStr)
{ 
	m_re = new wxRegEx(reStr);//, wxRE_ADVANCED);
}

RegexProcessor::~RegexProcessor()
{
	delete m_re;
}

bool RegexProcessor::GetGroup(const wxString &str, int grp, wxString &out)
{
	if(m_re && m_re->IsValid()){
		if(m_re->Matches(str)){
			out = m_re->GetMatch(str, grp);
			TrimString(out);
			return true;
		}
	}
	return false;
}
