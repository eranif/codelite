#pragma once

#include "fileutils.h"

#include <optional>
#include <wx/sstream.h>
#include <wx/stream.h>
#include <wx/string.h>
#include <wx/xml/xml.h>

class clXmlDocument : public wxXmlDocument
{
public:
    clXmlDocument() = default;
    ~clXmlDocument() override = default;

    /**
     * Saves the XML document to a file only if the content has changed.
     *
     * This method compares the current state of the document against a cached
     * version to avoid unnecessary disk I/O. If changes are detected or no
     * cache exists, the content is written to the specified filename.
     *
     * @param filename The path to the file where the XML content should be saved.
     * @param indentstep The number of spaces to use for indentation (defaults to 2).
     * @return True if the file was successfully saved or if no changes were needed;
     *         false if the XML serialization or file writing failed.
     */
    bool SaveIfNeeded(const wxString& filename, int indentstep = 2) const
    {
        wxString content;
        wxStringOutputStream sos(&content);

        // Fix: Ensure indentstep is actually used if supported by the API
        if (!wxXmlDocument::Save(sos, indentstep)) {
            m_oldContent = std::nullopt;
            return false;
        }

        // Simplified comparison
        if (!m_oldContent || *m_oldContent != content) {
            m_oldContent = content;
            return FileUtils::WriteFileContent(filename, content);
        }

        return true;
    }

private:
    mutable std::optional<wxString> m_oldContent;
};
