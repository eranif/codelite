#include "SvnShowRecentChangesDlg.h"
#include "macros.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"

struct SvnShowDiffChunkUI : public wxClientData {
    SvnShowDiffChunk m_chunk;
    SvnShowDiffChunkUI(const SvnShowDiffChunk& chunk)
        : m_chunk(chunk)
    {
    }
    virtual ~SvnShowDiffChunkUI() {}
};

SvnShowRecentChangesDlg::SvnShowRecentChangesDlg(wxWindow* parent, const SvnShowDiffChunk::List_t& changes)
    : SvnShowRecentChangesBaseDlg(parent)
    , m_changes(changes)
{
    std::for_each(changes.begin(), changes.end(), [&](const SvnShowDiffChunk& chunk) {
        m_listBoxRevisions->Append(chunk.revision, new SvnShowDiffChunkUI(chunk));
    });
    m_listBoxRevisions->Select(0);

    LexerConf::Ptr_t diffLexer = ColoursAndFontsManager::Get().GetLexer("diff");
    if(diffLexer) {
        diffLexer->Apply(m_stcDiff);
    }

    LexerConf::Ptr_t textLexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(textLexer) {
        textLexer->Apply(m_stcComment);
    }
    DoSelectRevision(0);
}

SvnShowRecentChangesDlg::~SvnShowRecentChangesDlg() {}

void SvnShowRecentChangesDlg::OnRevisionSelected(wxCommandEvent& event)
{
    int where = event.GetSelection();
    if(where == wxNOT_FOUND) return;
    DoSelectRevision(where);
}

void SvnShowRecentChangesDlg::DoSelectRevision(int index)
{
    SvnShowDiffChunkUI* chunk = dynamic_cast<SvnShowDiffChunkUI*>(m_listBoxRevisions->GetClientObject(index));
    CHECK_PTR_RET(chunk);

    m_stcComment->SetReadOnly(false);
    m_stcComment->SetText(chunk->m_chunk.comment);
    m_stcComment->SetReadOnly(false);

    m_stcDiff->SetReadOnly(false);
    m_stcDiff->SetText(chunk->m_chunk.diff);
    m_stcDiff->SetReadOnly(true);
    
    m_staticTextDesc->SetLabel(chunk->m_chunk.description);
}
