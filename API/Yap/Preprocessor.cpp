#include "Preprocessor.h"

#include <cassert>
#include <iostream>
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

map<TokenType, wstring> token_map = {
	{TokenOperatorPortLink, L"->"},
	{TokenOperatorInOutMapping, L"<=>"},
	{TokenOperatorInMapping, L"<=="},
	{TokenOperatorOutMapping, L"==>"},
	{TokenOperatorDot, L"."},
	{TokenOperatorAssign, L"="},
	{TokenLessThen, L"<"},
	{TokenGreaterThen, L">"},
	{TokenSharp, L"#"},
	{TokenComma, L","},
	{TokenSemiColon, L";"},
	{TokenLeftBrace, L"{"},
	{TokenRightBrace, L"}"},
	{TokenLeftParenthesis, L"("},
	{TokenRightParenthesis, L"), "},
	{TokenOperatorMinus, L"-"},
	{TokenId, L"identifier"},
	{TokenStringLiteral, L"string literal"},
	{TokenNumericLiteral, L"numeric literal"},
	{TokenKeywordBool,    L"bool"},
	{TokenKeywordFloat,	  L"float"},
	{TokenKeywordInt,	  L"int"},
	{TokenKeywordString,  L"string"},
	{TokenKeywordArray,	  L"array"},
	{TokenKeywordStruct,  L"struct"},
	{TokenKeywordImport,  L"import"},
	{TokenKeywordInclude, L"include"},
	{TokenKeywordSelf, L"self"},
	{TokenKeywordTrue, L"true"},
	{TokenKeywordFalse, L"false"},
};

Statement::Guard::Guard(Statement& statement) :
	_statement{statement}
{
	_statement.StartProcessingStatement();
}

Statement::Guard::~Guard()
{
	_statement.FinishProcessingStatement();
}

Statement::Statement(const vector<Token>& tokens) :
	_tokens{tokens},
	_type{StatementUnknown}
{
	_begin = _iter = tokens.cbegin();
}

void Statement::StartProcessingStatement()
{
	_iter = _begin;
}

void Statement::FinishProcessingStatement()
{
	AssertToken(TokenSemiColon, true);
	_begin = _iter;
	_type = StatementUnknown;
}

void Statement::Next()
{
	++_iter;

	if (_iter == _tokens.end())
	{
		throw (CompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"Unexpected end of statement."));
	}
}

/// Check to see whether next token in the statement is of specified type. 
/**
	\remarks This function check to see whether next token in the statement is if specified type.
	If not, a compiler error exception will be thrown.
	\param type
	\param move_next, if \true, extract the next token.
*/
void Statement::AssertToken(TokenType type, bool move_next)
{
	if (_iter == _tokens.end())
	{
		throw (CompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"Unexpected end of statement."));
	}

	if ((_iter->type != type) &&
		!(type == TokenId && _iter->type == TokenKeywordSelf)) // treat keyword 'self' as id
	{
		throw CompileErrorTokenType(*_iter, type);
	}

	if (move_next)
	{
		++_iter;
	}
}

/// Check to see whether next token in the statement is of specified type. 
/**
\remarks This function check to see whether next token in the statement is if specified type.
This function will not move the pointer.
\return true if the next token is of the specified type, false otherwise.
\param type

*/
bool Statement::IsNextTokenOfType(TokenType type)
{
	if (_iter == _tokens.end())
	{
		throw (CompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"Unexpected end of file."));
	}

	return (((_iter + 1)->type == type) ||
		(type == TokenId && (_iter + 1)->type == TokenKeywordSelf)); // treat keyword 'self' as id
}

/// Check to see whether the current token is of the given type.
bool Statement::IsTokenOfType(TokenType type, bool move_next)
{
	auto result = (_iter->type == type);
	if (move_next && result)
	{
		++_iter;
	}

	return result;
}

bool Statement::IsFirstTokenInStatement() const
{
	return _iter == _begin;
}

/// Extract an Id from the statement.
/**
	\remarks Id may be a class id, variable id, processor id, etc. Current token will be consumed.
	\return The Id extracted from the statement. 
*/
std::wstring Statement::GetId()
{
	AssertToken(TokenId, false);

	return (_iter++)->text;
}

wstring Statement::GetStringLiteral()
{
	const Token& token = GetCurrentToken();
	if (AtEnd() || token.type != TokenStringLiteral)
	{
		throw CompileError(token, CompileErrorStringExpected, L"String literals expected.");
	}
	return (_iter++)->text;
}

std::wstring Statement::GetLiteralValue()
{
	if (AtEnd() ||
		(_iter->type != TokenStringLiteral && _iter->type != TokenNumericLiteral &&
		_iter->type != TokenKeywordTrue && _iter->type != TokenKeywordFalse))
	{
		throw CompileError(*_iter, CompileErrorValueExpected, L"Property value expected.");
	}

	return (_iter++)->text;
}

