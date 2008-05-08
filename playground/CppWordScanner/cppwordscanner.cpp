#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include "cppwordscanner.h"
#include "stringaccessor.h"
#include "tokendb.h"

enum {
	STATE_NORMAL = 0,
	STATE_C_COMMENT,
	STATE_CPP_COMMENT,
	STATE_DQ_STRING,
	STATE_SINGLE_STRING
};

CppWordScanner::CppWordScanner(const wxString &file_name)
		: m_db(NULL)
{
	wxString key_words =
	    wxT("auto break case char const continue default define defined do double elif else endif enum error extern float"
	        "for  goto if ifdef ifndef include int line long pragma register return short signed sizeof static struct switch"
	        "typedef undef union unsigned void volatile while class namespace delete friend inline new operator overload"
	        "protected private public this virtual template typename dynamic_cast static_cast const_cast reinterpret_cast"
	        "using throw catch");

	//add this items into map
	m_arr = wxStringTokenize(key_words, wxT(" "));
	m_arr.Sort();

	//open the file and read all its content
	wxFFile file(file_name, wxT("rb"));

	//first try the Utf8
	file.ReadAll(&m_text, wxConvUTF8);
	if (m_text.IsEmpty()) {
		//try local
		file.Seek(0);

		file.ReadAll(&m_text, wxConvLocal);
		if (m_text.IsEmpty()) {
			file.Seek(0);
			file.ReadAll(&m_text, wxConvLibc);
		}
	}
	file.Close();
}

CppWordScanner::~CppWordScanner()
{
	m_db = NULL;
}

void CppWordScanner::FindAll(CppTokenList &l)
{
	doFind(wxEmptyString, l);
}

void CppWordScanner::Match(const wxString& word, CppTokenList& l)
{
	doFind(word, l);
	if (m_db) {
		CppTokenList::iterator iter = l.begin();
		for (; iter != l.end(); iter++) {
			m_db->Store( (*iter) );
		}
	}
}

void CppWordScanner::doFind(const wxString& filter, CppTokenList& l)
{
	int state(STATE_NORMAL);

	StringAccessor accessor(m_text);
	CppToken token;

	for (size_t i=0; i<m_text.size(); i++) {
		char ch = accessor.safeAt(i);
		switch (state) {
		case STATE_NORMAL:

			if (accessor.match("//", i)) {

				// C++ comment, advance i
				state = STATE_CPP_COMMENT;
				i++;

			} else if (accessor.match("/*", i)) {

				// C comment
				state = STATE_C_COMMENT;
				i++;

			} else if (accessor.match("'", i)) {

				// single quoted string
				state = STATE_SINGLE_STRING;

			} else if (accessor.match("\"", i)) {

				// dbouble quoted string
				state = STATE_DQ_STRING;

			} else if ( accessor.isWordChar(ch) ) {

				// is valid C++ word?
				token.append( ch );
				if (token.getOffset() == wxString::npos) {
					token.setOffset( i );
				}
			} else {

				if (token.getName().empty() == false) {

					//dont add C++ key words
					if (m_arr.Index(token.getName()) == wxNOT_FOUND) {

						// filter out non matching words
						if (filter.empty() || filter == token.getName()) {
							l.push_back( token );
						}
					}
				}

				token.reset();
			}

			break;
		case STATE_C_COMMENT:
			if ( accessor.match("*/", i)) {
				state = STATE_NORMAL;
				i++;
			}
			break;
		case STATE_CPP_COMMENT:
			if ( accessor.match("\n", i)) {
				state = STATE_NORMAL;
			}
			break;
		case STATE_DQ_STRING:
			if (accessor.match("\\\"", i)) {
				//escaped string
				i++;
			} else if (accessor.match("\"", i)) {
				state = STATE_NORMAL;
			}
			break;
		case STATE_SINGLE_STRING:
			if (accessor.match("\\'", i)) {
				//escaped single string
				i++;
			} else if (accessor.match("'", i)) {
				state = STATE_NORMAL;
			}
			break;
		}
	}
}

void CppWordScanner::SetDatabase(TokenDb* db)
{
	m_db = db;
}

