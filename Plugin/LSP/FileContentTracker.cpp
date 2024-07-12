#include "FileContentTracker.hpp"

#include "Diff/clDTL.h"
#include "file_logger.h"

namespace
{
const wxString EMPTY_STRING;
}

FileContentTracker::FileContentTracker() {}

FileContentTracker::~FileContentTracker() {}

bool FileContentTracker::exists(const wxString& filepath)
{
    FileState* dummy = nullptr;
    return find(filepath, &dummy);
}

void FileContentTracker::erase(const wxString& filepath)
{
    for(size_t i = 0; i < m_files.size(); ++i) {
        if(m_files[i].file_path == filepath) {
            m_files.erase(m_files.begin() + i);
            break;
        }
    }
}

std::vector<LSP::TextDocumentContentChangeEvent> FileContentTracker::changes_from(const wxString& before,
                                                                                  const wxString& after)
{
    clDTL diff;
    auto steps = diff.CreatePatch(before, after);

    LOG_IF_DEBUG
    {
        if(!steps.empty()) {
            auto escap_crlf = [](const wxString& s) -> wxString {
                wxString ss = s;
                ss.Replace("\r", "\\r");
                ss.Replace("\n", "\\n");
                return ss;
            };

            for(auto c : steps) {
                LSP_DEBUG() << escap_crlf(c.to_string()) << endl;
            }
        }
    }

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
            end_pos.SetLine(step.line_number + 1);
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

bool FileContentTracker::find(const wxString& filepath, FileState** state)
{
    for(size_t i = 0; i < m_files.size(); ++i) {
        if(m_files[i].file_path == filepath) {
            *state = &m_files[i];
            return true;
        }
    }
    return false;
}

void FileContentTracker::update_content(const wxString& filepath, const wxString& content)
{
    FileState* state = nullptr;
    if(find(filepath, &state)) {
        state->content = content;
    } else {
        FileState state;
        state.content = content;
        state.file_path = filepath;
        m_files.push_back(state);
    }
}

bool FileContentTracker::get_last_content(const wxString& filepath, wxString* content)
{
    FileState* state = nullptr;
    if(find(filepath, &state)) {
        *content = state->content;
        return true;
    }
    return false;
}

void FileContentTracker::add_flag(const wxString& filepath, size_t flag)
{
    FileState* state = nullptr;
    if(find(filepath, &state)) {
        state->flags |= flag;
    }
}

void FileContentTracker::remove_flag(const wxString& filepath, size_t flag)
{
    FileState* state = nullptr;
    if(find(filepath, &state)) {
        state->flags &= ~flag;
    }
}
