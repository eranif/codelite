#ifndef FILECONTENTTRACKER_HPP
#define FILECONTENTTRACKER_HPP

#include "LSP/basic_types.h"
#include "codelite_exports.h"
#include "macros.h"

#include <map>
#include <vector>
#include <wx/string.h>

enum FileStateFlags {
    FILE_STATE_NONE = 0,
};

struct WXDLLIMPEXP_SDK FileState {
    size_t flags = FILE_STATE_NONE;
    wxString content;
    wxString file_path;
};

class WXDLLIMPEXP_SDK FileContentTracker
{
    std::vector<FileState> m_files;

private:
    bool find(const wxString& filepath, FileState** state);

public:
    FileContentTracker();
    virtual ~FileContentTracker();

    /**
     * @brief do we track `filepath`?
     */
    bool exists(const wxString& filepath);
    /**
     * @brief remove the file from the tracker
     * @param filepath
     */
    void erase(const wxString& filepath);

    /**
     * @brief return the changes needed in otder to change `before` into `after`
     */
    std::vector<LSP::TextDocumentContentChangeEvent> changes_from(const wxString& before, const wxString& after);

    /**
     * @brief update the content for `filepath`
     */
    void update_content(const wxString& filepath, const wxString& content);

    /**
     * @brief return the last seen content for filepath
     */
    bool get_last_content(const wxString& filepath, wxString* content);
    void clear() { m_files.clear(); }

    void add_flag(const wxString& filepath, size_t flag);
    void remove_flag(const wxString& filepath, size_t flag);
};

#endif // FILECONTENTTRACKER_HPP
