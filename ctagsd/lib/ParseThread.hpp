#ifndef PARSETHREAD_HPP
#define PARSETHREAD_HPP

#include "macros.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>
#include <wx/arrstr.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <wx/thread.h>

using namespace std;
enum class eParseThreadCallbackRC {
    RC_SUCCESS,
    RC_EXIT,
};

typedef function<eParseThreadCallbackRC()> ParseThreadTaskFunc;

class ParseThread
{
    thread* m_change_thread = nullptr;
    mutex m_mutex;
    condition_variable m_cv;
    atomic_bool m_shutdown;
    vector<ParseThreadTaskFunc> m_queue;
    wxString m_settings_folder;
    wxString m_indexer_path;

public:
    ParseThread();
    ~ParseThread();

    void start(const wxString& settings_folder, const wxString& indexer_path);
    void stop();
    void queue_parse_request(ParseThreadTaskFunc&& task);
};

#endif // PARSETHREAD_HPP
