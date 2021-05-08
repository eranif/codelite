#include "GitDiffOutputParser.h"
#include <wx/tokenzr.h>

GitDiffOutputParser::GitDiffOutputParser() {}

GitDiffOutputParser::~GitDiffOutputParser() {}

void GitDiffOutputParser::GetDiffMap(const wxString& rawDiff, wxStringMap_t& M, wxArrayString* commitMessage) const
{
    wxStringTokenizer tokenizer(rawDiff, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);
    wxString currentFile;
    wxString currentDiff;
    const wxString diffPrefix = "diff --git a/";
    bool foundFirstDiff = false;
    eGitDiffStates state = kLookingForFileName; // Searching for file name
    wxString empty;
    wxString& line = empty;
    bool nextToken = true;
    while(tokenizer.HasMoreTokens() || nextToken == false) {
        if(nextToken) {
            line = tokenizer.GetNextToken();
        }
        nextToken = true;

        if(commitMessage && !foundFirstDiff) {
            // The git blame and git CommitList need to display the commit message
            // This can be found at the top of the output, before the first "diff "
            if(!line.StartsWith("diff ")) {
                commitMessage->Add(line + "\n");
                continue;
            } else {
                foundFirstDiff = true;
            }
        }

        switch(state) {
        case kLookingForFileName: {
            if(line.StartsWith(diffPrefix)) {
                int where = line.Find(diffPrefix);
                line = line.Mid(where + diffPrefix.length());
                where = line.Find(" b/");
                if(where != wxNOT_FOUND) {
                    line = line.Mid(0, where);
                }
                currentFile = line;
                state = kLookingForDiff;
            }
        } break;
        case kLookingForDiff: {
            if(line.StartsWith(diffPrefix)) {
                // Add the current outpt to the result output and continue
                M[currentFile] = currentDiff;
                currentDiff.Clear();
                currentFile.Clear();

                // Reset the scanner loop
                state = kLookingForFileName;
                nextToken =
                    false; // This will make sure we process this line again, however, in the other state switch case
            } else {
                currentDiff << line << "\n";
            }
        } break;
        }
    }

    // Add any leftovers
    if(!currentFile.IsEmpty()) {
        M[currentFile] = currentDiff;
    }
}
