#include "FileContentTracker.hpp"

#include "clDTL.h"
#include "file_logger.h"

namespace
{
const wxString EMPTY_STRING;
}

FileContentTracker::FileContentTracker() {}

FileContentTracker::~FileContentTracker() {}

bool FileContentTracker::exists(const wxString& filepath) const { return (m_files.count(filepath) != 0); }
void FileContentTracker::erase(const wxString& filepath) { m_files.erase(filepath); }

std::vector<LSP::TextDocumentContentChangeEvent> FileContentTracker::changes_from(const wxString& before,
                                                                                  const wxString& after)
{
    clDTL diff;
    auto steps = diff.CreatePatch(before, after);
    // convert the patch into ranges
    std::vector<LSP::TextDocumentContentChangeEvent> result;
    for(const auto& step : steps) {
        LSP::TextDocumentContentChangeEvent event;
        LSP::Range range;
        LSP::Position start_pos;
        LSP::Position end_pos;
        start_pos.SetCharacter(0);
        end_pos.SetCharacter(0);
        switch(step.action) {
        case PatchAction::ADD_LINE: {
            start_pos.SetLine(step.line_number);
            end_pos.SetLine(step.line_number);
            range.SetEnd(end_pos).SetStart(start_pos);
            event.SetRange(range);
            event.SetText(step.content);
            result.push_back(event);
        } break;
        case PatchAction::DELETE_LINE: {
            start_pos.SetLine(step.line_number);
            end_pos.SetLine(step.line_number);
            range.SetEnd(end_pos).SetStart(start_pos);
            event.SetRange(range);
            event.SetText(wxEmptyString); // this ensures a delete operation
            result.push_back(event);
        } break;
        default:
            break;
        }
    }
    return result;
}

void FileContentTracker::update_content(const wxString& filepath, const wxString& content)
{
    m_files.erase(filepath);
    m_files.insert({ filepath, content });
}

const wxString& FileContentTracker::get_last_content(const wxString& filepath) const
{
    if(!exists(filepath)) {
        return EMPTY_STRING;
    }
    return m_files.find(filepath)->second;
}
