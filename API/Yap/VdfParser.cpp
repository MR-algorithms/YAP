#include "VdfParser.h"
#include "Interface/Implement/VariableManager.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

VdfParser::VdfParser()
{}

VdfParser::~VdfParser()
{}

bool Yap::VdfParser::Compile(const wchar_t * text)
{
	wistringstream input;
	input.str(text);

	return DoCompile(input);
}

bool VdfParser::CompileFile(const wchar_t * path)
{
	wifstream script_file;
	script_file.open(path);

	if (!script_file)
	{
		wstring message = wstring(L"Failed to open script file: ") + path;
		// throw CompileError(Token(), CompileErrorFailedOpenFile, message);
		return SmartPtr<CompositeProcessor>();
	}

	return DoCompile(script_file);
}

bool VdfParser::DoCompile(std::wistream& input)
{
// 	Preprocess(input);
// 
// 	if (!_constructor)
// 	{
// 		_constructor = shared_ptr<PipelineConstructor>(new PipelineConstructor);
// 	}
// 	_constructor->Reset(true);
// 
// 	try
// 	{
// 		if (Process())
// 		{
// 			return _constructor->GetPipeline();
// 		}
// 
// 		return Yap::SmartPtr<CompositeProcessor>();
// 	}
// 	catch (CompileError& e)
// 	{
// 		wostringstream output;
// 		output << L"Line " << e.GetToken().line + 1
// 			<< L" Column " << e.GetToken().column + 1
// 			<< ": Error code " << e.GetErrorNumber()
// 			<< " " << e.GetErrorMessage();
// 
//         wstring erorr_messge = output.str();
// 		wcerr << output.str();
// 
// 		return SmartPtr<CompositeProcessor>();
// 	}

	return false;
}

bool VdfParser::Process()
{
// 	Statement statement(*_constructor);
// 
// 	for (auto token : _tokens)
// 	{
// 		token.text = GetTokenString(token);
// 
// 		switch (token.type)
// 		{
// 		case TokenKeyword:
// 			if (token.text == L"import" && statement.IsCurrentStatementEmpty())
// 			{
// 				statement.SetType(StatementImport);
// 			}
// 			break;
// 		case TokenOperatorAssign:
// 			if (statement.GetType() == StatementUnknown)
// 			{
// 				statement.SetType(StatementAssign);
// 			}
// 			break;
// 		case TokenOperatorLink:
// 			if (statement.GetType() == StatementUnknown)
// 			{
// 				statement.SetType(StatementPropertyLink);
// 			}
// 			break;
// 		case TokenOperatorPortLink:
// 			statement.SetType(StatementPortLink);
// 			break;
// 		case TokenId:
// 			if (statement.GetTokenCount() == 1 && statement.GetToken(0).type == TokenId)
// 			{
// 				statement.SetType(StatementDeclaration);
// 			}
// 			break;
// 		case TokenSemiColon:
// 			if (!statement.IsCurrentStatementEmpty())
// 			{
// 				if (statement.GetType() == StatementUnknown)
// 				{
// 					throw(CompileError(statement.GetLastToken(), 
// 						CompileErrorIncompleteStatement, L"Statement not complete."));
// 				}
// #ifdef DEBUG
// 				DebugOutputStatement(cout, statement);
// #endif
// 				statement.Process();
// 				statement.Reset();
// 			}
// 			break;
// 		default:
// 			;
// 		}
// 
// 		if (token.type != TokenSemiColon)
// 		{
// 			statement.AddToken(token);
// 		}
// 	}

	return true;
}
