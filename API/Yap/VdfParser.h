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
	class TypeManager;

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
		bool ProcessEnumDeclaration(Tokens& tokens);
		bool ProcessArrayDeclaration(Tokens& tokens, VariableTable& variables);
		bool ProcessStructDeclaration(Tokens& tokens);
		bool ProcessNamespace(Tokens& tokens);
		bool ProcessUsing(Tokens& tokens);

		std::wstring ResolveFqId(const wchar_t * const id, bool is_type_id, Tokens& tokens) const;
		std::wstring GetFqId(const wchar_t * const id) const;

		bool IdExists(const wchar_t * const id, bool is_type_id) const;
		std::wstring OuterNamespace(const std::wstring& inner_namespace) const;

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<VariableSpace> _variables;
		std::shared_ptr<TypeManager> _types;

		std::wstring _current_namespace;
		std::set <std::wstring> _namespace_in_use;
	};
}

#endif // VARIABLE_DEFINITION_PARSER_H
