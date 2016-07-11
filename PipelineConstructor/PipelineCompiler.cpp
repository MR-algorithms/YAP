#include "stdafx.h"
#include "PipelineCompiler.h"
#include <cassert>
#include <boost/assign/list_of.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../Utilities/macros.h"
#include "PipelineConstructor.h"
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

void CStatement::Reset()
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

/// 如果迭代其指向的token的类型不是type，则抛出编译错误。
void CStatement::CheckFor(TokenType type, bool move_next)
{
	if (_iter == _tokens.end())
	{
		throw (CCompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"不完整的语句。"));
	}

	if ((_iter->type != type) &&
		!(type == TokenId && _iter->type == TokenKeyword && _iter->text == L"self")) // treat keyword 'self' as id
	{
		wstring message = L"Incorrect token type, " + token_map[type] + L" expected.";
		throw (CCompileError(*_iter, CompileErrorTokenType, message));
	}

	if (move_next)
	{
		++_iter;
	}
}

/// 检查迭代器指向的token是否指定的类型。该函数不引起迭代器变化。
bool CStatement::IsType(TokenType type)
{
	return _iter->type == type;
}

/// 试图提取一个Id，迭代器移动到提取内容之后。
std::wstring CStatement::GetId()
{
	CheckFor(TokenId, false);

	return (_iter++)->text;
}

std::wstring CStatement::GetLiteralValue()
{
	if (AtEnd() ||
		(_iter->type != TokenStringLiteral && _iter->type != TokenNumericLiteral &&
		!(_iter->type == TokenKeyword && (_iter->text == L"true" || _iter->text == L"false"))))
	{
		throw CCompileError(*_iter, CompileErrorValueExpected, L"Property value expected.");
	}

	return _iter->text;
}

/// 试图提取处理器/成员（属性或者端口）对，迭代器移动到提取内容之后。
std::pair<std::wstring, std::wstring> CStatement::GetProcessorMember(bool empty_member_allowed)
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

/// 试图提取参数id。迭代器移动到提取内容之后。
std::wstring CStatement::GetParamId()
{
	wstring param_id;
	bool id_expected = true;
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
	{
		if (!((id_expected && _iter->type == TokenId) || (!id_expected && _iter->type == TokenOperatorDot)))
		{
			throw CCompileError(*_iter, CompileErrorParamIdInvalid, L"参数Id的形式不合法。");
		}
		param_id += (_iter++)->text;
		id_expected = !id_expected;
	}

	return param_id;
}

Yap::CStatement::CStatement(CPipelineConstructor& constructor) :
	_constructor(constructor), 
	_type(StatementUnknown)
{
	Reset();
}

bool Yap::CStatement::IsEmpty()
{
	return _tokens.empty();
}

void Yap::CStatement::SetType(StatementType type)
{
	_type = type;
}

Yap::StatementType Yap::CStatement::GetType() const
{
	return _type;
}

size_t Yap::CStatement::GetTokenCount() const
{
	return _tokens.size();
}

void Yap::CStatement::AddToken(const Token& token)
{
	_tokens.push_back(token);
}

const Yap::Token& Yap::CStatement::GetToken(unsigned int index)
{
	assert(index < _tokens.size()); 
	return _tokens[index];
}

const Yap::Token& Yap::CStatement::GetCurrentToken()
{
	return *_iter;
}

const Yap::Token& Yap::CStatement::GetLastToken() const
{
	return *(_tokens.end() - 1);
}

bool Yap::CStatement::AtEnd() const
{
	return _iter == _tokens.end();
}

std::wstring Yap::CStatement::GetVariableId()
{
	wstring variable_id;
	bool id_expected = true;
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
	{
		if ((!id_expected && _iter->type != TokenOperatorDot) || (id_expected && _iter->type != TokenId))
		{
			throw CCompileError(*_iter, CompileErrorParamIdInvalid, L"Invalid format for system variable id.");
		}
		variable_id += (_iter++)->text;
		id_expected = !id_expected;
	}
	if (id_expected)
	{
		throw CCompileError(*(_iter - 1), CompileErrorParamIdInvalid, L"Invalid format for system variable id.");
	}

	return variable_id;
}

