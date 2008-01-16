#include "gdbinfolexer.h"

GdbInfoLexer::GdbInfoLexer(const wxString &input)
: m_input(input)
{
	m_input = m_input.Trim();
	m_input = m_input.Trim(false);
}

GdbInfoLexer::~GdbInfoLexer()
{
}

int GdbInfoLexer::Lex(wxString &word)
{
	word.Empty();
	if(m_input.IsEmpty()){
		return GdbEof;
	}

	while(m_input.Length() > 0){
		//Skip whitespaces
		wxChar ch = m_input.GetChar(0);
		m_input = m_input.Remove(0, 1);

		switch(ch){
			case wxT('{'):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT("{"));
					return GdbWord;
				}else{
					word = ch;
					return GdbOpenBrace;
				}
			case wxT('}'):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT("}"));
					return GdbWord;
				}else{
					word = ch;
					return GdbCloseBrace;
				}
			case wxT('='):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT("="));
					return GdbWord;
				}else{
					word = ch;
					return GdbEqualSign;
				}
			case wxT(','):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT(","));
					return GdbWord;
				}else{
					word = ch;
					return GdbComma;
				}
			case wxT('\n'):
			case wxT('\t'):
			case wxT('\v'):
			case wxT('\r'):
				//whitespaces
				break;
			case wxT(' '):
				word = word.Trim();
				if(word.IsEmpty() == false){
					return GdbWord;
				}
				break;
			default:
				word.Append(ch);
				break;
		}
	}

	word = word.Trim();
	if(word.IsEmpty() == false){
		return GdbWord;
	}
	return GdbEof;
}
