#include "PipelineCompiler.h"
#include "PipelineConstructor.h"

#include <boost/assign/list_of.hpp>

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

void Statement::Reset()
{
	_type = StatementUnknown;
	_tokens.clear();
	_iter = _tokens.begin();
}

map<TokenType, wstring> token_map = boost::assign::map_list_of
	(TokenOperatorPointer, L"->")
	(TokenOperatorLink, L"<=>")
	(TokenOperatorDot, L".")
	(TokenOperatorAssign, L"=")
	(TokenComma, L",")
	(TokenSemiColon, L";")
	(TokenLeftBrace, L"{")
	(TokenRightParenthesis, L"}")
	(TokenLeftParenthesis, L"(")
	(TokenRightParenthesis, L")")
	(TokenOperatorMinus, L"-")
	(TokenId, L"identifier")
	(TokenStringLiteral, L"string literal")
	(TokenNumericLiteral, L"numeric literal")
	(TokenKeyword, L"keyword");

/// ����������ָ����token�����Ͳ���type�����׳�����������
void Statement::CheckFor(TokenType type, bool move_next)
{
	if (_iter == _tokens.end())
	{
		throw (CompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"�����������䡣"));
	}

	if ((_iter->type != type) &&
		!(type == TokenId && _iter->type == TokenKeyword && _iter->text == L"self")) // treat keyword 'self' as id
	{
		wstring message = L"Incorrect token type, " + token_map[type] + L" expected.";
		throw (CompileError(*_iter, CompileErrorTokenType, message));
	}

	if (move_next)
	{
        ++_iter;
	}
}

/// ����������ָ����token�Ƿ�ָ�������͡��ú����������������仯��
bool Statement::IsType(TokenType type)
{
	return _iter->type == type;
}

/// ��ͼ��ȡһ��Id���������ƶ�����ȡ����֮����
std::wstring Statement::GetId()
{
	CheckFor(TokenId, false);

	return (_iter++)->text;
}

std::wstring Statement::GetLiteralValue()
{
	if (AtEnd() ||
		(_iter->type != TokenStringLiteral && _iter->type != TokenNumericLiteral &&
		!(_iter->type == TokenKeyword && (_iter->text == L"true" || _iter->text == L"false"))))
	{
		throw CompileError(*_iter, CompileErrorValueExpected, L"Property value expected.");
	}

	return (_iter++)->text;
}

/// ��ͼ��ȡ������/��Ա�����Ի��߶˿ڣ��ԣ��������ƶ�����ȡ����֮����
std::pair<std::wstring, std::wstring> Statement::GetProcessorMember(bool empty_member_allowed)
{
	pair<wstring, wstring> result;
	result.first = GetId();

	if (empty_member_allowed)
	{
		if (IsType(TokenOperatorDot))
		{
			++_iter;
			result.second = GetId();
		}
	}
	else
	{
		CheckFor(TokenOperatorDot, true);
		result.second = GetId();
	}
	return result;
}

/// ��ͼ��ȡ����id���������ƶ�����ȡ����֮����
std::wstring Statement::GetParamId()
{
	wstring param_id;
	bool id_expected = true;
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
	{
		if (!((id_expected && _iter->type == TokenId) || (!id_expected && _iter->type == TokenOperatorDot)))
		{
			throw CompileError(*_iter, CompileErrorParamIdInvalid, L"����Id����ʽ���Ϸ���");
		}
		param_id += (_iter++)->text;
		id_expected = !id_expected;
	}

	return param_id;
}

Yap::Statement::Statement(PipelineConstructor& constructor) :
	_constructor(constructor), 
	_type(StatementUnknown)
{
	Reset();
}

bool Yap::Statement::IsEmpty()
{
	return _tokens.empty();
}

void Yap::Statement::SetType(StatementType type)
{
	_type = type;
}

Yap::StatementType Yap::Statement::GetType() const
{
	return _type;
}

size_t Yap::Statement::GetTokenCount() const
{
	return _tokens.size();
}

void Yap::Statement::AddToken(const Token& token)
{
	_tokens.push_back(token);
}

const Yap::Token& Yap::Statement::GetToken(unsigned int index)
{
	assert(index < _tokens.size()); 
	return _tokens[index];
}

const Yap::Token& Yap::Statement::GetCurrentToken()
{
	return *_iter;
}

const Yap::Token& Yap::Statement::GetLastToken() const
{
	return *(_tokens.end() - 1);
}

