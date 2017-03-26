#ifndef VARIABLE_DEFINITION_PARSER_H
#define VARIABLE_DEFINITION_PARSER_H

#pragma once

#include <string>
#include <vector>
#include <set>
#include <stack>
#include <memory>

#include "Preprocessor.h"

namespace Yap
{
	/// Variable definition file.
	class VdfParser
	{
	public:
		VdfParser();
		~VdfParser();

        bool CompileFile(const wchar_t * path);
        bool Compile(const wchar_t * text);

	protected:
        bool DoCompile(std::wistream& input);
		bool Process();

		std::vector<std::wstring> _script_lines;
		std::vector<Token> _tokens;
		std::set<std::wstring> _key_words;
	};
}

#endif // VARIABLE_DEFINITION_PARSER_H