bool CStatement::Process()
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
迭代器指向模块名称。
*/
bool CStatement::ProcessImport()
{
	assert(GetType() == StatementImport);
	assert(!IsEmpty());

	if (GetTokenCount() > 2)
	{
		throw CCompileError(GetToken(2), CompileErrorSemicolonExpected, L"Semicolon expected.");
	}

	_iter = _tokens.begin();
	++_iter;	// bypass 'import'

	const Token& token = GetCurrentToken();
	if (GetTokenCount() == 1 || token.type != TokenStringLiteral)
	{
		throw CCompileError(token, CompileErrorStringExpected,
			L"String literal should be used to specify plugin to import.");
	}

	if (!_constructor.LoadModule(token.text.c_str()))
	{
		wstring output = wstring(L"无法加载模块文件：") + token.text;
		throw CCompileError(token, CompileErrorLoadModule, output);
	}

	return true;
}

/**
迭代器指向第一个元素。一般形式是：
process1.output_port->process2.input_port;
其中.output_port和.input_port是可选的，如果省略，则使用缺省的Output和Input。
*/
bool CStatement::ProcessPortLink()
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

	if (GetCurrentToken().type == TokenOperatorDot)
	{
		++_iter;
		dest_port = GetId();
	}

	if (source_processor == L"self")
	{
		if (dest_processor == L"self")
		{
			throw CCompileError(GetCurrentToken(), CompileErrorSelfLink,
				L"Can not link the output of the pipeline to the input of itself.");
		}
		else
		{
			return _constructor.AssignPipelineInPort(source_port.c_str(), dest_processor.c_str(), dest_port.c_str());
		}
	}
	else if (dest_processor == L"self")
	{
		return _constructor.AssignPipelineOutPort(dest_port.c_str(), source_processor.c_str(), source_port.c_str());
	}
	else
	{
		return _constructor.Link(source_processor.c_str(), source_port.empty() ? L"Output" : source_port.c_str(),
			dest_processor.c_str(), dest_port.empty() ? L"Input" : dest_port.c_str());
	}
}


bool CStatement::ProcessDeclaration()
{
	assert(_type == StatementDeclaration);
	assert(GetTokenCount() >= 2);

	_iter = _tokens.begin();

	wstring class_id = GetId();
	wstring instance_id = GetId();

	if (_constructor.InstanceIdExists(instance_id.c_str()))
	{
		throw CCompileError(GetToken(1), CompileErrorIdExists,
			wstring(L"Instance id specified for the processor already exists: ") + instance_id);
	}

	if (AtEnd())
	{
		_constructor.CreateProcessor(class_id.c_str(), instance_id.c_str());
	}
	else if (GetCurrentToken().type != TokenLeftParenthesis)
	{
		throw CCompileError(GetCurrentToken(), CompileErrorSemicolonExpected,
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
				throw CCompileError(GetCurrentToken(), CompileErrorPropertyOperatorExpected,
					L"Property operator must be specified, you can use either \'=\' or \'<=>\'.");
			}

			if (AtEnd())
			{
				throw CCompileError(GetLastToken(), CompileErrorRightParenthesisExpected, L"Right parenthesis expected.");
			}
			else if (GetCurrentToken().type == TokenRightParenthesis)
			{
				break;
			}
			else if (GetCurrentToken().type != TokenComma)
			{
				throw CCompileError(GetCurrentToken(), CompileErrorCommaExpected,
					L"Comma \',\' or right parenthesis \')\' expected.");
			}
		}
	}

	return true;
}

bool CStatement::ProcessPropertyLink()
{
	assert(_type == StatementPropertyLink);

	_iter = _tokens.begin();

	wstring processor_instance_id = GetId();
	if (!_constructor.InstanceIdExists(processor_instance_id.c_str()))
	{
		throw CCompileError(_tokens[0], CompileErrorProcessorNotFound,
			wstring(L"Processor not found: ") + processor_instance_id);
	}

	CheckFor(TokenOperatorDot, true);
	wstring property = GetId();

	CheckFor(TokenOperatorLink, true);
	wstring variable_id = GetVariableId();

	return _constructor.LinkProperty(processor_instance_id.c_str(), property.c_str(), variable_id.c_str());
}

