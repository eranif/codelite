#include "cppwordscanner.h"
#include "stringaccessor.h"

enum {
	STATE_NORMAL = 0,
	STATE_C_COMMENT,
	STATE_CPP_COMMENT,
	STATE_DQ_STRING,
	STATE_SINGLE_STRING
};

CppWordScanner::CppWordScanner(const std::string &text)
: m_text(text)
{
}

CppWordScanner::~CppWordScanner()
{
}

void CppWordScanner::parse(CppTokenList &l)
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
				if (token.getOffset() == std::string::npos) {
					token.setOffset( i );
				}
			} else {

				// not valid C++ word
				if (token.getName().empty() == false) {
					l.push_back( token );
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