bool Yap::Statement::AtEnd() const
{
	return _iter == _tokens.end();
}

std::wstring Yap::Statement::GetVariableId()
{
	wstring variable_id;
	bool id_expected = true;
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
	{
		if ((!id_expected && _iter->type != TokenOperatorDot) || (id_expected && _iter->type != TokenId))
		{
			throw CompileError(*_iter, CompileErrorParamIdInvalid, L"Invalid format for system variable id.");
		}
		variable_id += (_iter++)->text;
		id_expected = !id_expected;
	}
	if (id_expected)
	{
		throw CompileError(*(_iter - 1), CompileErrorParamIdInvalid, L"Invalid format for system variable id.");
	}

	return variable_id;
}

bool Statement::Process()
{
	switch (_type)
	{
	case StatementImport:
		return ProcessImport();
	case StatementAssign:
		return ProcessAssignment();
	case StatementDeclaration:
		return ProcessDeclaration();
	case StatementPortLink:
		return ProcessPortLink();
	case StatementPropertyLink:
		return ProcessPropertyLink();
	default:
		assert(0);
		return false;
	}
}

/**
������ָ��ģ�����ơ�
*/
bool Statement::ProcessImport()
{
	assert(GetType() == StatementImport);
	assert(!IsEmpty());

	if (GetTokenCount() > 2)
	{
		throw CompileError(GetToken(2), CompileErrorSemicolonExpected, L"Semicolon expected.");
	}

	_iter = _tokens.begin();
	++_iter;	// bypass 'import'

	const Token& token = GetCurrentToken();
	if (GetTokenCount() == 1 || token.type != TokenStringLiteral)
	{
		throw CompileError(token, CompileErrorStringExpected,
			L"String literal should be used to specify plugin to import.");
	}

	if (!_constructor.LoadModule(token.text.c_str()))
	{
		wstring output = wstring(L"�޷�����ģ���ļ���") + token.text;
		throw CompileError(token, CompileErrorLoadModule, output);
	}

	return true;
}

/**
������ָ����һ��Ԫ�ء�һ����ʽ�ǣ�
process1.output_port->process2.input_port;
����.output_port��.input_port�ǿ�ѡ�ģ�����ʡ�ԣ���ʹ��ȱʡ��Output��Input��
*/
bool Statement::ProcessPortLink()
{
	_iter = _tokens.begin();
	auto source_processor = GetId();

	wstring source_port(L"Output");
	if (GetCurrentToken().type == TokenOperatorDot)
	{
		++_iter;
		source_port = GetId();
	}

	CheckFor(TokenOperatorPointer, true);

	auto dest_processor = GetId();

	wstring dest_port(L"Input");


	if (!AtEnd() && GetCurrentToken().type == TokenOperatorDot)
	{
		++_iter;
		dest_port = GetId();
	}

	if (source_processor == L"self")
	{
		if (dest_processor == L"self")
		{
			throw CompileError(GetCurrentToken(), CompileErrorSelfLink,
				L"Can not link the output of the pipeline to the input of itself.");
		}
		else
		{
			return _constructor.MapInput(source_port.c_str(), dest_processor.c_str(), dest_port.c_str());
		}
	}
	else if (dest_processor == L"self")
	{
		return _constructor.MapOutput(dest_port.c_str(), source_processor.c_str(), source_port.c_str());
	}
	else
	{
		return _constructor.Link(source_processor.c_str(), source_port.empty() ? L"Output" : source_port.c_str(),
			dest_processor.c_str(), dest_port.empty() ? L"Input" : dest_port.c_str());
	}
}


