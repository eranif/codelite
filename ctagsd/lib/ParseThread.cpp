#include "ParseThread.hpp"

#include "ProtocolHandler.hpp"
#include "file_logger.h"

ParseThread::ParseThread() {}

ParseThread::~ParseThread() { stop(); }

void ParseThread::start(const wxString& settings_folder, const wxString& indexer_path)
{
    stop();
    m_change_thread = new thread(
        [=](mutex& m, condition_variable& cv, vector<pair<wxString, wxString>>& Q) {
            while(true) {
                wxString filepath;
                wxString file_content;
                {
                    unique_lock<mutex> lk{ m };
                    cv.wait(lk, [&] { return !Q.empty(); });
                    filepath = Q.front().first;
                    file_content = Q.front().second;
                    Q.erase(Q.begin());
                }

                if(filepath.empty()) {
                    clDEBUG() << "requested to go down" << endl;
                    break;
                }
                // parse the file
                ProtocolHandler::parse_file(filepath, file_content, settings_folder, indexer_path);
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
    queue_parse_request(wxEmptyString, wxEmptyString);
    m_change_thread->join();
    wxDELETE(m_change_thread);
    clDEBUG() << "Success" << endl;
}

void ParseThread::queue_parse_request(const wxString& filepath, const wxString& file_content)
{
    unique_lock<mutex> lk{ m_mutex };
    for(auto& d : m_queue) {
        if(d.first == filepath) {
            clDEBUG() << "a request for file" << filepath << "already exists in the queue. updateing the request"
                      << endl;
            d.second = file_content;
            return;
        }
    }

    // add new entry
    m_queue.push_back({ filepath, file_content });
    m_cv.notify_one();
}
