#include "ScanFileParser.h"
#include "Implement/VariableSpace.h"
#include "Preprocessor.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

ScanFileParser::ScanFileParser() :
	_preprocessor {	make_shared<Preprocessor>(PreprocessScan)}
{
}


void ScanFileParser::SetVariableSpace(std::shared_ptr<VariableSpace> variables)
{
	_variables = variables;
}

std::shared_ptr<VariableSpace> ScanFileParser::GetVariableSpace()
{
	return _variables;
}

bool ScanFileParser::Load(const wchar_t * scan_file_path)
{
	wifstream scan_file;
	scan_file.open(scan_file_path);

	if (!scan_file)
	{
		wstring message = wstring(L"Failed to open scan file: ") + scan_file_path;
		return false;
	}

	try
	{
		if (!_preprocessor)
		{
			_preprocessor = make_shared<Preprocessor>(PreprocessScan);
		}
	}
	catch (bad_alloc& )
	{
		return false;
	}

	return _preprocessor->Preprocess(scan_file) && Process();
}

bool ScanFileParser::Process()
{
	assert(_preprocessor);
	assert(_variables);

	auto statement = _preprocessor->GetTokens();

	while (!statement.AtEnd())
	{
		Tokens::Guard guard(statement);
		auto type = statement.GetCurrentToken().type;
		auto variable_id = statement.GetCurrentToken().text;

		assert(type == TokenId);
		statement.Next();

		if (statement.GetCurrentToken().type == TokenOperatorDot)
		{// struct
			statement.Next();
			auto member_type = statement.GetCurrentToken().type;
			auto member_id = statement.GetCurrentToken().text;
			assert(member_type == TokenId);
			statement.Next();

			variable_id = variable_id + L"." + member_id;
		}
		else if (statement.GetCurrentToken().type == TokenDoubleColon)
		{// namespace
			statement.Next();
			variable_id = variable_id + wstring(L"::") + statement.GetCurrentToken().text;
			statement.Next();

			if (statement.GetCurrentToken().type == TokenOperatorDot)
			{
				statement.Next();
				assert(statement.GetCurrentToken().type == TokenId);
				auto member_id = statement.GetCurrentToken().text;
				statement.Next();

				variable_id = variable_id + L"." + member_id;
			}
		}

		statement.AssertToken(TokenOperatorAssign, true);
		wstring value_string;

		while (statement.GetCurrentToken().type != TokenSemiColon)
		{
			if (statement.GetCurrentToken().type == TokenStringLiteral)
			{
				value_string += L'\"';
				value_string += statement.GetCurrentToken().text;
				value_string += L'\"';
			}
			else
			{
				value_string += statement.GetCurrentToken().text;
			}
			statement.Next();
		}

		auto variable = _variables->GetVariable(variable_id.c_str());
		if (variable == nullptr)
			return false;

		if (!variable->FromString(value_string.c_str()))
			return false;
	}

	return true;
}
