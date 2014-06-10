#include "SvnBlameFrame.h"
#include <windowattrmanager.h>
#include <lexer_configuration.h>
#include <editor_config.h>

SvnBlameFrame::SvnBlameFrame(wxWindow* parent, const wxFileName& filename, const wxString& content)
    : SvnBlameFrameBase(parent)
    , m_filename(filename)
{
    // Now that the base class is done with its initialization
    // do our custom initialization to the blame editor
   
    m_stc->SetText( content );
    m_stc->SetReadOnly(true);
    LexerConfPtr lexer = EditorConfigST::Get()->GetLexerForFile(filename.GetFullName());
    if ( lexer ) {
        lexer->Apply( m_stc, true );
    }
    m_stc->Initialize();
    WindowAttrManager::Load(this, "SvnBlameFrame");
}

SvnBlameFrame::~SvnBlameFrame()
{
    WindowAttrManager::Save(this, "SvnBlameFrame");
}
