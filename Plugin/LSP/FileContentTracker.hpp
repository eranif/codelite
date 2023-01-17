#ifndef FILECONTENTTRACKER_HPP
#define FILECONTENTTRACKER_HPP

#include "LSP/basic_types.h"
#include "codelite_exports.h"
#include "macros.h"

#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_SDK FileContentTracker
{
    wxStringMap_t m_files;

public:
    FileContentTracker();
    virtual ~FileContentTracker();

    /**
     * @brief do we track `filepath`?
     */
    bool exists(const wxString& filepath) const;
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
    const wxString& get_last_content(const wxString& filepath) const;

    void clear() { m_files.clear(); }
};

#endif // FILECONTENTTRACKER_HPP