bool Statement::ProcessDeclaration()
{
	assert(_type == StatementDeclaration);
	assert(GetTokenCount() >= 2);

	_iter = _tokens.begin();

	wstring class_id = GetId();
	wstring instance_id = GetId();

	if (_constructor.InstanceIdExists(instance_id.c_str()))
	{
		throw CompileError(GetToken(1), CompileErrorIdExists,
			wstring(L"Instance id specified for the processor already exists: ") + instance_id);
	}

	if (AtEnd())
	{
		_constructor.CreateProcessor(class_id.c_str(), instance_id.c_str());
	}
	else if (GetCurrentToken().type != TokenLeftParenthesis)
	{
		throw CompileError(GetCurrentToken(), CompileErrorSemicolonExpected,
			L"Semicolon or left parenthesis expected.");
	}
	else
	{
		_constructor.CreateProcessor(class_id.c_str(), instance_id.c_str());
		for (;;)
		{
			++_iter;
			wstring property = GetId();

			if (GetCurrentToken().type == TokenOperatorAssign)
			{
				++_iter;
				wstring value = GetLiteralValue();
				_constructor.SetProperty(instance_id.c_str(), property.c_str(), value.c_str());
			}
			else if (GetCurrentToken().type == TokenOperatorLink)
			{
				++_iter;
				wstring variable_id = GetVariableId();

				_constructor.LinkProperty(instance_id.c_str(), property.c_str(), variable_id.c_str());
			}
			else
			{
				throw CompileError(GetCurrentToken(), CompileErrorPropertyOperatorExpected,
					L"Property operator must be specified, you can use either \'=\' or \'<=>\'.");
			}

			if (AtEnd())
			{
				throw CompileError(GetLastToken(), CompileErrorRightParenthesisExpected, L"Right parenthesis expected.");
			}
			else if (GetCurrentToken().type == TokenRightParenthesis)
			{
				break;
			}
			else if (GetCurrentToken().type != TokenComma)
			{
				throw CompileError(GetCurrentToken(), CompileErrorCommaExpected,
					L"Comma \',\' or right parenthesis \')\' expected.");
			}
		}
	}

	return true;
}

bool Statement::ProcessPropertyLink()
{
	assert(_type == StatementPropertyLink);

	_iter = _tokens.begin();

	wstring processor_instance_id = GetId();
	if (!_constructor.InstanceIdExists(processor_instance_id.c_str()))
	{
		throw CompileError(_tokens[0], CompileErrorProcessorNotFound,
			wstring(L"Processor not found: ") + processor_instance_id);
	}

	CheckFor(TokenOperatorDot, true);
	wstring property = GetId();

	CheckFor(TokenOperatorLink, true);
	wstring variable_id = GetVariableId();

	return _constructor.LinkProperty(processor_instance_id.c_str(), property.c_str(), variable_id.c_str());
}

bool Statement::ProcessAssignment()
{
	assert(_type == StatementAssign);

	_iter = _tokens.begin();

	wstring processor_instance_id = GetId();
	if (!_constructor.InstanceIdExists(processor_instance_id.c_str()))
	{
		throw CompileError(GetToken(0), CompileErrorProcessorNotFound,
			wstring(L"Processor not found: ") + processor_instance_id);
	}

	CheckFor(TokenOperatorDot, true);
	wstring property = GetId();

	CheckFor(TokenOperatorAssign, true);
	wstring value = GetLiteralValue();

	return _constructor.SetProperty(processor_instance_id.c_str(), property.c_str(), value.c_str());
}

void Yap::Statement::DebugOutput(wostream& output)
{
	static map<StatementType, wstring> statment_label = boost::assign::map_list_of
	(StatementImport, L"Import")
		(StatementAssign, L"Assign")
		(StatementDeclaration, L"Decl")
		(StatementPortLink, L"PortLink")
		(StatementPropertyLink, L"PropertyLink");

	output << statment_label[_type] << L"\t\t: ";

	for (auto token : _tokens)
	{
		output << token.text << " ";
	}

	output << "\n";
}

PipelineCompiler::PipelineCompiler(void)
{
	_key_words.insert(L"self");
	_key_words.insert(L"import");
	_key_words.insert(L"true");
	_key_words.insert(L"false");
}

PipelineCompiler::~PipelineCompiler(void)
{
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
	Preprocess(input);

	if (!_constructor)
	{
		_constructor = shared_ptr<PipelineConstructor>(new PipelineConstructor);
	}
	_constructor->Reset(true);

	try
	{
		if (Process())
		{
			return _constructor->GetPipeline();
		}

        return Yap::SmartPtr<CompositeProcessor>();
	}
	catch (CompileError& e)
	{
		wostringstream output;
		output << L"Line " << e.GetToken().line + 1
			<< L" Column " << e.GetToken().column + 1
			<< ": Error code " << e.GetErrorNumber()
			<< " " << e.GetErrorMessage();

		wcerr << output.str();

        return SmartPtr<CompositeProcessor>();
	}
}

