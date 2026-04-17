#ifndef CTAGSGENERATOR_HPP
#define CTAGSGENERATOR_HPP

#include "clResult.hpp"
#include "codelite_exports.h"
#include "database/entry.h"
#include "fileextmanager.h"

#include <optional>
#include <vector>
#include <wx/filename.h>
#include <wx/textfile.h>

class WXDLLIMPEXP_CL CTags
{
public:
    enum class SymbolKind {
        kClass,
        kStruct,
        kTrait,
        kFunction,
        kPrototype,
        kGlobalMethod,
        kMethod,
    };

    struct SymbolInfo {
        wxString name;
        wxString file;
        SymbolKind kind = SymbolKind::kFunction;
        int line = 0;
        /**
         * Optional ending line reported by ctags JSON output.
         * If missing, callers should compute the end from the next symbol.
         */
        std::optional<int> end_line{std::nullopt};
        wxString signature;

        inline wxString to_string() const
        {
            wxString s;
            s << name << signature << ". Line:" << line << ", EndLine:" << end_line.value_or(-1);
            return s;
        }
    };
    /**
     * Parse C++ source files with universal-ctags and return the resulting tag entries.
     *
     * This method invokes the configured ctags executable to generate tags for the given files,
     * then converts each output line into a TagEntry. It also adjusts enumerator scope information
     * when an enumerator is detected to belong to the most recently seen enum in the same file.
     *
     * @param files const std::vector<wxString>& A list of C++ source file paths to parse.
     * @return clStatusOr<std::vector<TagEntryPtr>> A status-or containing the parsed tag entries on
     * success, or an error status if ctags is not installed. If ctags runs but produces no output,
     * an empty vector is returned.
     * @throws StatusNotFound If the universal-ctags executable is not available.
     */
    static clStatusOr<std::vector<TagEntryPtr>> ParseCxxFiles(const std::vector<wxString>& files);

    /**
     * @brief Parses a single C++ source file and returns the collected tag entries.
     *
     * This is a convenience wrapper around ParseCxxFiles() for processing one file in the
     * context of the CTags parser. It delegates the actual parsing work to the multi-file
     * overload and returns whatever status or results that function produces.
     *
     * @param file const wxString& The path to the C++ source file to parse.
     *
     * @return clStatusOr<std::vector<TagEntryPtr>> A status-or object containing the
     *         resulting vector of TagEntryPtr entries on success, or an error status on failure.
     *
     * @throws Any errors reported by ParseCxxFiles(), including parsing or file access failures.
     */
    static clStatusOr<std::vector<TagEntryPtr>> ParseCxxFile(const wxString& file);

    /**
     * Parses C++ tags from an in-memory buffer by writing it to a temporary file and reusing the file-based parser.
     *
     * This method creates a temporary "cpp" file, writes the provided buffer into it, invokes the CTags
     * file parser, and then updates each returned tag so that its source file is reported as the supplied
     * filename rather than the temporary file path.
     *
     * @param filename wxFileName The logical source file name to assign to all parsed tags.
     * @param buffer wxString The C++ source text to parse.
     * @param ctags_exe wxString The path to the ctags executable used for parsing.
     *
     * @return std::vector<TagEntryPtr> A vector of parsed tag entries extracted from the buffer.
     */
    static clStatusOr<std::vector<TagEntryPtr>> ParseCxxBuffer(const wxFileName& filename, const wxString& buffer);

    /**
     * Parses the symbols found in a source file using universal-ctags.
     *
     * If the file has no extension but its content suggests one, the function writes the
     * content to a temporary file with the inferred extension before invoking ctags.
     * The resulting symbols are parsed and returned in ascending order by line number,
     * then by symbol name.
     *
     * @param file const wxString& Path to the file whose symbols should be parsed.
     * @return clStatusOr<std::vector<CTags::SymbolInfo>> A vector of parsed symbol
     *     information on success, or a status error if the ctags executable is missing,
     *     the file cannot be read, or a temporary file cannot be written. If symbol
     *     generation fails, an empty vector is returned.
     * @throws No exceptions are thrown directly; errors are reported through the
     *     returned clStatusOr status value.
     */
    static clStatusOr<std::vector<SymbolInfo>> ParseFileSymbols(const wxString& file);

    /**
     * @brief Parses symbols from an in-memory buffer by writing it to a temporary file and analyzing it with ctags.
     *
     * This helper resolves the most likely file extension from the provided filename and buffer, writes the buffer to a
     * temporary file with that extension, and then reuses the file-based symbol parser. The function requires a
     * configured universal-ctags executable and may fail if the extension cannot be determined or the temporary file
     * cannot be written.
     *
     * @param filename const wxString& The original file name used to infer the file extension.
     * @param buffer const wxString& The in-memory contents to parse for symbols.
     *
     * @return clStatusOr<std::vector<CTags::SymbolInfo>> A status-or containing the parsed symbol list on success, or
     * an error status describing why parsing failed.
     *
     * @throws StatusNotFound If the universal-ctags executable is not configured or cannot be found.
     * @throws StatusInvalidArgument If the file extension cannot be resolved from the provided inputs.
     * @throws StatusIOError If the temporary file cannot be written to disk.
     */
    static clStatusOr<std::vector<SymbolInfo>> ParseBufferSymbols(const wxString& filename, const wxString& buffer);

