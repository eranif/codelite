#include "ThemeImporterInnoSetup.hpp"

ThemeImporterInnoSetup::ThemeImporterInnoSetup()
{
    SetKeywords0("code components custommessages dirs files icons ini installdelete langoptions languages messages "
                 "registry run setup types tasks uninstalldelete uninstallrun _istool");

    SetKeywords1(
        "  allowcancelduringinstall allownoicons allowrootdirectory allowuncpath   alwaysrestart "
        "alwaysshowcomponentslist alwaysshowdironreadypage   alwaysshowgrouponreadypage alwaysusepersonalgroup "
        "appcomments appcontact   appcopyright appenddefaultdirname appenddefaultgroupname appid appmodifypath   "
        "appmutex appname apppublisher apppublisherurl appreadmefile appsupporturl   appupdatesurl appvername "
        "appversion architecturesallowed   architecturesinstallin64bitmode backcolor backcolor2 backcolordirection   "
        "backsolid changesassociations changesenvironment compression copyrightfontname   copyrightfontsize "
        "createappdir createuninstallregkey defaultdirname   defaultgroupname defaultuserinfoname defaultuserinfoorg "
        "defaultuserinfoserial   dialogfontname dialogfontsize direxistswarning disabledirpage   disablefinishedpage "
        "disableprogramgrouppage disablereadymemo disablereadypage   disablestartupprompt diskclustersize "
        "diskslicesize diskspanning   enablesdirdoesntexistwarning encryption extradiskspacerequired   "
        "flatcomponentslist infoafterfile infobeforefile internalcompresslevel   languagedetectionmethod "
        "languagecodepage languageid languagename licensefile   mergeduplicatefiles minversion onlybelowversion "
        "outputbasefilename outputdir   outputmanifestfile password privilegesrequired reservebytes   "
        "restartifneededbyrun setupiconfile showcomponentsizes showlanguagedialog   showtaskstreelines slicesperdisk "
        "solidcompression sourcedir timestamprounding   timestampsinutc titlefontname titlefontsize touchdate "
        "touchtime uninstallable   uninstalldisplayicon uninstalldisplayname uninstallfilesdir uninstalllogmode   "
        "uninstallrestartcomputer updateuninstalllogappname usepreviousappdir   usepreviousgroup useprevioussetuptype "
        "useprevioustasks useprevioususerinfo   userinfopage usesetupldr versioninfocompany versioninfocopyright   "
        "versioninfodescription versioninfotextversion versioninfoversion   welcomefontname welcomefontsize "
        "windowshowcaption windowstartmaximized   windowresizable windowvisible wizardimagebackcolor wizardimagefile   "
        "wizardimagestretch wizardsmallimagefile   ");

    SetKeywords2("  afterinstall attribs beforeinstall check comment components copymode   description destdir "
                 "destname excludes extradiskspacerequired filename flags   fontinstall groupdescription hotkey "
                 "infoafterfile infobeforefile iconfilename   iconindex key languages licensefile messagesfile "
                 "minversion name   onlybelowversion parameters permissions root runonceid section source   statusmsg "
                 "string subkey tasks type types valuedata valuename valuetype   workingdir   ");

    SetKeywords3("  append define dim else emit endif endsub error expr file for if ifdef ifexist   ifndef ifnexist "
                 "include insert pragma sub undef   ");

    SetKeywords4("  begin break case const continue do downto else end except finally for function   if of procedure "
                 "repeat then to try until uses var while with   ");
    SetFileExtensions("*.iss");
    m_langName = "innosetup";
}

ThemeImporterInnoSetup::~ThemeImporterInnoSetup() {}

LexerConf::Ptr_t ThemeImporterInnoSetup::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_INNOSETUP);
    AddProperty(lexer, wxSTC_INNO_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_INNO_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_INNO_KEYWORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_INNO_PARAMETER, "Parameter", m_variable);
    AddProperty(lexer, wxSTC_INNO_SECTION, "Section", m_klass);
    AddProperty(lexer, wxSTC_INNO_PREPROC, "Pre processor", m_string);
    AddProperty(lexer, wxSTC_INNO_INLINE_EXPANSION, "Inline Expansion", m_string);
    AddProperty(lexer, wxSTC_INNO_COMMENT_PASCAL, "Pascal Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_INNO_KEYWORD_PASCAL, "Pascal Keyword", m_keyword);
    AddProperty(lexer, wxSTC_INNO_KEYWORD_USER, "User Keyword", m_keyword);
    AddProperty(lexer, wxSTC_INNO_STRING_DOUBLE, "Double string", m_string);
    AddProperty(lexer, wxSTC_INNO_STRING_SINGLE, "Single string", m_string);
    AddProperty(lexer, wxSTC_INNO_IDENTIFIER, "Identifier", m_editor);
    FinalizeImport(lexer);
    return lexer;
}
