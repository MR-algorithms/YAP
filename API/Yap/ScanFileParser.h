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
	class VariableManager;

	class ScanFileParser
	{
	public:
		ScanFileParser();
		~ScanFileParser();

	protected:

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<VariableManager> _variables;
	};
}
#endif //SCAN_DEFINITION_PARSER_H