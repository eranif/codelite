#include "EditorOptionsCopyPaste.h"

EditorOptionsCopyPaste::EditorOptionsCopyPaste(wxWindow* parent)
    : EditorOptionsCopyPasteBase(parent)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    size_t flags = options->GetOptions();
    
    int sel = 0; // Copy entire line
    if ( flags & OptionsConfig::Opt_CopyLineIfLineNotEmpty ) {
        sel = 1;
    } else if ( flags & OptionsConfig::Opt_CopyNothing ) {
        sel = 2;
    }
    m_radioBoxCopyOptions->SetSelection( sel );
}

EditorOptionsCopyPaste::~EditorOptionsCopyPaste()
{
}

void EditorOptionsCopyPaste::Save(OptionsConfigPtr options)
{
    size_t flags = options->GetOptions();
    flags &= ~(OptionsConfig::Opt_AlwaysCopyLine|OptionsConfig::Opt_CopyNothing|OptionsConfig::Opt_CopyLineIfLineNotEmpty);
    
    switch ( m_radioBoxCopyOptions->GetSelection() ) {
    case 1:
        flags |= OptionsConfig::Opt_CopyLineIfLineNotEmpty;
        break;
    case 2:
        flags |= OptionsConfig::Opt_CopyNothing;
        break;
    default:
    case 0:
        flags |= OptionsConfig::Opt_AlwaysCopyLine;
        break;
    }
    options->SetOptions( flags );
}
