#ifndef CTAGSGENERATOR_HPP
#define CTAGSGENERATOR_HPP

#include "codelite_exports.h"
#include "entry.h"
#include "readtags.h"
#include "tag_tree.h"

#include <vector>
#include <wx/filename.h>
#include <wx/textfile.h>

class WXDLLIMPEXP_CL CTags
{
public:
    enum eSearchFlags {
        kSearchFullmach = TAG_FULLMATCH,
        kSearchPartialMatch = TAG_PARTIALMATCH,
        kSearchIcase = TAG_IGNORECASE,
    };

protected:
    tagFile* m_file = nullptr;
    tagFileInfo m_fileInfo;
    wxFileName m_ctagsfile;
    size_t m_curline = 0;
    wxTextFile m_textFile;

protected:
    static wxString WrapSpaces(const wxString& file);
    /**
     * @brief parse list of files
     * @param filesContent contains a list of files to be parsed. Each file is placed on its own line
     * @param path location for the output ctags file. The output is written into `wxFileName(path, "ctags")`;
     * @param codelite_indexer path to `codelite_indexer`
     * @param ctags_args arguments to pass to ctags executable. Leave empty for the defaults
     * @param output if provided, holds the ctags content
     * @return true on success, false otherwise
     */
    static bool DoGenerate(const wxString& filesContent, const wxString& path, const wxString& codelite_indexer,
                           const wxString& ctags_args = wxEmptyString, wxString* output = nullptr);

    TagTreePtr TreeFromTags(std::vector<TagEntry>& tags);

public:
    CTags(const wxString& path);
    virtual ~CTags();

    bool IsOpened() const { return m_file != nullptr; }

    /**
     * @brief read from the tags file and return list of all tags from the same file
     * as a tree
     */
    TagTreePtr GetTagsTreeForFile(wxString& fullpath, std::vector<TagEntry>& tags,
                                  const wxString& force_filepath = wxEmptyString);
    size_t FindTags(const wxString& filter, std::vector<TagEntryPtr>& tags, size_t flags = kSearchFullmach);
    size_t FindTags(const wxArrayString& filter, std::vector<TagEntryPtr>& tags, size_t flags = kSearchFullmach);

    /**
     * @brief given a list of files, generate an output tags file and place it under 'path'
     */
    static bool Generate(const std::vector<wxFileName>& files, const wxString& path,
                         const wxString& codelite_indexer = wxEmptyString);
    static bool Generate(const wxArrayString& files, const wxString& path,
                         const wxString& codelite_indexer = wxEmptyString);

    /**
     * @brief run codelite-indexer on a given filename and return the output
     */
    static std::vector<TagEntry> Run(const wxFileName& filename, const wxString& temp_dir,
                                     const wxString& ctags_args = wxEmptyString,
                                     const wxString& codelite_indexer = wxEmptyString);

    /**
     * @brief run codelite-indexer on a buffer and return list of tags
     */
    static std::vector<TagEntry> RunOnBuffer(const wxString& buffer, const wxString& temp_dir,
                                             const wxString& ctags_args = wxEmptyString,
                                             const wxString& codelite_indexer = wxEmptyString);

    /**
     * @brief search for tags
     * @param filter tag name or part of it
     * @param ctagsFolder path to the ctags folder
     * @param [output] tags
     * @param flags search flags
     * @return number of tags found
     */
    static size_t FindTags(const wxString& filter, const wxString& ctagsFolder, std::vector<TagEntryPtr>& tags,
                           size_t flags = kSearchFullmach);
    static size_t FindTags(const wxArrayString& filter, const wxString& ctagsFolder, std::vector<TagEntryPtr>& tags,
                           size_t flags = kSearchFullmach);
};

#endif // CTAGSGENERATOR_HPP
