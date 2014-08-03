/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#ifndef _MEMCHECKSETTINGS_H_
#define _MEMCHECKSETTINGS_H_

#include "cl_config.h"

//////////////////////////////////////////////////////////////
// default settings values
//////////////////////////////////////////////////////////////

#define CONFIG_ITEM_NAME_VALGRIND "Valgrind"
#define VALGRIND_BINARY "/usr/bin/valgrind"
#define VALGRIND_OUTPUT_IN_PRIVATE_FOLDER true
#define VALGRIND_OUTPUT_FILE ""
#define VALGRIND_MANDATORY_OPTIONS "--tool=memcheck --xml=yes --fullpath-after= --gen-suppressions=all"
#define VALGRIND_OUTPUT_FILE_OPTION "--xml-file"
#define VALGRIND_SUPPRESSION_FILE_OPTION "--suppressions"
#define VALGRIND_OPTIONS "--leak-check=yes --track-origins=yes"
#define VALGRIND_SUPP_FILE_IN_PRIVATE_FOLDER true

#define CONFIG_ITEM_NAME_MEMCHECK "MemCheck"
#define RESULT_PAGE_SIZE 50
#define RESULT_PAGE_SIZE_MAX 200
#define OMIT_NONWORKSPACE false
#define OMIT_DUPLICATIONS false
#define OMIT_SUPPRESSED true

#define MEMCHECK_CONFIG_FILE "memcheck.conf"

/////////////////////////////////////////////////////////////////


class ValgrindSettings: public clConfigItem
{
public:
    ValgrindSettings();
    virtual ~ ValgrindSettings() {
    }

    virtual void FromJSON(const JSONElement & json);
    virtual JSONElement ToJSON() const;

private:
    wxString m_binary;
    bool m_outputInPrivateFolder;
    wxString m_outputFile;
    wxString m_mandatoryOptions;
    wxString m_outputFileOption;
    wxString m_suppressionFileOption;
    wxString m_options;
    bool m_suppFileInPrivateFolder;
    wxArrayString m_suppFiles;

public:
    void SetBinary(const wxString & binary) {
        m_binary = binary;
    }
    const wxString & GetBinary() const {
        return m_binary;
    }
    void SetOutputInPrivateFolder(bool outputInPrivateFolder) {
        m_outputInPrivateFolder = outputInPrivateFolder;
    }
    bool GetOutputInPrivateFolder() const {
        return m_outputInPrivateFolder;
    }
    void SetOutputFile(const wxString & outputFile) {
        m_outputFile = outputFile;
    }
    const wxString & GetOutputFile() const {
        return m_outputFile;
    }
//  void SetMandatoryOptions(const wxString & mandatoryOptions) {
//    m_mandatoryOptions = mandatoryOptions;
//  }
    const wxString & GetMandatoryOptions() const  {
        //return wxString::Format(m_mandatoryOptions, m_outputFile);
        return m_mandatoryOptions;
    }
    void SetOutputFileOption(const wxString& outputFileOption) {
        m_outputFileOption = outputFileOption;
    }
    const wxString& GetOutputFileOption() const {
        return m_outputFileOption;
    }
    void SetSuppressionFileOption(const wxString& suppressionFileOption) {
        m_suppressionFileOption = suppressionFileOption;
    }
    const wxString& GetSuppressionFileOption() const {
        return m_suppressionFileOption;
    }
    void SetOptions(const wxString& options) {
        m_options = options;
    }
    const wxString & GetOptions() const {
        return m_options;
    }
    void SetSuppFileInPrivateFolder(bool suppFileInPrivateFolder) {
        m_suppFileInPrivateFolder = suppFileInPrivateFolder;
    }
    bool GetSuppFileInPrivateFolder() const {
        return m_suppFileInPrivateFolder;
    }
    void SetSuppFiles(const wxArrayString& suppFiles) {
        m_suppFiles = suppFiles;
    }
    const wxArrayString& GetSuppFiles() const {
        return m_suppFiles;
    }
};


class MemCheckSettings: public clConfigItem
{
public:
    MemCheckSettings();
    virtual ~ MemCheckSettings() {
    }

    virtual void FromJSON(const JSONElement & json);
    virtual JSONElement ToJSON() const;

private:
    wxString m_engine;
    wxArrayString m_availableEngines;
    size_t m_result_page_size;
    size_t m_result_page_size_max;
    bool m_omitNonWorkspace;
    bool m_omitDuplications;
    bool m_omitSuppressed;

    /**
     * @brief place for particular subsetting nodes
     *
     * May be remake to list of several processor settings.
     */
    ValgrindSettings m_valgrindSettings;

public:
    void SetEngine(const wxString & engine) {
        m_engine = engine;
    }
    const wxString & GetEngine() const {
        return m_engine;
    }
//  void SetAvailableEngines(const wxArrayString & availableEngines) {
//    m_availableEngines = availableEngines;
//  }
    const wxArrayString & GetAvailableEngines() const {
        return m_availableEngines;
    }
    void SetResultPageSize(size_t result_page_size) {
        m_result_page_size = result_page_size;
    }
    size_t GetResultPageSize() const {
        return m_result_page_size;
    }
//    void SetResultPageSizeMax(size_t result_page_size_max) {
//        m_result_page_size_max = result_page_size_max;
//    }
    size_t GetResultPageSizeMax() const {
        return m_result_page_size_max;
    }
    void SetOmitNonWorkspace(bool omitNonWorkspace) {
        m_omitNonWorkspace = omitNonWorkspace;
    }
    bool GetOmitNonWorkspace() const {
        return m_omitNonWorkspace;
    }
    void SetOmitDuplications(bool omitDuplications) {
        m_omitDuplications = omitDuplications;
    }
    bool GetOmitDuplications() const {
        return m_omitDuplications;
    }
    void SetOmitSuppressed(bool omitSuppressed) {
        m_omitSuppressed = omitSuppressed;
    }
    bool GetOmitSuppressed() const {
        return m_omitSuppressed;
    }

    /**
     * @brief place for particular handlers of subsetting nodes
     */
    ValgrindSettings & GetValgrindSettings() {
        return m_valgrindSettings;
    }

    void SavaToConfig() {
        clConfig conf(MEMCHECK_CONFIG_FILE);
        conf.WriteItem(this);
    }

    void LoadFromConfig() {
        clConfig conf(MEMCHECK_CONFIG_FILE);
        conf.ReadItem(this);
    }
};

#endif
