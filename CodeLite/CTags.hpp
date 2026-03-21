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
     * Parse C++ source files with ctags and convert the generated output into tag entries.
     *
     * This method builds a newline-separated file list, invokes the C++ tag generator, and
     * parses each returned line into a TagEntry object. It also adjusts enumerator scope
     * information when an enumerator belongs to the most recently seen enum in the same file.
     *
     * @param files A vector of wxString file paths to parse.
     * @param ctags_exe The path to the ctags executable to use for tag generation.
     * @return A std::vector<TagEntryPtr> containing the parsed tag entries, or an empty vector
     *         if tag generation failed or produced no usable output.
     */
    static std::vector<TagEntryPtr> ParseCxxFiles(const std::vector<wxString>& files, const wxString& ctags_exe);

    /**
     * @brief Parse a single C++ source file using ctags.
     *
     * This is a convenience wrapper around ParseCxxFiles() for parsing one file. It
     * forwards the provided file path as a single-element list and returns the tags
     * produced by the underlying ctags invocation.
     *
     * @param file The path to the C++ source file to parse.
     * @param ctags_exe The path to the ctags executable to use.
     *
     * @return std::vector<TagEntryPtr> A vector containing the parsed tag entries.
     */
    static std::vector<TagEntryPtr> ParseCxxFile(const wxString& file, const wxString& ctags_exe);

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
    static std::vector<TagEntryPtr>
    ParseCxxBuffer(const wxFileName& filename, const wxString& buffer, const wxString& ctags_exe);
    /**
     * Parse C++ local symbols from ctags output for the given source buffer.
     *
     * This method writes the supplied buffer to a temporary file, runs ctags with
     * the options needed to capture local symbols and functions, and converts each
     * output line into a TagEntry associated with the provided file name.
     *
     * @param filename const wxFileName& The source file name to associate with the
     *        generated tags.
     * @param buffer const wxString& The C++ source text to analyze.
     * @param ctags_exe const wxString& The path to the ctags executable to run.
     *
     * @return std::vector<TagEntryPtr> A vector of parsed tag entries. Returns an
     *         empty vector if ctags fails or produces no usable output.
     *
     * @throws None. Failure to generate ctags output is handled by returning an
     *         empty vector.
     */
    static std::vector<TagEntryPtr>
    ParseCxxLocals(const wxFileName& filename, const wxString& buffer, const wxString& ctags_exe);

    /**
     * @brief Parses symbols from a file using ctags and returns them in sorted order.
     *
     * This function reads the file content, optionally writes it to a temporary file when
     * the input path has no extension but the content-based extension can be detected, and
     * then invokes ctags symbol generation before parsing the output into symbol records.
     * The resulting symbols are sorted by line number and then by symbol name.
     *
     * @param file const wxString& Path to the file whose symbols should be parsed.
     * @param ctags_exe const wxString& Path to the ctags executable used to generate symbol data.
     *
     * @return std::vector<CTags::SymbolInfo> A vector of parsed symbols, sorted by line number
     *         and then by name. Returns an empty vector if the file cannot be read, temporary
     *         file creation or writing fails, or symbol generation/parsing does not produce data.
     */
    static std::vector<SymbolInfo> ParseFileSymbols(const wxString& file, const wxString& ctags_exe);

    /**
     * @brief Parses symbols from an in-memory buffer by writing it to a temporary file and invoking ctags.
     *
     * This function determines the appropriate file extension from the provided filename and buffer,
     * writes the buffer to a temporary file with that extension, and then parses symbols from that file
     * using the specified ctags executable. If the extension cannot be determined or the temporary file
     * cannot be written, an empty symbol list is returned.
     *
     * @param filename const wxString& The original file name used to help determine the buffer's file extension.
     * @param buffer const wxString& The source code contents to analyze.
     * @param ctags_exe const wxString& The path to the ctags executable used for symbol extraction.
     *
     * @return std::vector<CTags::SymbolInfo> A vector of parsed symbol information, or an empty vector on failure.
     */
    static std::vector<SymbolInfo>
    ParseBufferSymbols(const wxString& filename, const wxString& buffer, const wxString& ctags_exe);

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
     * Find the symbol range near a line and also return the matched symbol.
     *
     * The returned range uses the symbol's own ctags-provided end line when available;
     * otherwise, the end is inferred from the next symbol in the sorted list.
     */
    static std::optional<SymbolRangeInfo> FindSymbolsRangeNearLine(const std::vector<SymbolInfo>& symbols, int line);

    static clStatusOr<wxString> LocateExe();

private:
    static std::optional<wxString>
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
    DoCxxGenerate(const wxString& filesContent, const wxString& ctags_exe, const wxString& ctags_kinds = wxEmptyString);

    static std::optional<wxString> DoSymbolGenerate(const wxString& file, const wxString& ctags_exe);
    static bool IsSupportedSymbolLanguage(const wxString& language);
    static std::vector<SymbolInfo> ParseSymbolOutput(const wxString& content);
    static std::optional<SymbolKind>
    MapSymbolKind(const wxString& kind, const wxString& scope, const wxString& kind_from_ctags);

    static void Initialise(const wxString& ctags_exe);
};

#endif // CTAGSGENERATOR_HPP
