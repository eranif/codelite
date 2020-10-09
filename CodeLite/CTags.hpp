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
    static bool DoGenerate(const wxString& filesContent, const wxString& path);
    
    TagTreePtr TreeFromTags(std::vector<TagEntry>& tags);

public:
    CTags(const wxString& path);
    virtual ~CTags();

    bool IsOpened() const { return m_file != nullptr; }


    /**
     * @brief read from the tags file and return list of all tags from the same file
     * as a tree
     */
    TagTreePtr GetTagsTreeForFile(wxString& fullpath);

    size_t FindTags(const wxString& filter, std::vector<TagEntryPtr>& tags, size_t flags = kSearchFullmach);
    size_t FindTags(const wxArrayString& filter, std::vector<TagEntryPtr>& tags, size_t flags = kSearchFullmach);

    /**
     * @brief given a list of files, generate an output tags file and place it under 'path'
     */
    static bool Generate(const std::vector<wxFileName>& files, const wxString& path);
    static bool Generate(const wxArrayString& files, const wxString& path);

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
