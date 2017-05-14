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
	class VariableSpace;

	/// Variable definition file.
	class VdfParser
	{
	public:
		VdfParser();
		~VdfParser();

        std::shared_ptr<VariableSpace> CompileFile(const wchar_t * path);
        std::shared_ptr<VariableSpace> Compile(const wchar_t * text);

	protected:
        std::shared_ptr<VariableSpace> DoCompile(std::wistream& input);
		bool Process();
		bool ProcessSimpleDeclaration(Statement& statement);
		bool ProcessArrayDeclaration(Statement& statement);
		bool ProcessStructDeclaration(Statement& statement);

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<VariableSpace> _variables;
	};
}

#endif // VARIABLE_DEFINITION_PARSER_H
