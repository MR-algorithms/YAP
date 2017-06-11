#include "PipelineCompiler.h"
#include "PipelineConstructor.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <ctype.h>
#include <cassert>

using namespace Yap;
using namespace std;

PipelineCompiler::PipelineCompiler(void) :
	_preprocessor(make_shared<Preprocessor>(PreprocessPipeline))
{}

PipelineCompiler::~PipelineCompiler(void)
{}

/**
	Process an "import" statement in the file.
*/
bool PipelineCompiler::ProcessImport(Statement& statement)
{
	assert(statement.GetType() == StatementImport);
	Statement::Guard guard(statement);

	statement.AssertToken(TokenKeywordImport, true);
	wstring path = statement.GetStringLiteral();

	if (!_constructor->LoadModule(path.c_str()))
	{
		wstring output = wstring(L"Failed to load specified plug-in. "
								 "Check to see if the file and its dependent files exist.") + path;
		throw CompileError(statement.GetCurrentToken(), CompileErrorLoadModule, output);
	}

	statement.AssertToken(TokenSemiColon, false); //是不是可以去掉？

	return true;
}

/**
Port link is in the form of: process1.output_port->process2.input_port;
If the output_port and input_port is not specified, they default to "Output" and "Input" respectively.
*/
bool PipelineCompiler::ProcessPortLink(Statement& statement)
{
	assert(_constructor);
	Statement::Guard guard(statement);

	auto source_processor = statement.GetId();

	wstring source_port(L"Output");
	if (statement.IsTokenOfType(TokenOperatorDot, true))
	{
		source_port = statement.GetId();
	}

	statement.AssertToken(TokenOperatorPortLink, true);

	auto dest_processor = statement.GetId();
	wstring dest_port(L"Input");
	if (statement.IsTokenOfType(TokenOperatorDot, true))
	{
		dest_port = statement.GetId();
	}

	if (source_processor == L"self")
	{
		if (dest_processor == L"self")
		{
			throw CompileError(statement.GetCurrentToken(), CompileErrorSelfLink,
				L"Can not link the output of the pipeline to the input of itself.");
		}
		else
		{
			return _constructor->MapInput(source_port.c_str(), dest_processor.c_str(), dest_port.c_str());
		}
	}
	else if (dest_processor == L"self")
	{
		return _constructor->MapOutput(dest_port.c_str(), source_processor.c_str(), source_port.c_str());
	}
	else
	{
		return _constructor->Link(source_processor.c_str(), source_port.empty() ? L"Output" : source_port.c_str(),
			dest_processor.c_str(), dest_port.empty() ? L"Input" : dest_port.c_str());
	}
}

bool PipelineCompiler::ProcessDeclaration(Statement& statement)
{
	assert(_constructor);
	assert(statement.GetType() == StatementDeclaration);

	Statement::Guard guard(statement);

	wstring class_id = statement.GetId();
	wstring instance_id = statement.GetId();

	if (_constructor->InstanceIdExists(instance_id.c_str()))
	{
		throw CompileError(statement.GetToken(1), CompileErrorIdExists,
			wstring(L"Instance id specified for the processor already exists: ") + instance_id);
	}

	if (statement.IsTokenOfType(TokenSemiColon, false))
	{
		_constructor->CreateProcessor(class_id.c_str(), instance_id.c_str());
	}
	else if (!statement.IsTokenOfType(TokenLeftParenthesis, true)) 
	{
		// must be the parameter list.
		throw CompileError(statement.GetCurrentToken(), CompileErrorSemicolonExpected,
			L"Semicolon or left parenthesis expected.");
	}
	else
	{
		_constructor->CreateProcessor(class_id.c_str(), instance_id.c_str());
		for (;;)
		{
			wstring property = statement.GetId();

			if (statement.IsTokenOfType(TokenOperatorAssign, true))
			{
				wstring value = statement.GetLiteralValue();
				_constructor->SetProperty(instance_id.c_str(), property.c_str(), value.c_str());
			}
			else if (statement.IsTokenOfType(TokenOperatorInOutMapping, true))
			{
				wstring variable_id = statement.GetVariableId();
				_constructor->MapProperty(instance_id.c_str(), property.c_str(), variable_id.c_str(), true, true);
			}
			else if (statement.IsTokenOfType(TokenOperatorInMapping, true))
			{
				wstring variable_id = statement.GetVariableId();
				_constructor->MapProperty(instance_id.c_str(), property.c_str(), variable_id.c_str(), true, false);
			}
			else if (statement.IsTokenOfType(TokenOperatorOutMapping, true))
			{
				wstring variable_id = statement.GetVariableId();
				_constructor->MapProperty(instance_id.c_str(), property.c_str(), variable_id.c_str(), false, true);
			}
			else
			{
				throw CompileError(statement.GetCurrentToken(), CompileErrorPropertyOperatorExpected,
					L"Property operator must be specified, you can use either \'=\' or \'<=>\'.");
			}

			if (statement.IsTokenOfType(TokenRightParenthesis, true))
			{
				break;
			}
			else if (!statement.IsTokenOfType(TokenComma, true))
			{
				throw CompileError(statement.GetCurrentToken(), CompileErrorCommaExpected,
					L"Comma \',\' or right parenthesis \')\' expected.");
			}
		}
	}

	return true;
}

