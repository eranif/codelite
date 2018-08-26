#include "XDebugEvalCmdHandler.h"
#include "XDebugManager.h"
#include "evalpane.h"
#include <editor_config.h>
#include <event_notifier.h>
#include <lexer_configuration.h>

EvalPane::EvalPane(wxWindow* parent)
    : EvalPaneBase(parent)
{
    Hide();
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_EVAL_EXPRESSION, &EvalPane::OnExpressionEvaluate, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_UNKNOWN_RESPONSE, &EvalPane::OnDBGPCommandEvaluated, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &EvalPane::OnSettingsChanged, this);
    LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("text");
    if(lex) { lex->Apply(m_stcOutput); }

    // Since XDebug replies with XML, use the XML lexer for the output
    LexerConf::Ptr_t xml_lex = EditorConfigST::Get()->GetLexer("xml");
    if(xml_lex) { xml_lex->Apply(m_stcOutputXDebug, true); }
    m_stcOutput->SetEditable(false);
}

EvalPane::~EvalPane()
{
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_EVAL_EXPRESSION, &EvalPane::OnExpressionEvaluate, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_UNKNOWN_RESPONSE, &EvalPane::OnDBGPCommandEvaluated, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &EvalPane::OnSettingsChanged, this);
}

void EvalPane::OnEnter(wxCommandEvent& event)
{
    if(!m_textCtrlExpression->IsEmpty()) { OnSend(event); }
}

void EvalPane::OnSend(wxCommandEvent& event)
{
    XDebugManager::Get().SendEvalCommand(m_textCtrlExpression->GetValue(), XDebugEvalCmdHandler::kEvalForEvalPane);
}

void EvalPane::OnSendUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlExpression->IsEmpty() && XDebugManager::Get().IsDebugSessionRunning());
}

void EvalPane::OnExpressionEvaluate(XDebugEvent& e)
{
    if(e.GetEvalReason() == XDebugEvalCmdHandler::kEvalForEvalPane) {

        m_stcOutput->SetEditable(true);
        m_stcOutput->ClearAll();

        wxString str;
        if(!e.IsEvalSucceeded()) {
            str << _("*** Error evaluating expression: ") << e.GetString() << "\n" << e.GetErrorString();
        } else {
            str << e.GetString() << " = \n";
            wxString evaluated = e.GetEvaluted();
            // Reomve extra escapes
            evaluated.Replace("\\n", "\n");
            evaluated.Replace("\\t", "\t");
            evaluated.Replace("\\r", "\r");
            evaluated.Replace("\\v", "\v");
            evaluated.Replace("\\b", "\b");
            str << evaluated;
        }

        m_stcOutput->AppendText(str);
        m_stcOutput->SetEditable(false);
        m_stcOutput->ScrollToEnd();

    } else {
        e.Skip();
    }
}

void EvalPane::OnSendXDebugCommand(wxCommandEvent& event)
{
    XDebugManager::Get().SendDBGPCommand(m_textCtrlExpressionXdebug->GetValue());
}

void EvalPane::OnSendXDebugCommandUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlExpressionXdebug->IsEmpty() && XDebugManager::Get().IsDebugSessionRunning());
}

void EvalPane::OnDBGPCommandEvaluated(XDebugEvent& e)
{
    e.Skip();
    m_stcOutputXDebug->SetEditable(true);
    m_stcOutputXDebug->ClearAll();
    m_stcOutputXDebug->SetText(e.GetEvaluted());
    m_stcOutputXDebug->SetEditable(true);
    m_stcOutputXDebug->ScrollToEnd();
}

void EvalPane::OnSettingsChanged(wxCommandEvent& event) { event.Skip(); }