std::pair<std::wstring, std::wstring> Statement::GetProcessorMember(bool empty_member_allowed)
{
	pair<wstring, wstring> result;
	result.first = GetId();

	if (empty_member_allowed)
	{
		if (IsTokenOfType(TokenOperatorDot))
		{
			++_iter;
			result.second = GetId();
		}
	}
	else
	{
		AssertToken(TokenOperatorDot, true);
		result.second = GetId();
	}
	return result;
}

/// 锟斤拷图锟斤拷取锟斤拷锟斤拷id锟斤拷锟斤拷锟斤拷锟斤拷锟狡讹拷锟斤拷锟斤拷取锟斤拷锟斤拷之锟斤拷锟斤拷
std::wstring Statement::GetParamId()
{
	wstring param_id;
	bool id_expected = true;
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
	{
		if (!((id_expected && _iter->type == TokenId) || (!id_expected && _iter->type == TokenOperatorDot)))
		{
			throw CompileError(*_iter, CompileErrorParamIdInvalid, L"锟斤拷锟斤拷Id锟斤拷锟斤拷式锟斤拷锟较凤拷锟斤拷");
		}
		param_id += (_iter++)->text;
		id_expected = !id_expected;
	}

	return param_id;
}

bool Yap::Statement::IsCurrentStatementEmpty()
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

const Yap::Token& Yap::Statement::GetToken(unsigned int index)
{
	assert(_begin + index < _tokens.end());
	return *(_begin + index);
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
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis
			&& _iter->type != TokenSemiColon && _iter->type != TokenSharp && _iter->type != TokenLeftBrace)
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

void Yap::Statement::DebugOutput(wostream& output)
{
	static map<StatementType, wstring> statment_label = {
		{StatementImport, L"Import"},
		{StatementAssign, L"Assign"},
		{StatementDeclaration, L"Declaration"},
		{StatementPortLink, L"PortLink"},
		{StatementPropertyMapping, L"PropertyMapping"},
	};

	output << statment_label[_type] << L"\t\t: ";

	for (auto token : _tokens)
	{
		output << token.text << " ";
	}

	output << "\n";
}

Preprocessor::Preprocessor(PreprocessType type)
{
	switch (type)
	{
		case PreprocessPipeline:
			_supported_tokens = {
				TokenId,
				TokenOperatorPortLink,	// ->
				TokenOperatorInOutMapping,		// <=> 
				TokenOperatorDot,
				TokenOperatorAssign,
				TokenSemiColon,
				TokenComma,
				TokenLeftParenthesis,
				TokenRightParenthesis,

				TokenStringLiteral,
				TokenNumericLiteral,
				TokenKeywordTrue,
				TokenKeywordFalse,

				TokenKeywordBool,
				TokenKeywordFloat,
				TokenKeywordInt,
				TokenKeywordString,
				// TokenKeywordArray,
				// TokenKeywordStruct,
				TokenKeywordImport,
				TokenKeywordInclude,
				TokenKeywordSelf,
			};
			break;
		case PreprocessVariableDefinition:
			_supported_tokens = {
				TokenId,
				TokenOperatorDot,
				TokenOperatorAssign,
				TokenSemiColon,
				TokenComma,
				TokenLeftBrace,
				TokenRightBrace,
				TokenGreaterThen,
				TokenLessThen,
				TokenSharp, // #

				TokenStringLiteral,
				TokenNumericLiteral,

				TokenKeywordBool,
				TokenKeywordFloat,
				TokenKeywordInt,
				TokenKeywordString,
				TokenKeywordArray,
				TokenKeywordStruct,
				TokenKeywordInclude,
				TokenKeywordTrue,
				TokenKeywordFalse,
			};
			break;
		case PreprocessScan:
			break;
			_supported_tokens = {
				TokenId,
				TokenOperatorDot,
				TokenOperatorAssign,
				TokenSemiColon,
				TokenStringLiteral,
				TokenNumericLiteral,

				TokenKeywordTrue,
				TokenKeywordFalse,
			};
		default:
			assert(0 && "Unknown type.");
	};

	_keywords = {
		{L"bool", 	TokenKeywordBool},
		{L"float", TokenKeywordFloat},
		{L"int", TokenKeywordInt},
		{L"string", TokenKeywordString},
		{L"array", TokenKeywordArray},
		{L"struct", TokenKeywordStruct},
		{L"import",TokenKeywordImport},
		{L"include", TokenKeywordInclude},
		{L"self", TokenKeywordSelf},
		{L"true", TokenKeywordTrue},
		{L"false", TokenKeywordFalse}
	};

	_operators = {
		{L"->", TokenOperatorPortLink},
		{L"<=>", TokenOperatorInOutMapping},
		{L".", TokenOperatorDot},
		{L"=", TokenOperatorAssign},
		{L";", TokenSemiColon},
		{L",", TokenComma},
		{L"-", TokenOperatorMinus},
		{L"{", TokenLeftBrace},
		{L"}", TokenRightBrace},
		{L"(", TokenLeftParenthesis},
		{L")", TokenRightParenthesis},
		{L">", TokenGreaterThen},
		{L"<", TokenLessThen},
		{L"#", TokenSharp},
	};
}