bool PipelineCompiler::ProcessPropertyMapping(Statement& statement)
{
	assert(_constructor);
	assert(statement.GetType() == StatementPropertyMapping);
	Statement::Guard guard(statement);

	wstring processor_instance_id = statement.GetId();
	if (!_constructor->InstanceIdExists(processor_instance_id.c_str()))
	{
		throw CompileError(statement.GetToken(0), CompileErrorProcessorNotFound,
			wstring(L"Processor not found: ") + processor_instance_id);
	}

	statement.AssertToken(TokenOperatorDot, true);
	wstring property = statement.GetId();

	bool input = false, output = false;
	if (statement.IsTokenOfType(TokenOperatorInOutMapping, true))
	{
		input = true;
		output = true;
	}
	else if (statement.IsTokenOfType(TokenOperatorInMapping, true))
	{
		input = true;
	}
	else if (statement.IsTokenOfType(TokenOperatorOutMapping, true))
	{
		output = true;
	}
	else
	{
		throw CompileErrorTokenType(statement.GetCurrentToken(), TokenOperatorInOutMapping);
	}

	wstring variable_id = statement.GetVariableId();

	return _constructor->MapProperty(processor_instance_id.c_str(), property.c_str(), variable_id.c_str(), input, output);
}

bool PipelineCompiler::ProcessAssignment(Statement& statement)
{
	assert(_constructor);
	assert(statement.GetType() == StatementAssign);
	Statement::Guard guard(statement);

	wstring processor_instance_id = statement.GetId();
	if (!_constructor->InstanceIdExists(processor_instance_id.c_str()))
	{
		throw CompileError(statement.GetToken(0), CompileErrorProcessorNotFound,
			wstring(L"Processor not found: ") + processor_instance_id);
	}

	statement.AssertToken(TokenOperatorDot, true);
	wstring property = statement.GetId();

	statement.AssertToken(TokenOperatorAssign, true);
	wstring value = statement.GetLiteralValue();

	return _constructor->SetProperty(processor_instance_id.c_str(), property.c_str(), value.c_str());
}

Yap::SmartPtr<CompositeProcessor> Yap::PipelineCompiler::Compile(const wchar_t * text)
{
	wistringstream input;
	input.str(text);

	return DoCompile(input);
}

Yap::SmartPtr<CompositeProcessor> PipelineCompiler::CompileFile(const wchar_t * path)
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

Yap::SmartPtr<CompositeProcessor> PipelineCompiler::DoCompile(std::wistream& input)
{
	_preprocessor->Preprocess(input);

	if (!_constructor)
	{
		_constructor = shared_ptr<PipelineConstructor>(new PipelineConstructor);
	}

	try
	{
		_constructor->Reset(true);
		Process();

		return _constructor->GetPipeline();
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

		return SmartPtr<CompositeProcessor>();
	}
}

bool PipelineCompiler::Process()
{
	assert(_preprocessor);
	auto statement = _preprocessor->GetStatement();

	while (!statement.AtEnd())
	{
		switch (statement.GetCurrentToken().type)
		{
			case TokenKeywordImport:
				if (!statement.IsCurrentStatementEmpty())
				{
					statement.SetType(StatementImport);
					ProcessImport(statement);
				}
				else
				{
                    throw CompileError(statement.GetCurrentToken(), CompileErrorInvalidImport,
									   wstring(L"Invalid use of keyword import."));
				}
				break;
			case TokenOperatorAssign:
				if (statement.GetType() == StatementUnknown)
				{
					statement.SetType(StatementAssign);
					ProcessAssignment(statement);
				}
				else
				{
					statement.Next();
				}
				break;
			case TokenOperatorInOutMapping:
			case TokenOperatorInMapping:
			case TokenOperatorOutMapping:
				if (statement.GetType() == StatementUnknown)
				{
					statement.SetType(StatementPropertyMapping);
					ProcessPropertyMapping(statement);
				}
				else
				{
					statement.Next();
				}
				break;
			case TokenOperatorPortLink:
				statement.SetType(StatementPortLink);
				ProcessPortLink(statement);
				break;
			case TokenId:
			case TokenKeywordSelf:
				if (statement.IsFirstTokenInStatement() && statement.IsTokenOfType(TokenId) && 
					statement.IsNextTokenOfType(TokenId))
				{
					statement.SetType(StatementDeclaration);
					ProcessDeclaration(statement);
				}
				else
				{
					statement.Next();
				}
				break;
			case TokenSemiColon:
				if (!statement.IsCurrentStatementEmpty() && statement.GetType() == StatementUnknown)
				{
					throw(CompileError(statement.GetLastToken(),
									   CompileErrorIncompleteStatement, L"Statement not complete."));
				}
				else
				{
					statement.Next();
				}
			case TokenOperatorDot:
				statement.Next();
			default:
				;
		}
	}

	return true;
}
