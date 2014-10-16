#include "PHPTerminal.h"
#include "lexer_configuration.h"
#include "ColoursAndFontsManager.h"
#include "windowattrmanager.h"

PHPTerminal::PHPTerminal(wxWindow* parent)
    : TerminalEmulatorFrame(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("php");
    if(lexer) {
        lexer->Apply(GetTerminalUI()->GetTerminalOutputWindow());
    }
    WindowAttrManager::Load(this, "PHPTerminal");
}

PHPTerminal::~PHPTerminal()
{
    WindowAttrManager::Save(this, "PHPTerminal");
}

