#ifndef SCAN_FILE_PARSER_H
#define SCAN_FILE_PARSER_H

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

	class ScanFileParser
	{
	public:
		ScanFileParser();
		~ScanFileParser() = default;

		void SetVariableSpace(std::shared_ptr<VariableSpace> variables);
		std::shared_ptr<VariableSpace> GetVariableSpace();

		bool Load(const wchar_t * scan_file_path);
	protected:
		bool Process();
		bool ProcessAssignment();
		bool ProcessSimpleAssignment(const wchar_t* variable_id, Tokens& tokens);
		bool ProcessStructAssignment(const wchar_t* variable_id, Tokens& tokens);
		bool ProcessArrayAssignment(const wchar_t* variable_id, Tokens& tokens);
		bool ProcessArrayResizeOperator(const wchar_t* variable_id, Tokens& tokens);
		bool ProcessArrayElementAssignment(const wchar_t* variable_id, Tokens& tokens);
		bool ProcessSimpleArrayElementAssignment(const wchar_t* variable_id, int type, Tokens& tokens);
		bool ProcessAllSimpleArrayElementAssighment(const wchar_t* variable_id, Tokens& tokens);
		bool OperatorAssignment(IVariable* varaible, Tokens& tokens);
		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<VariableSpace> _variables;
	};
}
#endif //SCAN_DEFINITION_PARSER_H