#ifndef __cpptoken__
#define __cpptoken__

#include <wx/string.h>
#include <list>

class CppToken
{
	int m_id;
	wxString name;		// the name of the token
	size_t offset;		// file offset
	wxString filename;

public:
	CppToken();
	~CppToken();

	void reset();
	void append(const char ch);

	void setName(const wxString& name) {
		this->name = name;
	}

	void setOffset(const size_t& offset) {
		this->offset = offset;
	}

	const wxString& getName() const {
		return name;
	}
	const size_t& getOffset() const {
		return offset;
	}

	void setFilename(const wxString& filename) {
		this->filename = filename;
	}

	const wxString& getFilename() const {
		return filename;
	}

	void setId(const int& id) {
		this->m_id = id;
	}

	const int& getId() const {
		return m_id;
	}

	void print();
};
typedef std::list<CppToken> CppTokenList;
#endif // __cpptoken__