bool CStatement::ProcessAssignment()
{
	assert(_type == StatementAssign);

	_iter = _tokens.begin();

	wstring processor_instance_id = GetId();
	if (!_constructor.InstanceIdExists(processor_instance_id.c_str()))
	{
		throw CCompileError(GetToken(0), CompileErrorProcessorNotFound,
			wstring(L"Processor not found: ") + processor_instance_id);
	}

	CheckFor(TokenOperatorDot, true);
	wstring property = GetId();

	CheckFor(TokenOperatorAssign, true);
	wstring value = GetLiteralValue();

	return _constructor.SetProperty(processor_instance_id.c_str(), property.c_str(), value.c_str());
}

void Yap::CStatement::DebugOutput(wostream& output)
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

CPipelineCompiler::CPipelineCompiler(void)
{
	_key_words.insert(L"self");
	_key_words.insert(L"import");
	_key_words.insert(L"true");
	_key_words.insert(L"false");
}

CPipelineCompiler::~CPipelineCompiler(void)
{
}


std::shared_ptr<CCompositeProcessor> Yap::CPipelineCompiler::Compile(const wchar_t * text)
{
	wistringstream input;
	input.str(text);

	return DoCompile(input);
}

shared_ptr<CCompositeProcessor> CPipelineCompiler::CompileFile(const wchar_t * path)
{
	wifstream script_file;
	script_file.open(path);

	if (!script_file)
	{
		wstring message = wstring(L"Failed to open script file: ") + path;
		// throw CCompileError(Token(), CompileErrorFailedOpenFile, message);
		return shared_ptr<CCompositeProcessor>();
	}

	return DoCompile(script_file);
}

std::shared_ptr<CCompositeProcessor> CPipelineCompiler::DoCompile(std::wistream& input)
{
	Preprocess(input);

	if (!_constructor)
	{
		_constructor = shared_ptr<CPipelineConstructor>(new CPipelineConstructor);
	}
	_constructor->Reset(true);

	try
	{
		if (Process())
		{
			return _constructor->GetPipeline();
		}

		return std::shared_ptr<CCompositeProcessor>();
	}
	catch (CCompileError& e)
	{
		wostringstream output;
		output << L"Line " << e.GetToken().line + 1
			<< L" Column " << e.GetToken().column + 1
			<< ": Error code " << e.GetErrorNumber()
			<< " " << e.GetErrorMessage();

		wcerr << output.str();

		return std::shared_ptr<CCompositeProcessor>();
	}
}

bool CPipelineCompiler::Preprocess(wistream& script_file)
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

bool CPipelineCompiler::PreprocessLine(std::wstring& line, 
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
				throw CCompileError(token, CompileErrorNoMatchingLeftParenthesis, L"No matching left parethesis found.");
			}
			else if (_matching_check.top().type == TokenLeftBrace)
			{
				throw CCompileError(token, CompileErrorRightBraceExpected, L"Right brace expected.");
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
				throw CCompileError(token, CompileErrorNoMatchingLeftBrace, L"No matching left brace found.");
			}
			else if (_matching_check.top().type == TokenLeftParenthesis)
			{
				throw CCompileError(token, CompileErrorRightParenthesisExpected, L"Right parenthesis expected.");
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
				// 在同一行上没有找到匹配的结束引号
				throw CCompileError(Token(line_number, pos, i, TokenStringLiteral), CompileErrorNoMatchingQuote,
					L"No matching quote found on the same line. String literals must be defined on one line.");
			}

			// token中不包含引号
			_tokens.push_back(Token(line_number, pos + 1, i - 1, TokenStringLiteral));
			pos += i + 1;
			break;
		}
		case '/':
		{
			if (pos + 1 < int(line.length()) && line[pos + 1] == '/')
			{
				// 注释部分
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
				throw CCompileError(Token(line_number, pos, 1, TokenUnknown), CompileErrorUnrecognizedSymbol, 
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
				throw CCompileError(Token(line_number, pos, 1, TokenUnknown), CompileErrorUnrecognizedSymbol, 
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

bool CPipelineCompiler::Process()
{
	CStatement statement(*_constructor);

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
					throw(CCompileError(statement.GetLastToken(), 
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




wstring CPipelineCompiler::GetTokenString(const Token& token) const
{
	return _script_lines[token.line].substr(token.column, token.length);
}



void CPipelineCompiler::DebugOutputTokens(std::wostream& output)
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

void CPipelineCompiler::TestTokens()
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
