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
	_preprocessor {	make_shared<Preprocessor>(PreprocessScan)},
	_variables{ make_shared<VariableSpace>() }
{}


// ScanFileParser::~ScanFileParser()
// {
// }

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

	// return _preprocessor->Preprocess(scan_file) && Process();
	return _preprocessor->Preprocess(scan_file) && ProcessAssignment();
}

bool ScanFileParser::Compile(const wchar_t * text)
{
	try
	{
		if (!_preprocessor)
		{
			_preprocessor = make_shared<Preprocessor>(PreprocessScan);
		}
	}
	catch (bad_alloc&)
	{
		return false;
	}

	// return _preprocessor->Preprocess(scan_file) && Process();
	return _preprocessor->Preprocess(text) && ProcessAssignment();
}

bool ScanFileParser::OperatorAssignment(IVariable* variable, Tokens& tokens)
{
	if (variable == nullptr)
	{
		return false;
	}
	tokens.AssertToken(TokenOperatorAssign, false);
	wstring value_string;
	tokens.Next();
	while (tokens.GetCurrentToken().type != TokenSemiColon)
	{
		if (tokens.GetCurrentToken().type == TokenStringLiteral)
		{
			value_string += L'\"';
			value_string += tokens.GetCurrentToken().text;
			value_string += L'\"';
		}
		else
		{
			value_string += tokens.GetCurrentToken().text;
		}
		tokens.Next();
	}

	if (!variable->FromString(value_string.c_str()))
		return false;

	return true;
}

bool ScanFileParser::ProcessSimpleAssignment(const wchar_t* variable_id, Tokens& tokens)
{
	tokens.AssertToken(TokenOperatorAssign, false);
	return OperatorAssignment(_variables->GetVariable(variable_id), tokens);
}

bool ScanFileParser::ProcessStructAssignment(const wchar_t* variable_id, Tokens& tokens)
{
	if (tokens.GetCurrentToken().type == TokenOperatorAssign)
	{
		return OperatorAssignment(_variables->GetVariable(variable_id), tokens);
	}
	else
	{
		tokens.AssertToken(TokenOperatorDot, true);
		assert(tokens.GetCurrentToken().type == TokenId);
		wstring variable_id_str = variable_id + wstring(L".") + tokens.GetCurrentToken().text;
		auto type = _variables->GetVariable(variable_id_str.c_str())->GetType();
		tokens.Next();
		switch (type)
		{
		case VariableBool:
		case VariableInt:
		case VariableFloat:
		case VariableString:
			ProcessSimpleAssignment(variable_id_str.c_str(), tokens);
			break;
		case VariableStruct:
			ProcessStructAssignment(variable_id_str.c_str(), tokens);
			break;
		case VariableIntArray:
		case VariableFloatArray:
		case VariableStringArray:
			ProcessArrayAssignment(variable_id_str.c_str(), tokens);
			break;
		default:
			throw(CompileError(tokens.GetCurrentToken(),
				CompileErrorTypeExpected, L"Expected one of float, int, string, bool, array, struct."));
			break;
		}
		return true;
	}
}

bool ScanFileParser::ProcessArrayResizeOperator(const wchar_t* variable_id, Tokens& tokens)
{
	tokens.Next();
	assert(tokens.GetCurrentToken().text == L"resize");  // 句式.resize(2)
	tokens.Next();
	tokens.AssertToken(TokenLeftParenthesis, true);
	assert(tokens.GetCurrentToken().type == TokenNumericLiteral);
	auto array_size = tokens.GetCurrentToken().text;
	_variables->ResizeArray(variable_id, _wtoi(array_size.c_str()));
	tokens.Next();
	tokens.AssertToken(TokenRightParenthesis, true);
	tokens.AssertToken(TokenSemiColon, false);
	return true;
}

bool ScanFileParser::ProcessSimpleArrayElementAssignment(const wchar_t* variable_id, int type, Tokens& tokens)
{
	tokens.AssertToken(TokenOperatorAssign, true);
	wstring value_string;
	while (tokens.GetCurrentToken().type != TokenSemiColon)
	{
		if (tokens.GetCurrentToken().type == TokenStringLiteral)
		{
			value_string += L'\"';
			value_string += tokens.GetCurrentToken().text;
			value_string += L'\"';
		}
		else
		{
			value_string += tokens.GetCurrentToken().text;
		}
		tokens.Next();
	}

	switch (type)
	{
	case VariableBoolArray:
		_variables->Set<bool>(variable_id, _wtoi(value_string.c_str()));
		break;
	case VariableIntArray:
		_variables->Set<int>(variable_id, _wtoi(value_string.c_str()));
		break;
	case VariableFloatArray:
		_variables->Set<double>(variable_id, _wtof(value_string.c_str()));
		break;
	case VariableStringArray:
		_variables->Set<std::wstring>(variable_id, value_string.c_str());
		break;
	default:
		throw(CompileError(tokens.GetCurrentToken(),
			CompileErrorTypeExpected, L"Simple array element Expected one of float, int, string, bool."));
		break;
	}
	return true;
}

