#ifndef GDBINFOLEXER_H
#define GDBINFOLEXER_H

#include "wx/string.h"

enum {
	GdbEof = 0,
	GdbOpenBrace,
	GdbCloseBrace,
	GdbComma,
	GdbEqualSign,
	GdbWord
};

class GdbInfoLexer
{
	wxString m_input;
public:
	GdbInfoLexer(const wxString &input);
	virtual ~GdbInfoLexer();

	int Lex(wxString &word);
};

#endif //GDBINFOLEXER_H

