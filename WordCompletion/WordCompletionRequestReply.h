#ifndef WordCompletionRequestReply_H__
#define WordCompletionRequestReply_H__

#include "worker_thread.h"

struct WordCompletionThreadRequest : public ThreadRequest {
    wxString buffer;
    wxString filter;
    wxFileName filename;
    bool insertSingleMatch;
};

struct WordCompletionThreadReply {
    wxStringSet_t suggest;
    wxFileName filename;
    wxString filter;
    bool insertSingleMatch;
};

#endif
