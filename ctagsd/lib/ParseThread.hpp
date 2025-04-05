#ifndef PARSETHREAD_HPP
#define PARSETHREAD_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>
#include <wx/string.h>

enum class eParseThreadCallbackRC {
    RC_SUCCESS,
    RC_EXIT,
};

typedef std::function<eParseThreadCallbackRC()> ParseThreadTaskFunc;

class ParseThread
{
    std::thread* m_change_thread = nullptr;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic_bool m_shutdown;
    std::vector<ParseThreadTaskFunc> m_queue;
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