    struct SymbolRangeInfo {
        SymbolInfo symbol;
        int start_line{0};
        /**
         * Optional ending line for the matched symbol range.
         * This is taken from ctags JSON "end" when available,
         * otherwise it is inferred from the next symbol in the list.
         */
        std::optional<int> end_line{std::nullopt};
        inline wxString to_string() const { return symbol.to_string(); }
    };

    /**
     * @brief Finds the first symbol whose line number is greater than the given value.
     *
     * This call is intended to be used with a sorted symbol range and a line number
     * threshold. It performs a binary-search-style lookup over the provided symbol
     * sequence and returns the position of the first element whose "line" member is
     * strictly greater than the supplied value.
     *
     * @param symbols.begin() iterator to the first symbol in the range being searched.
     * @param symbols.end() iterator one past the last symbol in the range being searched.
     * @param line int line number used as the search threshold.
     * @param value comparator capture representing the line number to compare against each symbol.
     *
     * @return Iterator to the first symbol with sym.line > line, or symbols.end() if no such symbol exists.
     *
     * @note The searched range must be ordered consistently with the comparison predicate
     *       for the result to be meaningful.
     */
    static std::optional<SymbolRangeInfo> FindSymbolsRangeNearLine(const std::vector<SymbolInfo>& symbols, int line);

    /**
     * @brief Initializes the ctags executable path and detects whether the installed ctags supports "macrodef".
     *
     * This one-time initialization locates the ctags executable, stores it for later use, and logs either
     * the discovered path or an error message. It then runs ctags with "--list-fields=c++" and scans the
     * output to determine whether the "macrodef" field is supported.
     *
     * @param None This function takes no parameters.
     *
     * @return void; this function does not return a value.
     *
     * @throws None This function does not throw exceptions directly. Errors from locating ctags or
     *         launching the process are handled internally by logging and by leaving the relevant state unchanged.
     */
    static void Initialise();

private:
    static clStatusOr<wxString> LocateExe();
    /**
     * Generate ctags output for a list of source files using a temporary options file.
     *
     * This method prepares a per-call ctags options file, writes the provided file list to a temporary
     * file, and then executes the configured ctags executable to produce tags on standard output.
     * It is a helper in the CTags class and uses the current thread id to create unique temporary
     * file names.
     *
     * @param filesContent const wxString& Newline-separated list of file paths to pass to ctags.
     * @param ctags_exe const wxString& Path to the ctags executable to run.
     * @param ctags_kinds const wxString& ctags kind selection string to apply to both C and C++,
     *        or an empty string to use the default kind set.
     *
     * @return std::optional<wxString> The command output produced by ctags, or an empty optional
     *         if execution fails or no output is available.
     *
     * @throws None explicitly. Errors are handled by the underlying file and process utility
     *         functions, which may fail internally when writing temporary files or executing ctags.
     */
    static std::optional<wxString>
    DoCxxGenerate(const wxString& filesContent, const wxString& ctags_exe, const wxString& ctags_kinds = wxEmptyString);
    /**
     * @brief Generates a JSON symbol listing for a source file using ctags.
     *
     * This method first verifies that the input file exists, then runs the configured
     * ctags executable with JSON output enabled and returns the captured standard output
     * as a wxString. If the file is missing or the external process fails, no value is
     * returned.
     *
     * @param file const wxString& Path to the source file to analyze.
     * @param ctags_exe const wxString& Path to the ctags executable to invoke.
     *
     * @return std::optional<wxString> The JSON output produced by ctags on success,
     *         or std::nullopt if the file does not exist or the command fails.
     *
     * @note This is a CTags class method and depends on an external ctags process.
     * @throws None. Failures are reported by returning std::nullopt.
     */
    static std::optional<wxString> DoSymbolGenerate(const wxString& file, const wxString& ctags_exe);
    /**
     * @brief Parses newline-delimited JSON symbol records into a list of symbol information entries.
     *
     * This function reads each non-empty line from the provided content, attempts to parse it as JSON,
     * and converts valid symbol objects into CTags::SymbolInfo instances. Invalid lines, malformed JSON,
     * and entries missing required fields are silently skipped.
     *
     * @param content wxString The input text containing one JSON object per line.
     * @param filename wxString The file path to assign to each parsed symbol entry.
     * @param file_ext wxString the filename extension.
     * @return std::vector<CTags::SymbolInfo> A vector of parsed symbol records, possibly empty if no valid entries are
     * found.
     *
     * @throws None. Any parsing or conversion errors are caught internally and the corresponding line is ignored.
     */
    static std::vector<SymbolInfo>
    ParseSymbolOutput(const wxString& content, const wxString& filename, const wxString& file_ext);
    static std::optional<SymbolKind> MapSymbolKind(const wxString& kind,
                                                   const wxString& scope,
                                                   const wxString& kind_from_ctags,
                                                   const wxString& file_ext);
};

#endif // CTAGSGENERATOR_HPP
