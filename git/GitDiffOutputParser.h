#ifndef GITDIFFOUTPUTPARSER_H
#define GITDIFFOUTPUTPARSER_H

#include "macros.h"
#include <wx/string.h>
class GitDiffOutputParser
{
    enum eGitDiffStates {
        kLookingForFileName = 0,
        kLookingForDiff = 1,
    };

public:
    GitDiffOutputParser();
    virtual ~GitDiffOutputParser();

    /**
     * @brief parse the output of 'git diff...' command and return a map
     * of file->diff
     * @param rawDiff the output of the command 'git diff ...'
     * @param M [output]
     */
    void GetDiffMap(const wxString& rawDiff, wxStringMap_t& M) const;
};

#endif // GITDIFFOUTPUTPARSER_H
