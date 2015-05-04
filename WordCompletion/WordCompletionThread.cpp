#include "WordCompletionThread.h"
#include "wordcompletion.h"
#include "WordsTokens.h"
#include "macros.h"
#include "WordCompletionSettings.h"
#include "WordCompletionDictionary.h"

WordCompletionThread::WordCompletionThread(WordCompletionDictionary* dict)
    : m_dict(dict)
{
}

WordCompletionThread::~WordCompletionThread() {}
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
    wxStringSet_t suggestsions;
    void* scanner = ::wordsLexerNew(buffer);
    CHECK_PTR_RET(scanner);

    WordToken token;

    WordCompletionSettings settings;
    size_t flags = settings.Load().GetCompleteTypes();

    while(::wordsLexerNext(scanner, token)) {
        switch(token.type) {
        case kWORD_T_NUMBER:
            if(flags & WordCompletionSettings::kCompleteNumbers) {
                suggest.insert(token.word);
            }
            break;
        case kWORD_T_WORD:
            if(flags & WordCompletionSettings::kCompleteWords) {
                suggest.insert(token.word);
            }
            break;
        case kWORD_T_STRING:
            if(flags & WordCompletionSettings::kCompleteStrings) {
                suggest.insert(token.word);
            }
            break;
        default:
            break;
        }
    }
    ::wordsLexerDestroy(&scanner);
}