bool PipelineCompiler::Preprocess(wistream& script_file)
{
	wstring statement;
	wstring line;
	unsigned line_number = 0;

	_script_lines.clear();
	while (!_matching_check.empty()) _matching_check.pop();

	while (getline(script_file, line, L'\n'))
	{
		_script_lines.push_back(line);

		PreprocessLine(line, line_number);
		++line_number;
	}

	DebugOutputTokens(wcout);

	return true;
}

bool PipelineCompiler::PreprocessLine(std::wstring& line, 
	int line_number)
{
	int pos = 0;

	while ((pos = int(line.find_first_not_of(L" \t\n", pos))) != wstring::npos)
	{
		switch (line[pos])
		{
		case '(':
			_tokens.push_back(Token(line_number, pos, 1, TokenLeftParenthesis));
			_matching_check.push(Token(line_number, pos, 1, TokenLeftParenthesis));
			pos += 1;
			break;
		case ')':
		{
			Token token(line_number, pos, 1, TokenRightParenthesis);
			if (_matching_check.empty())
			{
				throw CompileError(token, CompileErrorNoMatchingLeftParenthesis, L"No matching left parethesis found.");
			}
			else if (_matching_check.top().type == TokenLeftBrace)
			{
				throw CompileError(token, CompileErrorRightBraceExpected, L"Right brace expected.");
			}
			else
			{
				_matching_check.pop();
				pos += 1;
				_tokens.push_back(token);
			}
		}
		break;
		case '{':
			_tokens.push_back(Token(line_number, pos, 1, TokenLeftBrace));
			pos += 1;
			_matching_check.push(Token(line_number, pos, 1, TokenLeftBrace));
			break;
		case '}':
		{
			Token token(line_number, pos, 1, TokenRightBrace);
			if (_matching_check.empty())
			{
				throw CompileError(token, CompileErrorNoMatchingLeftBrace, L"No matching left brace found.");
			}
			else if (_matching_check.top().type == TokenLeftParenthesis)
			{
				throw CompileError(token, CompileErrorRightParenthesisExpected, L"Right parenthesis expected.");
			}
			else
			{
				_matching_check.pop();
				pos += 1;
				_tokens.push_back(token);
			}
		}
		break;
		case ';':
			_tokens.push_back(Token(line_number, pos, 1, TokenSemiColon));
			pos += 1;
			break;
		case '.':
			_tokens.push_back(Token(line_number, pos, 1, TokenOperatorDot));
			pos += 1;
			break;
		case ',':
			_tokens.push_back(Token(line_number, pos, 1, TokenComma));
			pos += 1;
			break;
		case '=':
			_tokens.push_back(Token(line_number, pos, 1, TokenOperatorAssign));
			pos += 1;
			break;
		case '\"':
		{
			int i = 1;
			for (; i + pos < int(line.length()) && line[pos + i] != '\"'; ++i);
			if (i + pos == line.length())
			{
				// ��ͬһ����û���ҵ�ƥ���Ľ�������
				throw CompileError(Token(line_number, pos, i, TokenStringLiteral), CompileErrorNoMatchingQuote,
					L"No matching quote found on the same line. String literals must be defined on one line.");
			}

			// token�в���������
			_tokens.push_back(Token(line_number, pos + 1, i - 1, TokenStringLiteral));
			pos += i + 1;
			break;
		}
		case '/':
		{
			if (pos + 1 < int(line.length()) && line[pos + 1] == '/')
			{
				// ע�Ͳ���
				return true;
			}
			break;
		}
		case '-':
			if (pos + 1 < int(line.length()) && line[pos + 1] == '>')
			{
				_tokens.push_back(Token(line_number, pos, 2, TokenOperatorPointer));
				pos += 2;
			}
			else
			{
				_tokens.push_back(Token(line_number, pos, 1, TokenOperatorMinus));
				++pos;
			}
			break;
		case '<':
			if (pos + 2 < int(line.length()) && line[pos + 1] == '=' && line[pos + 2] == '>')
			{
				_tokens.push_back(Token(line_number, pos, 3, TokenOperatorLink));
				pos += 3;
			}
			else
			{
				throw CompileError(Token(line_number, pos, 1, TokenUnknown), CompileErrorUnrecognizedSymbol, 
					L"Unsupported symbol:\'<\'");
			}
			break;
		default:
		{
			size_t next_separator = -1;
			Token token;
			token.line = line_number;
			token.column = pos;

			if (isdigit(line[pos]))
			{
				token.type = TokenNumericLiteral;
				next_separator = line.find_first_of(L" \t\n\"{}()+-,*/=<>;", pos);
			}
			else if (isalpha(line[pos]))
			{
				token.type = TokenId;
				next_separator = line.find_first_of(L" \t\n\"{}()+-.,*/=<>;", pos);
			}
			else
			{
				throw CompileError(Token(line_number, pos, 1, TokenUnknown), CompileErrorUnrecognizedSymbol, 
					L"Unrecognized symbol.");
			}

			token.length = int(((next_separator == -1) ? line.length() : next_separator) - token.column);

			if (token.type == TokenId)
			{
				auto token_string = line.substr(token.column, token.length);
				if (_key_words.find(token_string) != _key_words.end())
				{
					token.type = TokenKeyword;
				}
			}

			_tokens.push_back(token);
			pos = int(next_separator);

			if (next_separator == -1)
			{
				break;
			}
		}
		}
	}

	return true;
}

