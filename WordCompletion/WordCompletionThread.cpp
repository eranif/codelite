#include "WordCompletionThread.h"
#include "wordcompletion.h"
#include "WordsTokens.h"
#include "macros.h"
#include "WordCompletionSettings.h"

WordCompletionThread::WordCompletionThread(WordCompletionPlugin* plugin)
    : m_plugin(plugin)
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

    // Remove non matched words
    wxStringSet_t::iterator iter = reply.suggest.begin();
    wxStringSet_t filterdSet;
    for(; iter != reply.suggest.end(); ++iter) {
        wxString lcFilter = req->filter.Lower();
        wxString lcKey = iter->Lower();
        if(settings.GetComparisonMethod() == WordCompletionSettings::kComparisonStartsWith) {
            if(lcKey.StartsWith(lcFilter) && req->filter != (*iter)) {
                filterdSet.insert(*iter);
            }
        } else {
            if(lcKey.Contains(lcFilter) && req->filter != (*iter)) {
                filterdSet.insert(*iter);
            }
        }
    }
    
    reply.suggest.swap(filterdSet);
    m_plugin->CallAfter(&WordCompletionPlugin::OnSuggestThread, reply);
}
