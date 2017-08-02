#include "SvnShowFileChangesHandler.h"
#include "file_logger.h"
#include <wx/tokenzr.h>
#include "subversion2.h"

SvnShowFileChangesHandler::SvnShowFileChangesHandler(Subversion2* plugin, int cmdid, wxEvtHandler* owner)
    : SvnCommandHandler(plugin, cmdid, owner)
{
}

SvnShowFileChangesHandler::~SvnShowFileChangesHandler() {}

void SvnShowFileChangesHandler::Process(const wxString& output)
{
    clDEBUG() << "Show file changes returned:" << clEndl;
    clDEBUG() << output << clEndl;

    SvnShowDiffChunk::List_t changes;
    SvnShowDiffChunk curchange;
    wxArrayString lines = ::wxStringTokenize(output, "\n", wxTOKEN_RET_DELIMS);
    if(lines.IsEmpty()) return;
    
    // Remove everything until we find the first line that starts with "---------..."
    // Remove it and then break the loop
    while(!lines.IsEmpty()) {
        wxString firstLine = lines.Item(0);
        lines.RemoveAt(0);
        if(firstLine.StartsWith("------")) {
            break;
        }
    }

    eState state = kWaitingHeader;
    while(!lines.IsEmpty()) {
        wxString curline = lines.Item(0);
        switch(state) {
        case kWaitingHeader:
            curchange.description = curline.Trim().Trim(false);
            lines.RemoveAt(0);
            state = kWaitingDiffBlock;
            break;

        case kWaitingDiffBlock:
            if(curline.StartsWith("======")) {
                // the line above us was the file name of the diff, restore it
                if(!curchange.commentArr.IsEmpty()) {
                    wxString fileNameLine = curchange.commentArr.Last();
                    curchange.commentArr.RemoveAt(curchange.commentArr.size() - 1); // Remove the last line
                    lines.Insert(fileNameLine, 0);
                }
                state = kWaitingSeparator;
            } else {
                curchange.commentArr.Add(curline);
                lines.RemoveAt(0);
            }
            break;

        case kWaitingSeparator:
            if(curline.StartsWith("---------")) {
                // our chunk is complete, add it
                curchange.Finalize();
                changes.push_back(curchange);
                state = kWaitingHeader;
                curchange = SvnShowDiffChunk();
            } else {
                curchange.diff << curline;
            }
            lines.RemoveAt(0);
            break;
        }
    }
    m_plugin->CallAfter(&Subversion2::ShowRecentChangesDialog, changes);
}
