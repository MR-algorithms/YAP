#include "VdfParser.h"
#include "Interface/Implement/VariableManager.h"
#include "Preprocessor.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

VdfParser::VdfParser() :
	_preprocessor{make_shared<Preprocessor>(PreprocessVariableDefinition)},
	_variables{make_shared<VariableManager>()}
{}

VdfParser::~VdfParser()
{}

shared_ptr<VariableManager> Yap::VdfParser::Compile(const wchar_t * text)
{
	wistringstream input;
	input.str(text);

	return DoCompile(input);
}

shared_ptr<VariableManager> VdfParser::CompileFile(const wchar_t * path)
{
	wifstream script_file;
	script_file.open(path);

	if (!script_file)
	{
		wstring message = wstring(L"Failed to open script file: ") + path;
		// throw CompileError(Token(), CompileErrorFailedOpenFile, message);
		return shared_ptr<VariableManager>();
	}

	return DoCompile(script_file);
}

shared_ptr<VariableManager> VdfParser::DoCompile(std::wistream& input)
{
	_preprocessor->Preprocess(input);

	if (!_variables)
	{
		_variables = make_shared<VariableManager>();
	}
	_variables->Reset();

	try
	{
		if (Process())
		{
			return _variables;
		}

	}
	catch (CompileError& e)
	{
		wostringstream output;
		output << L"Line " << e.GetToken().line + 1
			<< L" Column " << e.GetToken().column + 1
			<< ": Error code " << e.GetErrorNumber()
			<< " " << e.GetErrorMessage();

        wstring erorr_messge = output.str();
		wcerr << output.str();
	}

	return shared_ptr<VariableManager>();
}

bool VdfParser::Process()
{
	assert(_preprocessor);
	auto statement = _preprocessor->GetStatement();

	while (!statement.AtEnd())
	{
		switch (statement.GetCurrentToken().type)
		{
			case TokenKeywordBool:
			case TokenKeywordInt:
			case TokenKeywordFloat:
			case TokenKeywordString:
				ProcessSimpleDeclaration(statement);
				break;
			case TokenKeywordArray:
				ProcessArrayDeclaration(statement);
				break;
			case TokenKeywordStruct:
				ProcessStructDeclaration(statement);
				break;
			case TokenSemiColon:
				statement.FinishProcessingStatement();
			default:
				throw(CompileError(statement.GetCurrentToken(),
								   CompilerErrorTypeExpected, L"Expected one of float, int, string, bool, array, struct."));
				;
		}
	}

	return true;
}

static map<TokenType, PropertyType> token_to_property{
	{TokenKeywordFloat, PropertyFloat},
	{TokenKeywordInt, PropertyInt},
	{TokenKeywordString, PropertyString},
	{TokenKeywordBool, PropertyBool},
};

bool VdfParser::ProcessSimpleDeclaration(Statement& statement)
{
	assert(_variables);

	Statement::Guard guard(statement);
	auto type = statement.GetCurrentToken().type;
	assert(type == TokenKeywordFloat || type == TokenKeywordInt ||
		   type == TokenKeywordBool || type == TokenKeywordString);
	statement.Next();
	auto id = statement.GetVariableId();

	_variables->AddProperty(token_to_property[type], id.c_str(), L"");

	return true;
}

bool VdfParser::ProcessArrayDeclaration(Statement& statement)
{
	Statement::Guard guard(statement);
	return false;
}

bool VdfParser::ProcessStructDeclaration(Statement& statement)
{
	Statement::Guard guard(statement);
	return false;
}

