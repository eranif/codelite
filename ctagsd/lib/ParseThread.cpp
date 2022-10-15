#include "ParseThread.hpp"

#include "ProtocolHandler.hpp"
#include "file_logger.h"

ParseThread::ParseThread() {}

ParseThread::~ParseThread() { stop(); }

void ParseThread::start(const wxString& settings_folder, const wxString& indexer_path)
{
    stop();
    m_change_thread = new std::thread(
        [=](std::mutex& m, std::condition_variable& cv, std::vector<ParseThreadTaskFunc>& Q) {
            FileLogger::RegisterThread(wxThread::GetCurrentId(), "Parser");
            clDEBUG() << "ctagsd parser thread started..." << endl;
            while(true) {
                ParseThreadTaskFunc task_callback = nullptr;
                {
                    std::unique_lock<std::mutex> lk{ m };
                    cv.wait(lk, [&] { return !Q.empty(); });
                    task_callback = std::move(Q.front());
                    Q.erase(Q.begin());
                }

                // parse the file
                if(task_callback() == eParseThreadCallbackRC::RC_EXIT) {
                    break;
                }
            }
        },
        ref(m_mutex), ref(m_cv), ref(m_queue));
}

void ParseThread::stop()
{
    clDEBUG() << "Shutting down change thread" << endl;
    if(!m_change_thread) {
        return;
    }

    // place an empty request
    ParseThreadTaskFunc stop_callback = []() { return eParseThreadCallbackRC::RC_EXIT; };
    queue_parse_request(std::move(stop_callback));

    m_change_thread->join();
    wxDELETE(m_change_thread);
    clDEBUG() << "Success" << endl;
}

void ParseThread::queue_parse_request(ParseThreadTaskFunc&& task)
{
    std::unique_lock<std::mutex> lk{ m_mutex };
    // add new entry
    m_queue.emplace_back(std::move(task));
    m_cv.notify_one();
}
