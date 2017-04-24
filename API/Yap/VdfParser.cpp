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
	_variables{shared_ptr<VariableManager>()}
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
		_variables = shared_ptr<VariableManager>(new VariableManager());
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
        case TokenId:
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
            break;
            default:
				throw(CompileError(statement.GetCurrentToken(),
                                   CompileErrorTypeExpected, L"Expected one of float, int, string, bool, array, struct."));
		}
	}

	return true;
}

bool VdfParser::ProcessSimpleDeclaration(Statement& statement)
{
    assert(_variables);

	Statement::Guard guard(statement);
	auto type = statement.GetCurrentToken().type;
    auto type_id = statement.GetCurrentToken().text;

	assert(type == TokenKeywordFloat || type == TokenKeywordInt ||
           type == TokenKeywordBool || type == TokenKeywordString ||
           type == TokenId);
	statement.Next();
	auto id = statement.GetVariableId();
	if (!statement.IsTokenOfType(TokenSharp, true))
	{
        _variables->Add(type_id.c_str(), id.c_str(), L"");
	}
	else
	{
		statement.AssertToken(TokenLessThen, true);
		auto start_index = _wtoi(statement.GetLiteralValue().c_str());
		statement.AssertToken(TokenComma, true);
		auto end_index = _wtoi(statement.GetLiteralValue().c_str());
		statement.AssertToken(TokenGreaterThen, true);

		for (int i = start_index; i <= end_index; ++i)
		{
			wostringstream output;
			output << id << i;
            _variables->Add(type_id.c_str(), output.str().c_str(), L"");
		}
	}

	return true;
}

bool VdfParser::ProcessArrayDeclaration(Statement& statement)
{
    static map<TokenType, int> token_to_array_property{
        {TokenKeywordFloat, VariableFloatArray},
        {TokenKeywordInt, VariableIntArray},
        {TokenKeywordString, VariableStringArray},
        {TokenKeywordBool, VariableBoolArray},
		{TokenId, VariableStructArray},
    };

    Statement::Guard guard(statement);
	statement.AssertToken(TokenKeywordArray, true);
	statement.AssertToken(TokenLessThen, true);

	auto type = statement.GetCurrentToken().type;
	assert(type == TokenKeywordFloat || type == TokenKeywordInt ||
		   type == TokenKeywordBool || type == TokenKeywordString ||
			type == TokenId);
	statement.Next();
	statement.AssertToken(TokenGreaterThen, true);

	auto id = statement.GetVariableId();
	if (!statement.IsTokenOfType(TokenSharp, true))
	{
		_variables->Add(token_to_array_property[type], id.c_str(), L"");
	}
	else
	{
		statement.AssertToken(TokenLessThen, true);
		auto start_index = _wtoi(statement.GetLiteralValue().c_str());
		statement.AssertToken(TokenComma, true);
		auto end_index = _wtoi(statement.GetLiteralValue().c_str());
		statement.AssertToken(TokenGreaterThen, true);

		for (int i = start_index; i <= end_index; ++i)
		{
			wostringstream output;
			output << id << i;
			_variables->Add(token_to_array_property[type], output.str().c_str(), L"");
		}
	}

    return true;
}

bool VdfParser::ProcessStructDeclaration(Statement& statement)
{
	Statement::Guard guard(statement);
    statement.AssertToken(TokenKeywordStruct, true);
    auto struct_id = statement.GetVariableId();
    statement.AssertToken(TokenLeftBrace, true);

    VariableManager struct_variables;

    do
    {
        auto& type_token = statement.GetCurrentToken();
        if (type_token.type != TokenKeywordBool && type_token.type != TokenKeywordFloat &&
                type_token.type != TokenKeywordInt && type_token.type != TokenKeywordString &&
                type_token.type != TokenId)
        {
            throw(CompileError(type_token, CompileErrorTypeExpected,
                               L"Expected either one of float, int, string, bool, or a struct name."));
        }

        if (type_token.type == TokenId)
        {
            if (!_variables->TypeExists(type_token.text.c_str()))
                throw CompileError(type_token, CompileErrorTypeNotFound, L"Type not found.");

            if (type_token.text == struct_id)
                throw CompileError(type_token, CompileErrorNestStruct, L"Nested struct definition not allowed.");

        }
        statement.Next();
        auto& member_token = statement.GetCurrentToken();

        if (struct_variables.VariableExists(member_token.text.c_str()))
            throw CompileError(type_token, CompileErrorMemeberExists, L"Duplicated struct member ids.");


        auto prototype = _variables->GetType(type_token.text.c_str());
        if (prototype == nullptr)
            throw CompileError(type_token, CompileErrorTypeNotFound, L"Type not found.");

        auto member = dynamic_cast<IVariable*>(prototype->Clone());
        assert(member != nullptr);
        member->SetId(member_token.text.c_str());
        struct_variables.Add(member);

        statement.Next();
        statement.AssertToken(TokenSemiColon, true);

    } while (!statement.IsTokenOfType(TokenRightBrace, false));

	statement.Next();
    statement.AssertToken(TokenSemiColon);

    _variables->AddType(struct_id.c_str(), struct_variables.Variables());

    return true;
}

