#include "ThemeImporterPerl.hpp"

ThemeImporterPerl::ThemeImporterPerl()
{
    SetKeywords0(
        "NULL __FILE__ __LINE__ __PACKAGE__ __DATA__ __END__ AUTOLOAD BEGIN CORE DESTROY END INIT CHECK UNITCHECK "
        "abs accept alarm and atan2 bind binmode bless break caller chdir chmod chomp chop chown chr chroot "
        "close closedir cmp connect continue cos crypt dbmclose dbmopen default defined delete die do dump "
        "each else elsif endgrent endhostent endnetent endprotoent endpwent endservent eof eq eval exec "
        "exists exit exp fcntl fileno flock for foreach fork format formline ge getc getgrent getgrgid "
        "getgrnam gethostbyaddr gethostbyname gethostent getlogin getnetbyaddr getnetbyname getnetent "
        "getpeername getpgrp getppid getpriority getprotobyname getprotobynumber getprotoent getpwent "
        "getpwnam getpwuid getservbyname getservbyport getservent getsockname getsockopt given glob "
        "gmtime goto grep gt hex if index int ioctl join keys kill last lc lcfirst le length link listen "
        "local localtime lock log lstat lt map mkdir msgctl msgget msgrcv msgsnd my ne next no not oct "
        "open opendir or ord our pack package pipe pop pos print printf prototype push quotemeta rand "
        "read readdir readline readlink readpipe recv redo ref rename require reset return reverse "
        "rewinddir rindex rmdir say scalar seek seekdir select semctl semget semop send setgrent "
        "sethostent setnetent setpgrp setpriority setprotoent setpwent setservent setsockopt shift shmctl "
        "shmget shmread shmwrite shutdown sin sleep socket socketpair sort splice split sprintf sqrt srand "
        "stat state study sub substr symlink syscall sysopen sysread sysseek system syswrite tell telldir "
        "tie tied time times truncate uc ucfirst umask undef unless unlink unpack unshift untie until use "
        "utime values vec wait waitpid wantarray warn when while write x xor");
    // Besides *.pl and *.pm, files named "Config" are Perl files as well
    SetFileExtensions("*.pl;*.pm;Config");
    SetLangName("perl");
}

LexerConf::Ptr_t ThemeImporterPerl::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_PERL);
    AddProperty(lexer, wxSTC_PL_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_PL_ERROR, "Error", m_editor);
    AddProperty(lexer, wxSTC_PL_COMMENTLINE, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_PL_POD, "POD", m_multiLineComment);
    AddProperty(lexer, wxSTC_PL_POD_VERB, "POD verbatim", m_multiLineComment);
    AddProperty(lexer, wxSTC_PL_DATASECTION, "Data section", m_multiLineComment);
    AddProperty(lexer, wxSTC_PL_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_PL_WORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_PL_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_PL_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_PL_PUNCTUATION, "Punctuation", m_oper);
    AddProperty(lexer, wxSTC_PL_PREPROCESSOR, "Preprocessor", m_keyword);
    AddProperty(lexer, wxSTC_PL_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_PL_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_PL_SCALAR, "Scalar", m_variable);
    AddProperty(lexer, wxSTC_PL_ARRAY, "Array", m_variable);
    AddProperty(lexer, wxSTC_PL_HASH, "Hash", m_variable);
    AddProperty(lexer, wxSTC_PL_SYMBOLTABLE, "Symbol table", m_variable);
    AddProperty(lexer, wxSTC_PL_VARIABLE_INDEXER, "Variable indexer", m_oper);
    AddProperty(lexer, wxSTC_PL_REGEX, "Regex", m_field);
    AddProperty(lexer, wxSTC_PL_REGSUBST, "Regex substitution", m_field);
    AddProperty(lexer, wxSTC_PL_LONGQUOTE, "Long quote", m_string);
    AddProperty(lexer, wxSTC_PL_BACKTICKS, "Backticks", m_string);
    AddProperty(lexer, wxSTC_PL_HERE_DELIM, "HERE delim", m_string);
    AddProperty(lexer, wxSTC_PL_HERE_Q, "HERE Q", m_string);
    AddProperty(lexer, wxSTC_PL_HERE_QQ, "HERE QQ", m_string);
    AddProperty(lexer, wxSTC_PL_HERE_QX, "HERE QX", m_string);
    AddProperty(lexer, wxSTC_PL_STRING_Q, "String Q", m_string);
    AddProperty(lexer, wxSTC_PL_STRING_QQ, "String QQ", m_string);
    AddProperty(lexer, wxSTC_PL_STRING_QX, "String QX", m_string);
    AddProperty(lexer, wxSTC_PL_STRING_QR, "String QR", m_field);
    AddProperty(lexer, wxSTC_PL_STRING_QW, "String QW", m_string);
    AddProperty(lexer, wxSTC_PL_SUB_PROTOTYPE, "Sub prototype", m_klass);
    AddProperty(lexer, wxSTC_PL_FORMAT_IDENT, "Format identifier", m_function);
    AddProperty(lexer, wxSTC_PL_FORMAT, "Format", m_string);
    AddProperty(lexer, wxSTC_PL_STRING_VAR, "String variable", m_variable);
    AddProperty(lexer, wxSTC_PL_XLAT, "Translation", m_field);
    AddProperty(lexer, wxSTC_PL_REGEX_VAR, "Regex variable", m_variable);
    AddProperty(lexer, wxSTC_PL_REGSUBST_VAR, "Regex substitution variable", m_variable);
    AddProperty(lexer, wxSTC_PL_BACKTICKS_VAR, "Backticks variable", m_variable);
    AddProperty(lexer, wxSTC_PL_HERE_QQ_VAR, "HERE QQ variable", m_variable);
    AddProperty(lexer, wxSTC_PL_HERE_QX_VAR, "HERE QX variable", m_variable);
    AddProperty(lexer, wxSTC_PL_STRING_QQ_VAR, "String QQ variable", m_variable);
    AddProperty(lexer, wxSTC_PL_STRING_QX_VAR, "String QX variable", m_variable);
    AddProperty(lexer, wxSTC_PL_STRING_QR_VAR, "String QR variable", m_variable);
    FinalizeImport(lexer);
    return lexer;
}
