#include "VdfParser.h"
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

VdfParser::VdfParser() :
	_preprocessor{make_shared<Preprocessor>(PreprocessVariableDefinition)},
	_variables{shared_ptr<VariableSpace>()}
{
}

VdfParser::~VdfParser()
{}

shared_ptr<VariableSpace> Yap::VdfParser::Compile(const wchar_t * text)
{
	wistringstream input;
	input.str(text);

	return DoCompile(input);
}

shared_ptr<VariableSpace> VdfParser::CompileFile(const wchar_t * path)
{
	wifstream script_file;
	script_file.open(path);

	if (!script_file)
	{
		wstring message = wstring(L"Failed to open script file: ") + path;
		// throw CompileError(Token(), CompileErrorFailedOpenFile, message);
		return shared_ptr<VariableSpace>();
	}

	return DoCompile(script_file);
}

shared_ptr<VariableSpace> VdfParser::DoCompile(std::wistream& input)
{
	_preprocessor->Preprocess(input);

	if (!_variables)
	{
		_variables = shared_ptr<VariableSpace>(new VariableSpace());
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

	return shared_ptr<VariableSpace>();
}

bool VdfParser::Process()
{
	assert(_preprocessor);
	auto tokens = _preprocessor->GetTokens();

	return ProcessStatement(tokens);
}

bool VdfParser::ProcessStatement(Tokens& tokens)
{
	while (!tokens.AtEnd())
	{
		switch (tokens.GetCurrentToken().type)
		{
        case TokenKeywordBool:
        case TokenKeywordInt:
        case TokenKeywordFloat:
        case TokenKeywordString:
			ProcessSimpleDeclaration(tokens);
			break;
        case TokenId:
            ProcessSimpleDeclaration(tokens);
            break;
        case TokenKeywordArray:
            ProcessArrayDeclaration(tokens, *_variables);
            break;
        case TokenKeywordStruct:
            ProcessStructDeclaration(tokens);
            break;
		case TokenKeywordNamespace:
			ProcessNamespace(tokens);
			break;
		case TokenKeywordUsing:
			ProcessUsing(tokens);
			break;
        case TokenSemiColon:
            tokens.FinishProcessingStatement();
            break;
		case TokenRightBrace:
			// This function maybe called when the processing iterator is pointing to 
			// the beginning of a tokens, which maybe hosted in a namespace.
			// If current tokens is enclosed in a namespace, then the right brace
			// means the end of namespace.
			if (_current_namespace.empty())
			{
				throw CompileError(tokens.GetCurrentToken(), CompileErrorTypeExpected,
					L"Unexpected \'}\'");
			}
			else
				return true;
        default:
			throw(CompileError(tokens.GetCurrentToken(),
                  CompileErrorTypeExpected, L"Expected one of float, int, string, bool, array, struct."));
		}
	}

	return true;
}

bool VdfParser::ProcessNamespace(Tokens& tokens)
{
	assert(_variables);
	Tokens::Guard guard(tokens, false); // namespace declaration ended with right brace, no semi-colon.
	tokens.AssertToken(TokenKeywordNamespace, true);
	wstring namespace_id = tokens.GetId();
	_current_namespace = _current_namespace.empty() ? namespace_id :
		(_current_namespace + L"::" + namespace_id);

	tokens.AssertToken(TokenLeftBrace, true);
	ProcessStatement(tokens);
	tokens.AssertToken(TokenRightBrace, true);	
	_current_namespace = OuterNamespace(_current_namespace);

	return true;
}

bool VdfParser::ProcessUsing(Tokens& tokens)
{
	assert(_variables);
	Tokens::Guard guard(tokens);
	tokens.AssertToken(TokenKeywordUsing, true);
	tokens.AssertToken(TokenKeywordNamespace, true);
	_namespace_in_use.push_back(tokens.GetNamespaceId());

	return true;
}

bool VdfParser::ProcessSimpleDeclaration(Tokens& tokens)
{
    assert(_variables);

	Tokens::Guard guard(tokens);
	auto type = tokens.GetCurrentToken().type;
    auto type_id = tokens.GetCurrentToken().text;

	assert(type == TokenKeywordFloat || type == TokenKeywordInt ||
           type == TokenKeywordBool || type == TokenKeywordString ||
           type == TokenId);

	tokens.Next();
	auto id = tokens.GetId();
	auto fq_type_id = ResolveFqId(type_id.c_str(), true, tokens);

	if (!tokens.IsTokenOfType(TokenSharp, true))
	{
        _variables->Add(fq_type_id.c_str(), GetFqId(id.c_str()).c_str(), L"");
	}
	else
	{
		tokens.AssertToken(TokenLessThan, true);
		auto start_index = _wtoi(tokens.GetLiteralValue().c_str());
		tokens.AssertToken(TokenComma, true);
		auto end_index = _wtoi(tokens.GetLiteralValue().c_str());
		tokens.AssertToken(TokenGreaterThan, true);

		for (int i = start_index; i <= end_index; ++i)
		{
			wostringstream output;
			output << id << i;
			wstring variable_id = GetFqId(output.str().c_str());
			
            _variables->Add(fq_type_id.c_str(), variable_id.c_str(), L"");
		}
	}

	return true;
}

bool VdfParser::IdExists(const wchar_t * const id, bool is_type_id) const
{
	return is_type_id ? _variables->TypeExists(id) : _variables->VariableExists(id);
}

/**
	\brief Returns full-qualified type id corresponding to the specify type id.

	\note If the type id is not found, or multiple types in different namespace
	in use matches the type id, compile error will be thrown.

	\remarks Namespace: About namespace in Variable Definition File

	VdfParser should manage the use of namespace in file by itself, and only passing
	full-qualified ids (including type ids and variable ids) to the VariableSpace.
	Whether variables will be organized in namespaces or namespaces are only regarded
	as part of the id is not defined in the interface.

	Whenever a new type or object is defined, it's id should be added to the current
	namespace. When a type or variable id is met, unless the id is prefixed by '::'
	(in which case the parser will try to find the id in the global namespace), VDF
	parser will try to find the id in the following steps:

	1. Try to find the id in current namespace , then in the namespace enclosing 
	current namespace, and then in namespace enclosing the enclosing namespace, ...,
	till global namespace. The search stops at the first encounter of the id.

	2. Try to find all the matching ids in namespaces declared with 'using'. Including 
	the matching id returned in the first step, if more than 1 id is found, then
	the compiler should complaining about the id ambiguity.

	This behavior is consistent with C++.
*/

std::wstring VdfParser::ResolveFqId(const wchar_t * const id, 
	bool is_type_id, 
	Tokens& tokens) const
{
	vector<wstring> matching_fq_ids;
	assert(_variables);
	wstring search_namespace = _current_namespace;

	for (;;)
	{
		wstring fq_id = search_namespace.empty() ? id : (search_namespace + L"::" + id);
		if (IdExists(fq_id.c_str(), is_type_id))
		{
			matching_fq_ids.push_back(fq_id);
			break;
		}
		else if (search_namespace.empty())
		{
			break;
		}
		else
		{
			search_namespace = OuterNamespace(search_namespace);
		}
	}

	for (auto name_space : _namespace_in_use)
	{
		assert(!name_space.empty());

		wstring fq_id = name_space + L"::" + id;
		if (IdExists(fq_id.c_str(), is_type_id))
		{
			matching_fq_ids.push_back(fq_id);
		}
	}

	if (matching_fq_ids.empty())
	{
		throw CompileError(tokens.GetCurrentToken(), CompileErrorTypeNotFound, wstring(L"Type not found: ") + id);
	}
	else if (matching_fq_ids.size() > 1)
	{
		wstring error_message = L"Ambiguous ID: ";
		error_message += id;
		error_message += L". Could be: \n";
		for (auto fq_id : matching_fq_ids)
		{
			error_message += (fq_id + L"\n");
		}

		throw CompileError(tokens.GetCurrentToken(), CompileErrorAmbiguousId, error_message);
	}

	return matching_fq_ids[0];
}

std::wstring VdfParser::OuterNamespace(const std::wstring& inner_namespace) const
{
	assert(!inner_namespace.empty());

	auto separator_pos = inner_namespace.rfind(L"::");
	return  (separator_pos == wstring::npos) ? L"" :
		inner_namespace.substr(0, separator_pos);
}

std::wstring VdfParser::GetFqId(const wchar_t * const id) const
{
	return (_current_namespace.empty()) ? id : (_current_namespace + L"::" + id);
}

bool VdfParser::ProcessArrayDeclaration(Tokens& tokens, VariableSpace& variables)
{
    static map<TokenType, int> token_to_array_property{
        {TokenKeywordFloat, VariableFloatArray},
        {TokenKeywordInt, VariableIntArray},
        {TokenKeywordString, VariableStringArray},
        {TokenKeywordBool, VariableBoolArray},
		{TokenId, VariableStructArray},
    };

    Tokens::Guard guard(tokens);
	tokens.AssertToken(TokenKeywordArray, true);
	tokens.AssertToken(TokenLessThan, true);

	auto type = tokens.GetCurrentToken().type;
	auto element_type_id = tokens.GetCurrentToken().text;
	auto fq_type_id = ResolveFqId(element_type_id.c_str(), true, tokens);

	assert(type == TokenKeywordFloat || type == TokenKeywordInt ||
		   type == TokenKeywordBool || type == TokenKeywordString ||
			type == TokenId);
	tokens.Next();
	tokens.AssertToken(TokenGreaterThan, true);

	auto id = (&variables == _variables.get()) ?
		GetFqId(tokens.GetId().c_str()) : tokens.GetId();

	if (!tokens.IsTokenOfType(TokenSharp, true))
	{
		variables.AddArray(fq_type_id.c_str(), id.c_str(), L"");
	}
	else
	{
		tokens.AssertToken(TokenLessThan, true);
		auto start_index = _wtoi(tokens.GetLiteralValue().c_str());
		tokens.AssertToken(TokenComma, true);
		auto end_index = _wtoi(tokens.GetLiteralValue().c_str());
		tokens.AssertToken(TokenGreaterThan, true);

		for (int i = start_index; i <= end_index; ++i)
		{
			wostringstream output;
			output << id << i;
			auto variable_id = (&variables == _variables.get()) ?
				GetFqId(output.str().c_str()) : output.str();
			variables.AddArray(fq_type_id.c_str(), variable_id.c_str(), L"");
		}
	}

    return true;
}

bool VdfParser::ProcessStructDeclaration(Tokens& tokens)
{
	Tokens::Guard guard(tokens);
    tokens.AssertToken(TokenKeywordStruct, true);
    auto struct_id = GetFqId(tokens.GetId().c_str());

    tokens.AssertToken(TokenLeftBrace, true);

    VariableSpace struct_variables;
	static std::set<TokenType> s_allowed_types = {
		TokenKeywordBool, TokenKeywordFloat, TokenKeywordInt, TokenKeywordString,
		TokenKeywordArray, TokenId };
    do
    {
        auto& type_token = tokens.GetCurrentToken();
        if (s_allowed_types.find(type_token.type) == s_allowed_types.end())
        {
            throw(CompileError(type_token, CompileErrorTypeExpected,
                               L"Expected either one of float, int, string, bool, array, or a struct name."));
        }

		if (type_token.type == TokenKeywordArray)
		{
			ProcessArrayDeclaration(tokens, struct_variables);
		}
		else
		{
			auto type_id = ResolveFqId(type_token.text.c_str(), true, tokens);
			if (type_token.type == TokenId)
			{
				if (!_variables->TypeExists(type_id.c_str()))
					throw CompileError(type_token, CompileErrorTypeNotFound, L"Type not found.");

				if (type_token.text == struct_id)
					throw CompileError(type_token, CompileErrorNestStruct, L"Nested struct definition not allowed.");

			}

			tokens.Next();
			auto& member_token = tokens.GetCurrentToken();

			if (struct_variables.VariableExists(member_token.text.c_str()))
				throw CompileError(type_token, CompileErrorMemeberExists, L"Duplicated struct member ids.");


			auto prototype = _variables->GetType(type_id.c_str());
			if (prototype == nullptr)
				throw CompileError(type_token, CompileErrorTypeNotFound, L"Type not found.");

			auto member = dynamic_cast<IVariable*>(prototype->Clone());
			assert(member != nullptr);
			member->SetId(member_token.text.c_str());
			struct_variables.Add(member);

			tokens.Next();
			tokens.AssertToken(TokenSemiColon, true);
		}
    } while (!tokens.IsTokenOfType(TokenRightBrace, false));

	tokens.Next();

    _variables->AddType(struct_id.c_str(), struct_variables.Variables());

    return true;
}

