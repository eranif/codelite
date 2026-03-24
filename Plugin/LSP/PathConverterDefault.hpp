#ifndef PATHCONVERTERDEFAULT_HPP
#define PATHCONVERTERDEFAULT_HPP

#include "LSP/IPathConverter.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK PathConverterDefault : public IPathConverter
{
public:
    /**
     * Convert a path string into an LSP::FilePath, normalizing URI/file URL forms and
     * marking remote files when appropriate.
     *
     * This method decodes any URI-encoded characters, strips a leading "file://" prefix,
     * and applies platform-specific adjustments. On Windows it also handles paths that
     * arrive in "/C:/..." form and attempts to map known SFTP editor paths back to their
     * local file paths. If the path does not exist locally on non-Windows systems, the
     * result is flagged as a remote file.
     *
     * @param path const wxString& The input path or file URI to convert.
     * @return LSP::FilePath The converted file path, with remote-file status set when needed.
     */
    LSP::FilePath ConvertFrom(const wxString& path) const override;
    /**
     * Converts a local editor file path into an LSP file path or file URL.
     *
     * This method checks whether the path corresponds to an open remote editor file.
     * If so, it returns that editor's remote path prefixed with "file://". Otherwise,
     * it falls back to the default wxWidgets filename-to-URL conversion.
     *
     * @param path const wxString& The local file path to convert.
     * @return LSP::FilePath The converted file path or file URL.
     */
    LSP::FilePath ConvertTo(const wxString& path) const override;
    PathConverterDefault() = default;
    ~PathConverterDefault() override = default;
};

#endif // PATHCONVERTERDEFAULT_HPP
