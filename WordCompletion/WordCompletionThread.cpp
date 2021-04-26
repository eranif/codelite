#include "WordCompletionDictionary.h"
#include "WordCompletionSettings.h"
#include "WordCompletionThread.h"
#include "WordTokenizerAPI.h"
#include "macros.h"
#include "wordcompletion.h"
#include "wx/strconv.h"

WordCompletionThread::WordCompletionThread(WordCompletionDictionary* dict)
    : m_dict(dict)
{
}

WordCompletionThread::~WordCompletionThread()
{
}
void WordCompletionThread::ProcessRequest(ThreadRequest* request)
{
    WordCompletionThreadRequest* req = dynamic_cast<WordCompletionThreadRequest*>(request);
    CHECK_PTR_RET(req);

    wxStringSet_t suggestsions;
    ParseBuffer(req->buffer, suggestsions);

    // Parse and send back the reply
    WordCompletionThreadReply reply;
    reply.filename = req->filename;
    reply.filter = req->filter;
    reply.insertSingleMatch = req->insertSingleMatch;
    reply.suggest.swap(suggestsions);
    m_dict->CallAfter(&WordCompletionDictionary::OnSuggestThread, reply);
}

void WordCompletionThread::ParseBuffer(const wxString& buffer, wxStringSet_t& suggest)
{
#if 0
    wxArrayString filteredWords;
    wxArrayString words = ::wxStringTokenize(buffer, "\r\n \t->./\\'\"[]()<>*&^%#!@+=:,;{}|/", wxTOKEN_STRTOK);
    for(size_t i=0; i<words.size(); ++i) {
        if(!wxIsdigit(words.Item(i).at(0))) {
            filteredWords.Add(words.Item(i));
        }
    }
    suggest.insert(filteredWords.begin(), filteredWords.end());
#else
    WordScanner_t scanner = ::WordLexerNew(buffer);
    if(!scanner)
        return;
    WordLexerToken token;
    std::string curword;
    while(::WordLexerNext(scanner, token)) {
        switch(token.type) {
        case kWordDelim:
            if(!curword.empty()) {
                suggest.insert(wxString(curword.c_str(), wxConvUTF8, curword.length()));
            }
            curword.clear();
            break;

        case kWordNumber: {
            if(!curword.empty()) {
                curword += token.text;
            }
            break;
        }
        default:
            curword += token.text;
            break;
        }
    }
    ::WordLexerDestroy(&scanner);
#endif
}
