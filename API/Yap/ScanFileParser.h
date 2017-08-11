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
		~ScanFileParser();

		void SetVariableSpace(std::shared_ptr<VariableSpace> variables);
		std::shared_ptr<VariableSpace> GetVariableSpace();

		bool Load(const wchar_t * scan_file_path);
	protected:
		bool Process();

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<VariableSpace> _variables;
	};
}
#endif //SCAN_DEFINITION_PARSER_H