bool PipelineCompiler::Process()
{
	Statement statement(*_constructor);

	for (auto token : _tokens)
	{
		token.text = GetTokenString(token);

		switch (token.type)
		{
		case TokenKeyword:
			if (token.text == L"import" && statement.IsEmpty())
			{
				statement.SetType(StatementImport);
			}
			break;
		case TokenOperatorAssign:
			if (statement.GetType() == StatementUnknown)
			{
				statement.SetType(StatementAssign);
			}
			break;
		case TokenOperatorLink:
			if (statement.GetType() == StatementUnknown)
			{
				statement.SetType(StatementPropertyLink);
			}
			break;
		case TokenOperatorPointer:
			statement.SetType(StatementPortLink);
			break;
		case TokenId:
			if (statement.GetTokenCount() == 1 && statement.GetToken(0).type == TokenId)
			{
				statement.SetType(StatementDeclaration);
			}
			break;
		case TokenSemiColon:
			if (!statement.IsEmpty())
			{
				if (statement.GetType() == StatementUnknown)
				{
					throw(CompileError(statement.GetLastToken(), 
						CompileErrorIncompleteStatement, L"Statement not complete."));
				}
#ifdef DEBUG
				DebugOutputStatement(cout, statement);
#endif
				statement.Process();
				statement.Reset();
			}
			break;
		default:
			;
		}

		if (token.type != TokenSemiColon)
		{
			statement.AddToken(token);
		}
	}

	return true;
}




wstring PipelineCompiler::GetTokenString(const Token& token) const
{
	return _script_lines[token.line].substr(token.column, token.length);
}



void PipelineCompiler::DebugOutputTokens(std::wostream& output)
{
	for (auto iter = _tokens.begin(); iter != _tokens.end(); ++iter)
	{
		auto token = _script_lines[iter->line].substr(iter->column, iter->length);
		auto token_item = token_map.find(iter->type);
		if (token_item != token_map.end())
		{
			output << token_item->second.c_str() << "\t: " << token.c_str() << "\n";
		}
		else
		{
			assert(0);
		}
	}
}

void PipelineCompiler::TestTokens()
{
	map<TokenType, wstring> token_map = boost::assign::map_list_of(TokenOperatorPointer, L"->")
		(TokenOperatorLink, L"<=>")
		(TokenOperatorDot, L".")
		(TokenOperatorAssign, L"=")
		(TokenComma, L",")
		(TokenSemiColon, L";")
		(TokenLeftBrace, L"{")
		(TokenRightParenthesis, L"}")
		(TokenLeftParenthesis, L"(")
		(TokenRightParenthesis, L")")
		(TokenOperatorMinus, L"-");

	for (auto iter = _tokens.begin(); iter != _tokens.end(); ++iter)
	{
		auto token = _script_lines[iter->line].substr(iter->column, iter->length);
		auto token_item = token_map.find(iter->type);
		if (token_item != token_map.end())
		{
			assert(token_item->second == token);
		}
		else
		{
			switch (iter->type)
			{
			case TokenId:
				assert(isalpha(token[0]));
				break;
			case TokenStringLiteral:
				assert(_script_lines[iter->line][iter->column - 1] == L'\"' &&
					_script_lines[iter->line][iter->column + iter->length] == L'\"');
				break;
			case TokenNumericLiteral:
				assert(isdigit(token[0]));
				break;
			case TokenKeyword:
				assert(_key_words.find(token) != _key_words.end());
				break;
			default:
				assert(0);
			}
		}
	}
}