Statement Preprocessor::GetStatement()
{
	return Statement(_tokens);
}

bool Preprocessor::Preprocess(wistream& script_file)
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

bool Preprocessor::PreprocessLine(std::wstring& line, 
	int line_number)
{
	int pos = 0;
	static set<wchar_t> braces = {L'(', L')', L'{', L'}'};

	while ((pos = int(line.find_first_not_of(L" \t\n", pos))) != wstring::npos)
	{
		if (braces.find(line[pos]) != braces.end())
		{
			pos = CheckBraceMatching(line[pos], line_number, pos);
			continue;
		}

		size_t next_separator = -1;
		Token token;
		token.line = line_number;
		token.column = pos;

		if (isdigit(line[pos]))
		{
			token.type = TokenNumericLiteral;
			next_separator = line.find_first_of(L" \t\n\"{}()+-,*/=<>#;", pos);
			token.length = int(((next_separator == -1) ? line.length() : next_separator) - token.column);
			token.text = line.substr(token.column, token.length);

			_tokens.push_back(token);
			pos = int(next_separator);

			if (next_separator == -1)
			{
				break;
			}
		}
		else if (isalpha(line[pos]))
		{
			next_separator = line.find_first_of(L" \t\n\"{}()+-.,*/=<>#;", pos);
			token.length = int(((next_separator == -1) ? line.length() : next_separator) - token.column);

			token.text = line.substr(token.column, token.length);
			auto iter = _keywords.find(token.text);

			token.type = (iter != _keywords.end()) ? iter->second : TokenId;

			_tokens.push_back(token);
			pos = int(next_separator);

			if (next_separator == -1)
			{
				break;
			}
		}
		else if (line[pos] == L'\"')
		{
			int i = 1;
			for (; pos + i < int(line.length()) && line[pos + i] != '\"'; ++i);
			if (i + pos == line.length())
			{
				throw CompileError(MakeToken(line_number, pos, i, TokenStringLiteral), CompileErrorNoMatchingQuote,
								   L"No matching quote found on the same line. String literals must be defined on one line.");
			}

			_tokens.push_back(MakeToken(line_number, pos + 1, i - 1, TokenStringLiteral));
			pos += i + 1;
		}
		else if (line[pos] == L'/')
		{
			if (pos + 1 < int(line.length()) && line[pos + 1] == '/')
			{
				// line comment found.
				return true;
			}
			else
			{
				throw CompileError(MakeToken(line_number, pos, 1, TokenUnknown), CompileErrorNoMatchingQuote,
								   L"Unknown token.");
			}
		}
		else
		{
			bool found = false;
			for (auto iter = _operators.rbegin(); iter != _operators.rend(); ++iter)
			{
				auto length = iter->first.length();
				if (line.substr(pos, length) == iter->first)
				{
					_tokens.push_back(MakeToken(line_number, pos, length, iter->second));
					pos += length;
					found = true;
					break;
				}
			}

			if (!found)
			{
				throw CompileError(MakeToken(line_number, pos, 1, TokenUnknown),
								   CompileErrorUnrecognizedSymbol, L"Unrecognized symbol.");
			}
		}
	}

	return true;
}

int Yap::Preprocessor::CheckBraceMatching(wchar_t c, int line_number, int pos)
{
	switch (c)
	{
		case '(':
			_tokens.push_back(MakeToken(line_number, pos, 1, TokenLeftParenthesis));
			_matching_check.push(MakeToken(line_number, pos, 1, TokenLeftParenthesis));
			pos += 1;
			break;
		case ')':
		{
			Token token(line_number, pos, 1, TokenRightParenthesis);
			if (_matching_check.empty())
			{
				throw CompileError(token, CompileErrorNoMatchingLeftParenthesis, L"No matching left parenthesis found.");
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
			_tokens.push_back(MakeToken(line_number, pos, 1, TokenLeftBrace));
			pos += 1;
			_matching_check.push(MakeToken(line_number, pos, 1, TokenLeftBrace));
			break;
		case '}':
		{
			Token token = MakeToken(line_number, pos, 1, TokenRightBrace);
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

	}
	return pos;
}

Token Preprocessor::MakeToken(unsigned int token_line, unsigned token_column, unsigned token_length, TokenType token_type)
{
	Token token(token_line, token_column, token_length, token_type);
	token.text = _script_lines[token.line].substr(token.column, token.length);

	return token;
}

void Preprocessor::DebugOutputTokens(std::wostream& output)
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

void Preprocessor::TestTokens()
{
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
			default:
				assert(0);
			}
		}
	}
}

Yap::CompileErrorTokenType::CompileErrorTokenType(const Token& token, TokenType expected_token) :
	CompileError{token, ErrorCodeTokenType, std::wstring()}
{
	_error_message = token_map[expected_token] + (L" expected.");
}
