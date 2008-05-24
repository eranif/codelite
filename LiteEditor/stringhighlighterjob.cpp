#include "stringsearcher.h"
#include "stringhighlighterjob.h"
#include <vector>

StringHighlighterJob::StringHighlighterJob(wxEvtHandler *parent, const wxChar *str, const wxChar *word)
: Job(parent)
, m_str(str)
, m_word(word)
{
}

StringHighlighterJob::~StringHighlighterJob()
{
}

void StringHighlighterJob::Process(wxThread* thread)
{
	wxUnusedVar(thread);
	if( m_str.IsEmpty() || m_word.IsEmpty() ) {return;}
	
	int pos(0);
	int match_len(0);

	// remove reverse search
	int offset(0);
	
	// allocate result on the heap (will be freed by the caller)
	std::vector<std::pair<int, int> > *matches = new std::vector<std::pair<int, int> >;
	
	while ( StringFindReplacer::Search(m_str, offset, m_word, wxSD_MATCHCASE | wxSD_MATCHWHOLEWORD, pos, match_len) ) {
		// add result
		std::pair<int, int> match;
		match.first = pos;
		match.second = match_len;
		
		matches->push_back( match );
		offset = pos + match_len;
	}
	
	// report the result back to parent
	Post((void*) matches);
}
