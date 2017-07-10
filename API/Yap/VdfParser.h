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
	class NamespaceManager;

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
		bool ProcessStatement(Tokens& tokens);
		bool ProcessSimpleDeclaration(Tokens& tokens);
		bool ProcessArrayDeclaration(Tokens& tokens);
		bool ProcessStructDeclaration(Tokens& tokens);
		bool ProcessNamespace(Tokens& tokens);
		bool ProcessUsing(Tokens& tokens);

		std::wstring GetFqTypeId(const wchar_t * const type_id, Tokens& tokens);

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<VariableSpace> _variables;

		std::wstring _current_namespace;
		std::vector <std::wstring> _namespace_in_use;
	};
}

#endif // VARIABLE_DEFINITION_PARSER_H
