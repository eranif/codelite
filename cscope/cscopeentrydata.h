#ifndef __cscopeentrydata__
#define __cscopeentrydata__

#include "wx/string.h"

enum {
	KindFileNode = 0,
	KindSingleEntry
};

class CscopeEntryData
{
	wxString m_file;
	int m_line;
	wxString m_pattern;
	wxString m_scope;
	int m_kind;

public:
	CscopeEntryData();
	~CscopeEntryData();


//Setters
	void SetFile(const wxString& file) {
		this->m_file = file;
	}
	void SetKind(const int& kind) {
		this->m_kind = kind;
	}
	void SetLine(const int& line) {
		this->m_line = line;
	}
	void SetPattern(const wxString& pattern) {
		this->m_pattern = pattern;
	}
	void SetScope(const wxString& scope) {
		this->m_scope = scope;
	}
//Getters
	const wxString& GetFile() const {
		return m_file;
	}
	const int& GetKind() const {
		return m_kind;
	}
	const int& GetLine() const {
		return m_line;
	}
	const wxString& GetPattern() const {
		return m_pattern;
	}
	const wxString& GetScope() const {
		return m_scope;
	}

};
#endif // __cscopeentrydata__
