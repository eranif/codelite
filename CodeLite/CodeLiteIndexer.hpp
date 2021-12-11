#ifndef CODELITEINDEXER_HPP
#define CODELITEINDEXER_HPP

#include "codelite_exports.h"
#include <string>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

using namespace std;
class IProcess;
class WXDLLIMPEXP_CL CodeLiteIndexer
{
    wxFileName m_exepath;
    IProcess* m_process = nullptr;

public:
    typedef wxSharedPtr<CodeLiteIndexer> ptr_t;

private:
    string get_channel_path() const;

public:
    CodeLiteIndexer();
    ~CodeLiteIndexer();

    void set_exe_path(const wxFileName& exepath);
    void start();
    void stop();
    void restart();
    bool is_running() const;
    void source_to_tags(const wxFileName& source, wxString& tags) const;
    void buffer_to_tags(const wxString& content, wxString& tags) const;
};

#endif // CODELITEINDEXER_HPP
