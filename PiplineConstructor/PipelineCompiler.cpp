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

using namespace Yap;
using namespace std;

void Statement::Reset()
{
	type = StatementUnknown;
	tokens.clear();
}

void Statement::Restart()
{
	_iter = tokens.begin();
}

/// 如果迭代其指向的token的类型不是type，则抛出编译错误。
void Statement::CheckFor(TokenType type, bool move_next)
{
	assert(_iter != tokens.end());
	if (_iter->type != type)
	{
		if (_iter->type == TokenSemiColon)
		{
			throw (CCompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"不完整的语句。"));
		}

		throw (CCompileError(*_iter, CompileErrorTokenType, L"符号的类型不正确。"));
	}
	if (move_next)
	{
		++_iter;
	}
}

/// 检查迭代器指向的token是否指定的类型。该函数不引起迭代器变化。
bool Statement::IsType(TokenType type)
{
	return _iter->type != type;
}

/// 试图提取一个Id，迭代器移动到提取内容之后。
std::wstring Statement::GetId()
{
	CheckFor(TokenId);

	return (_iter++)->text;
}

void Statement::Next()
{
	assert(_iter != tokens.end());
	++_iter;
}

/// 试图提取处理器/成员（属性或者端口）对，迭代器移动到提取内容之后。
std::pair<std::wstring, std::wstring> Statement::GetProcessorMember(bool empty_member_allowed)
{
	pair<wstring, wstring> result;
	result.first = GetId();

	if (empty_member_allowed)
	{
		if (IsType(TokenOperatorDot))
		{
			Next();
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
std::wstring Statement::GetParamId()
{
	wstring param_id;
	bool id_expected = true;
	while (_iter != tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
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

CCompositeProcessor * CPipelineCompiler::Load(const wchar_t * path)
{
	wifstream script_file;
	script_file.open(path);

	if (!script_file)
	{
		wstring message = wstring(L"Failed to open script file: ") + path;
		// throw CCompileError(Token(), CompileErrorFailedOpenFile, message);
	}

	Preprocess(script_file);
	script_file.close();

	if (!_constructor)
	{
		_constructor = shared_ptr<CPipelineConstructor>(new CPipelineConstructor);
	}
	_constructor->Reset(true);

	try
	{
		if (Process())
		{
			return _constructor->GetPipeline().get();
		}

		return nullptr;
	}
	catch (CCompileError& e)
	{
		wostringstream output;
		output << L"Line " << e.GetToken().line + 1
			<< L" Column " << e.GetToken().column + 1
			<< ": Error code " << e.GetErrorNumber()
			<< " " << e.GetErrorMessage();

		wcerr << output.str();

		return nullptr;
	}
}

bool CPipelineCompiler::Preprocess(wifstream& script_file)
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
	Statement statement;
	statement.Reset();

	for (auto token : _tokens)
	{
		token.text = GetTokenString(token);

		switch (token.type)
		{
		case TokenKeyword:
			if (token.text == L"import" && statement.tokens.empty())
			{
				statement.type = StatementImport;
			}
			statement.tokens.push_back(token);
			break;
		case TokenOperatorAssign:
			if (statement.type == StatementUnknown)
			{
				statement.type = StatementAssign;
			}
			statement.tokens.push_back(token);
			break;
		case TokenOperatorLink:
			if (statement.type == StatementUnknown)
			{
				statement.type = StatementPropertyLink;
			}
			statement.tokens.push_back(token);
			break;
		case TokenOperatorPointer:
			statement.type = StatementPortLink;
			statement.tokens.push_back(token);
			break;
		case TokenId:
			if (statement.tokens.size() == 1 && statement.tokens[0].type == TokenId)
			{
				statement.type = StatementDeclaration;
			}
			statement.tokens.push_back(token);
			break;
		case TokenSemiColon:
			if (!statement.tokens.empty())
			{
				if (statement.type == StatementUnknown)
				{
					throw(CCompileError(statement.tokens[statement.tokens.size() - 1], 
						CompileErrorIncompleteStatement, L"Statement not complete."));
				}
#ifdef DEBUG
				DebugOutputStatement(cout, statement);
#endif
				ProcessStatement(statement);
				statement.Reset();
			}
			break;
		default:
			statement.tokens.push_back(token);
		}
	}

	return true;
}


bool CPipelineCompiler::ProcessStatement(Statement& statement)
{
	switch (statement.type)
	{
	case StatementImport:
		return ProcessImport(statement);
	case StatementAssign:
		return ProcessAssignment(statement);
	case StatementDeclaration:
		return ProcessDeclaration(statement);
	case StatementPortLink:
		return ProcessPortLink(statement);
	case StatementPropertyLink:
		return ProcessPropertyLink(statement);
	default:
		assert(0);
		return false;
	}
}

wstring CPipelineCompiler::GetTokenString(const Token& token) const
{
	return _script_lines[token.line].substr(token.column, token.length);
}

/**
	迭代器指向模块名称。
*/
bool CPipelineCompiler::ProcessImport(Statement& statement)
{
	statement.Restart();
	statement.Next();

	auto iter = statement.tokens.begin();
	assert(iter != statement.tokens.end() && statement.type == StatementImport);

	if (statement.tokens.size() > 2)
	{
		throw CCompileError(*(iter + 2), CompileErrorSemicolonExpected, L"Semicolon expected.");
	}

	++iter;
	if (statement.tokens.size() == 1 || iter->type != TokenStringLiteral)
	{
		throw CCompileError(*iter, CompileErrorStringExpected, 
			L"String literal should be used to specify plugin to import.");
	}

	if (!_constructor->LoadModule(iter->text.c_str()))
	{
		wstring output = wstring(L"无法加载模块文件：") + iter->text;
		throw CCompileError(*iter, CompileErrorLoadModule, output);
	}

	return true;
}

/**
迭代器指向第一个元素。一般形式是：
process1.output_port->process2.input_port;
其中.output_port和.input_port是可选的，如果省略，则使用缺省的Output和Input。
*/
bool CPipelineCompiler::ProcessPortLink(Statement& statement)
{
	assert(_constructor);

	auto iter = statement.tokens.begin();
	auto source_processor = GetTokenString(*iter);
	if (iter->type != TokenId && !(iter->type == TokenKeyword && source_processor == L"self"))
	{
		throw CCompileError(*iter, CompileErrorProcessorExpected, L"Processor instance id or \'self\' expected.");
	}

	wstring source_port(L"Output");
	++iter;
	if (iter->type == TokenOperatorDot)
	{
		++iter;

		if (iter->type != TokenId)
		{
			throw CCompileError(*iter, CompileErrorPortExpected, L"Port name expected.");
		}
		source_port = GetTokenString(*iter);
		++iter;
	}

	if (iter->type != TokenOperatorPointer)
	{
		throw CCompileError(*iter, CompileErrorLinkOperatorExpected, L"Operator \'->\' expected.)");
	}

	++iter;
	auto dest_processor = GetTokenString(*iter);
	if (iter->type != TokenId && !(iter->type == TokenKeyword && dest_processor == L"self"))
	{
		throw CCompileError(*iter, CompileErrorProcessorExpected, L"Processor instance id or \'self\' expected.");
	}

	wstring dest_port(L"Input");
	++iter;
	if (iter != statement.tokens.end() && iter->type == TokenOperatorDot)
	{
		++iter;

		if (iter->type != TokenId)
		{
			throw CCompileError(*iter, CompileErrorPortExpected, L"Port name expected.");
		}
		dest_port = GetTokenString(*iter);
		++iter;
	}

	if (source_processor == L"self")
	{
		if (dest_processor == L"self")
		{
			throw CCompileError(*iter, CompileErrorSelfLink, 
				L"Can not link the output of the pipeline to the input of itself.");
		}
		else
		{
			return _constructor->AssignPipelineInPort(source_port.c_str(), dest_processor.c_str(), dest_port.c_str());
		}
	}
	else if (dest_processor == L"self")
	{
		return _constructor->AssignPipelineOutPort(dest_port.c_str(), source_processor.c_str(), source_port.c_str());
	}
	else
	{
		return _constructor->Link(source_processor.c_str(), source_port.empty() ? L"Output" : source_port.c_str(),
			dest_processor.c_str(), dest_port.empty() ? L"Input" : dest_port.c_str());
	}
}

bool CPipelineCompiler::ProcessDeclaration(Statement& statement)
{
	assert(statement.tokens.size() >= 2);
	assert(statement.type == StatementDeclaration);

	auto iter = statement.tokens.begin();

	wstring class_id = GetTokenString(*iter++);
	wstring instance_id = GetTokenString(*iter++);

	if (_constructor->InstanceIdExists(instance_id.c_str()))
	{
		throw CCompileError(*iter, CompileErrorIdExists, 
			wstring(L"Instance id specified for the processor already exists: ") + instance_id);
	}

	if (iter == statement.tokens.end())
	{
		_constructor->CreateProcessor(class_id.c_str(), instance_id.c_str());
	}
	else if (iter->type != TokenLeftParenthesis)
	{
		throw CCompileError(*iter, CompileErrorSemicolonExpected, L"Semicolon or left parenthesis expected.");
	}
	else
	{
		_constructor->CreateProcessor(class_id.c_str(), instance_id.c_str());
		for (;;)
		{
			++iter;
			if (iter == statement.tokens.end() || iter->type != TokenId)
			{
				throw CCompileError(*iter, CompileErrorIdExpected, L"Property id expected.");
			}
			wstring property = GetTokenString(*iter);

			++iter;
			if (iter->type == TokenOperatorAssign)
			{
				++iter;
				if (iter == statement.tokens.end() ||
					(iter->type != TokenStringLiteral && iter->type != TokenNumericLiteral &&
						!(iter->type == TokenKeyword && (GetTokenString(*iter) == L"true" || 
							GetTokenString(*iter) == L"false"))))
				{
					throw CCompileError(*iter, CompileErrorValueExpected, L"Property value expected.");
				}
				wstring value = GetTokenString(*iter);
				_constructor->SetProperty(instance_id.c_str(), property.c_str(), value.c_str());
				++iter;
			}
			else if (iter->type == TokenOperatorLink)
			{
				++iter;
				wstring param_id;
				bool id_expected = true;
				while (iter != statement.tokens.end() && iter->type != TokenComma && iter->type != TokenRightParenthesis)
				{
					if (!((id_expected && iter->type == TokenId) || (!id_expected && iter->type == TokenOperatorDot)))
					{
						throw CCompileError(*iter, CompileErrorParamIdInvalid, L"Invalid format for system variable id.");
					}
					param_id += GetTokenString(*iter++);
					id_expected = !id_expected;
				}
				_constructor->LinkProperty(instance_id.c_str(), property.c_str(), param_id.c_str());
			}
			else
			{
				throw CCompileError(*iter, CompileErrorPropertyOperatorExpected, 
					L"Property operator must be specified, you can use either \'=\' or \'<=>\'.");
			}

			if (iter == statement.tokens.end())
			{
				throw CCompileError(*--iter, CompileErrorRightParenthesisExpected, L"Right parenthesis expected.");
			}
			else if (iter->type == TokenRightParenthesis)
			{
				break;
			}
			else if (iter->type != TokenComma)
			{
				throw CCompileError(*iter, CompileErrorCommaExpected, L"Comma \',\' or right parenthesis \')\' expected.");
			}
		}
	}

	return true;
}

bool CPipelineCompiler::ProcessPropertyLink(Statement& statement)
{
	return true;
}

bool CPipelineCompiler::ProcessAssignment(Statement& statement)
{
	return true;
}

void CPipelineCompiler::DebugOutputTokens(std::wostream& output)
{
	map<TokenType, wstring> token_map = boost::assign::map_list_of(TokenOperatorPointer, L"->")
		(TokenOperatorLink, L"<=>")
		(TokenOperatorDot, L".")
		(TokenOperatorAssign, L"=")
		(TokenComma, L",")
		(TokenSemiColon, L";")
		(TokenLeftBrace, L"{")
		(TokenRightBrace, L"}")
		(TokenLeftParenthesis, L"(")
		(TokenRightParenthesis, L")")
		(TokenOperatorMinus, L"-")
		(TokenId, L"id")
		(TokenNumericLiteral, L"num")
		(TokenStringLiteral, L"str")
		(TokenKeyword, L"key");

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

void CPipelineCompiler::DebugOutputStatement(std::wostream& output,
	const Statement& statement)
{
	static map<StatementType, wstring> statment_label = boost::assign::map_list_of
		(StatementImport, L"Import")
		(StatementAssign, L"Assign")
		(StatementDeclaration, L"Decl")
		(StatementPortLink, L"PortLink")
		(StatementPropertyLink, L"PropertyLink");

	output << statment_label[statement.type] << L"\t\t: ";

	for (auto iter = statement.tokens.begin(); iter != statement.tokens.end(); ++iter)
	{
		output << iter->text << " ";
	}

	output << "\n";
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
