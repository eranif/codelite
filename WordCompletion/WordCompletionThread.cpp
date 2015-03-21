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
    void* scanner = ::wordsLexerNew(req->buffer);
    CHECK_PTR_RET(scanner);

    WordToken token;

    WordCompletionSettings settings;
    size_t flags = settings.Load().GetCompleteTypes();

    // Parse and send back the reply
    WordCompletionThreadReply reply;
    reply.filename = req->filename;
    reply.filter = req->filter;
    reply.insertSingleMatch = req->insertSingleMatch;
    
    while(::wordsLexerNext(scanner, token)) {
        switch(token.type) {
        case kWORD_T_NUMBER:
            if(flags & WordCompletionSettings::kCompleteNumbers) {
                reply.suggest.insert(token.word);
            }
            break;
        case kWORD_T_WORD:
            if(flags & WordCompletionSettings::kCompleteWords) {
                reply.suggest.insert(token.word);
            }
            break;
        case kWORD_T_STRING:
            if(flags & WordCompletionSettings::kCompleteStrings) {
                reply.suggest.insert(token.word);
            }
            break;
        default:
            break;
        }
    }
    ::wordsLexerDestroy(&scanner);
    m_dict->CallAfter(&WordCompletionDictionary::OnSuggestThread, reply);
}
