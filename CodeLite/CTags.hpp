#ifndef CTAGSGENERATOR_HPP
#define CTAGSGENERATOR_HPP

#include "codelite_exports.h"
#include "entry.h"
#include "readtags.h"
#include "tag_tree.h"

#include <vector>
#include <wx/filename.h>
#include <wx/textfile.h>

using namespace std;
class WXDLLIMPEXP_CL CTags
{
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
    static bool DoGenerate(const wxString& filesContent, const wxString& codelite_indexer,
                           const wxStringMap_t& macro_table, const wxString& ctags_kinds = wxEmptyString,
                           wxString* output = nullptr);

    static void Initialise(const wxString& codelite_indexer);

public:
    /**
     * @brief given a list of files, generate an output tags file and place it under 'path'
     */
    static size_t ParseFiles(const vector<wxString>& files, const wxString& codelite_indexer,
                             const wxStringMap_t& macro_table, vector<TagEntryPtr>& tags);

    /**
     * @brief given a list of files, generate an output tags file and place it under 'path'
     */
    static size_t ParseFile(const wxString& file, const wxString& codelite_indexer, const wxStringMap_t& macro_table,
                            vector<TagEntryPtr>& tags);

    /**
     * @brief run codelite-indexer on a buffer and return list of tags
     */
    static size_t ParseBuffer(const wxFileName& filename, const wxString& buffer, const wxString& codelite_indexer,
                              const wxStringMap_t& macro_table, vector<TagEntryPtr>& tags);
    /**
     * @brief parse list of local variables from a given source file
     */
    static size_t ParseLocals(const wxFileName& filename, const wxString& buffer, const wxString& codelite_indexer,
                              const wxStringMap_t& macro_table, vector<TagEntryPtr>& tags);
};

#endif // CTAGSGENERATOR_HPP