bool ScanFileParser::ProcessArrayElementAssignment(const wchar_t* variable_id, Tokens& tokens)
{
	auto variable = _variables->GetVariable(variable_id);
	assert(variable != nullptr);
	tokens.Next();
	wstring variable_id_str = variable_id + wstring(L"[") + tokens.GetCurrentToken().text + wstring(L"]");
	
	size_t index = (size_t)std::stoi(tokens.GetCurrentToken().text);
	if (index >= _variables->GetArraySize(variable_id))
	{
		_variables->ResizeArray(variable_id, index + 1);
	}
	
	tokens.Next();
	tokens.AssertToken(TokenRightSquareBracket, true);
	switch (variable->GetType())
	{
	case VariableBoolArray:
	case VariableIntArray:
	case VariableFloatArray:
	case VariableStringArray:
		ProcessSimpleArrayElementAssignment(variable_id_str.c_str(), variable->GetType(), tokens);
		break;
	case VariableStructArray:
		ProcessStructAssignment(variable_id_str.c_str(), tokens);
		break;
	default:
		throw(CompileError(tokens.GetCurrentToken(),
			CompileErrorTypeExpected, L"Array element Expected one of float, int, string, bool, struct."));
		break;
	}
	//
	return true;
}

// 支持所有元素赋初值,仅限简单类型 scanarray = [0, 1, 2, 3]
bool ScanFileParser::ProcessAllSimpleArrayElementAssighment(const wchar_t* variable_id, Tokens& tokens)
{
	auto variable = _variables->GetVariable(variable_id);
	assert(variable != nullptr);
	tokens.AssertToken(TokenOperatorAssign, true);
	tokens.AssertToken(TokenLeftSquareBracket, true);

	unsigned int index = 0;
	while (tokens.GetCurrentToken().type != TokenSemiColon)
	{
		if (index >= _variables->GetArraySize(variable_id))
		{
			_variables->ResizeArray(variable_id, index + 1);
		}

		wstring value_string;
		while (tokens.GetCurrentToken().type != TokenComma && tokens.GetCurrentToken().type != TokenRightSquareBracket)
		{
			if (tokens.GetCurrentToken().type == TokenStringLiteral)
			{
				value_string += L'\"';
				value_string += tokens.GetCurrentToken().text;
				value_string += L'\"';
			}
			else
			{
				value_string += tokens.GetCurrentToken().text;
			}
			tokens.Next();
		}
		wchar_t str[25];
		_itow_s(index, str, 10);
		wstring variable_id_str;
		variable_id_str = variable_id + wstring(L"[") + str + wstring(L"]");
		switch (variable->GetType())
		{
		case VariableBoolArray:
			_variables->Set<bool>(variable_id_str.c_str(), _wtoi(value_string.c_str()));
			break;
		case VariableIntArray:
			_variables->Set<int>(variable_id_str.c_str(), _wtoi(value_string.c_str()));
			break;
		case VariableFloatArray:
			_variables->Set<double>(variable_id_str.c_str(), _wtof(value_string.c_str()));
			break;
		case VariableStringArray:
			_variables->Set<std::wstring>(variable_id_str.c_str(), value_string.c_str());
			break;
		default:
			throw(CompileError(tokens.GetCurrentToken(),
				CompileErrorTypeExpected, L"Simple array element Expected one of float, int, string, bool."));
			break;
		}

		++index;
		tokens.Next();
	}

	return true;
}

bool ScanFileParser::ProcessArrayAssignment(const wchar_t* variable_id, Tokens& tokens)
{
	auto variable = _variables->GetVariable(variable_id);
	if (variable == nullptr)
	{
		return false;
	}

	if (tokens.GetCurrentToken().type == TokenOperatorDot) // 支持设置array大小
	{
		return ProcessArrayResizeOperator(variable_id, tokens);
	}
	else if(tokens.GetCurrentToken().type == TokenLeftSquareBracket) // 支持设置某个元素的值*[0];
	{
		return ProcessArrayElementAssignment(variable_id, tokens);
	}
	else
	{
		return ProcessAllSimpleArrayElementAssighment(variable_id, tokens);
	}
}

bool ScanFileParser::ProcessAssignment()
{
	assert(_preprocessor);
	assert(_variables);

	auto tokens = _preprocessor->GetTokens();
	while (!tokens.AtEnd())
	{
		Tokens::Guard guard(tokens);
		auto type = tokens.GetCurrentToken().type;
		assert(type == TokenId);
		auto variable_id = tokens.GetCurrentToken().text;
		tokens.Next();
		if (tokens.GetCurrentToken().type == TokenDoubleColon)
		{
			tokens.Next();
			variable_id = variable_id + wstring(L"::") + tokens.GetCurrentToken().text;
			tokens.Next();
		}
		auto variable_type = _variables->GetVariable(variable_id.c_str())->GetType();
		switch (variable_type)
		{
		case VariableBool:
		case VariableInt:
		case VariableFloat:
		case VariableString:
		case VariableStructArray:
			ProcessSimpleAssignment(variable_id.c_str(), tokens);
			break;
		case VariableStruct:
			ProcessStructAssignment(variable_id.c_str(), tokens);
			break;
		case VariableIntArray:
		case VariableFloatArray:
		case VariableStringArray:
		case VariableBoolArray:
			ProcessArrayAssignment(variable_id.c_str(), tokens);
			break;
		default:
			throw(CompileError(tokens.GetCurrentToken(),
				CompileErrorTypeExpected, L"Expected one of float, int, string, bool, array, struct."));
			break;
		}
	}
	return true;
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
				auto member_type = statement.GetCurrentToken().type;
				auto member_id = statement.GetCurrentToken().text;
				assert(member_type == TokenId);
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
