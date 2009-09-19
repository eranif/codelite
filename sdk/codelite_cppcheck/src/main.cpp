/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2009 Daniel Marjamäki and Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 */


/**
 *
 * @mainpage Cppcheck
 * @version 1.35
 *
 * @section overview_sec Overview
 * Cppcheck is a simple tool for static analysis of C/C++ code.
 *
 * The method used is to first tokenize the source code and then analyse the token list.
 * In the token list, the tokens are stored in plain text.
 *
 * The checks are written in C++. The checks are addons that can be easily added/removed.
 *
 * @section writing_checks_sec Writing a check
 * Below is a simple example of a check that detect division with zero:
 * @code
void CheckOther::checkZeroDivision()
{
    // Iterate through all tokens in the token list
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (Token::Match(tok, "/ 0"))
            reportError(tok, Severity::error, "zerodiv", "Division by zero");
    }
}
 @endcode
 *
 * The function Token::Match is often used in the checks. Through it
 * you can match tokens against patterns.
 *
 *
 * @section checkclass_sec Creating a new check class from scratch
 * %Check classes inherit from the Check class. The Check class specifies the interface that you must use.
 * To integrate a check class into cppcheck all you need to do is:
 * - Add your source file(s) so they are compiled into the executable.
 * - Create an instance of the class (the Check::Check() constructor registers the class as an addon that Cppcheck then can use).
 *
 *
 * @section embedding_sec Embedding Cppcheck
 * Cppcheck is designed to be easily embeddable into other programs.
 *
 * The "src/main.cpp" and "src/cppcheckexecutor.*" files illustrate how cppcheck
 * can be embedded into an application.
 *
 *
 * @section detailed_overview_sec Detailed overview
 * This happens when you execute cppcheck from the command line:
 * -# CppCheckExecutor::check this function executes the Cppcheck
 * -# CppCheck::parseFromArgs parse command line arguments
 *   - The Settings class is used to maintain settings
 *   - Use FileLister and command line arguments to get files to check
 * -# ThreadExecutor create more instances of CppCheck if needed
 * -# CppCheck::check is called for each file. It checks a single file
 * -# Preprocess the file (through Preprocessor)
 *   - Comments are removed
 *   - Macros are expanded
 * -# Tokenize the file (see Tokenizer)
 * -# Run the runChecks of all check classes.
 * -# Simplify the tokenlist (Tokenizer::simplifyTokenList)
 * -# Run the runSimplifiedChecks of all check classes
 *
 * When errors are found, they are reported back to the CppCheckExecutor through the ErrorLogger interface
 */

#ifdef __WXMSW__
#ifdef __DEBUG
#define PIPE_NAME "\\\\.\\pipe\\cppchecker_%s_dbg"
#else
#define PIPE_NAME "\\\\.\\pipe\\cppchecker_%s"
#endif
#else
#define PIPE_NAME "/tmp/cppchecker.%s.sock"
#endif

#include "network/cppchecker_protocol.h"
#include "cppcheckexecutor.h"
#include "network/cppchecker_net_reply.h"
#include "cppcheckexecutornetwork.h"
#include <iostream>
#include "network/named_pipe.h"
#include "network/np_connections_server.h"

/**
 * @brief return true if the cppchecker was invoked in the form of daemon
 * @param argc main's argc
 * @param argv main's argv
 * @param str [output] --daemon=str
 * @return true if --daemon was found in the command line arguments
 */
bool isDaemonMode(int argc, char *argv[], std::string &str)
{
	for (size_t i=1; i<argc; i++) {
		if ( strncmp(argv[i], "--daemon=", 9) == 0 ) {
			char *v = strtok(argv[i], "=");
			if ( v ) {
				v = strtok(NULL, "=");
				if ( v ) {
					str = v;
					return true;
				}
			} else {
				break;
			}
		}
	}
	return false;
}

/**
 * Main function of cppcheck
 *
 * @param argc Passed to CppCheck::parseFromArgs()
 * @param argv Passed to CppCheck::parseFromArgs()
 * @return What CppCheckExecutor::check() returns.
 */
int main(int argc, char* argv[])
{
	// Check if daemon mode was requested
	std::string daemon_name;
	if (isDaemonMode(argc, argv, daemon_name)) {

		// create channel and listen on it
		char channel_name[1024];
		int  max_requests(1500);
		int  requests    (0);

		sprintf(channel_name, PIPE_NAME, daemon_name.c_str());
		clNamedPipeConnectionsServer npServer(channel_name);

		while (true) {
			clNamedPipe *conn = npServer.waitForNewConnection( -1 );
			if (!conn) {
				continue;
			}

			// got a connection
			CppCheckExecutorNetwork exec(conn, argc, argv);
			exec.check(exec._argc, exec._argv);

			CPPCheckerReply reply;
			reply.setCompletionCode(CPPCheckerReply::CheckingCompleted);
			CPPCheckerProtocol::SendReply(conn, reply);

			// free the resource
			conn->disconnect();
			delete conn;

			requests ++;

			if (requests == max_requests) {
				std::cout << "INFO: Max requests reached, going down" << std::endl;
				break;
			}
		}

	} else {
		CppCheckExecutor exec;
		return exec.check(argc, argv);
	}
}
