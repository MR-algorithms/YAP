#include "Preprocessor.h"

#include <cassert>
#include <iostream>
#include <map>
#include <ctype.h>
#include <utility>

using namespace Yap;
using namespace std;

map <TokenType, pair<wstring, TokenCategory>> token_info{
	{ TokenOperatorPortLink, {L"->", TokenCategoryOperator }},
	{ TokenOperatorInOutMapping, {L"<=>", TokenCategoryOperator}},
	{ TokenOperatorInMapping, {L"<==", TokenCategoryOperator}},
	{ TokenOperatorOutMapping, {L"==>", TokenCategoryOperator }},
	{ TokenOperatorDot, {L".", TokenCategoryOperator }},
	{ TokenOperatorAssign, {L"=", TokenCategoryOperator }},
	{ TokenLessThan, {L"<", TokenCategoryOperator}},
	{ TokenGreaterThan, {L">", TokenCategoryOperator }},
	{ TokenSharp, {L"#", TokenCategorySpecial}},
	{ TokenComma, {L",", TokenCategorySeparator}},
	{ TokenSemiColon, {L";", TokenCategorySeparator}},
	{ TokenDoubleColon, {L"::", TokenCategoryOperator}},
	{ TokenColon, {L":", TokenCategoryOperator}},
	{ TokenLeftBrace, {L"{", TokenCategorySeparator}},
	{ TokenRightBrace, {L"}", TokenCategorySeparator}},
	{ TokenLeftParenthesis, {L"(", TokenCategorySeparator}},
	{ TokenRightParenthesis, {L"), ", TokenCategorySeparator}},
	{ TokenLeftSquareBracket, {L"[", TokenCategorySeparator}},
	{ TokenRightSquareBracket, {L"]", TokenCategorySeparator}},
	{ TokenOperatorMinus, {L"-", TokenCategoryOperator}},
	{ TokenId, {L"identifier", TokenCategoryId}},
	{ TokenStringLiteral, {L"string literal", TokenCategoryStringLiteral}},
	{ TokenNumericLiteral, {L"numeric literal", TokenCategoryNumericalLiteral}},
	{ TokenKeywordBool,    {L"bool", TokenCategoryKeyword}},
	{ TokenKeywordFloat,	  {L"float", TokenCategoryKeyword }},
	{ TokenKeywordInt,	  {L"int", TokenCategoryKeyword }},
	{ TokenKeywordString,  {L"string", TokenCategoryKeyword }},
	{ TokenKeywordArray,	  {L"array", TokenCategoryKeyword }},
	{ TokenKeywordEnum, {L"enum", TokenCategoryKeyword}},
	{ TokenKeywordStruct,  {L"struct", TokenCategoryKeyword }},
	{ TokenKeywordImport,  {L"import", TokenCategoryKeyword }},
	{ TokenKeywordInclude, {L"include", TokenCategoryKeyword }},
	{ TokenKeywordSelf, {L"self", TokenCategoryKeyword }},
	{ TokenKeywordTrue, {L"true", TokenCategoryKeyword }},
	{ TokenKeywordFalse, {L"false", TokenCategoryKeyword }},
	{ TokenKeywordNamespace, {L"namespace", TokenCategoryKeyword }},
	{ TokenKeywordUsing, {L"using", TokenCategoryKeyword }},
};

Tokens::Guard::Guard(Tokens& statement, bool end_with_semicolon) :
	_statement{statement},
	_end_with_semicolon{end_with_semicolon}
{
	_statement.StartProcessingStatement();
}

Tokens::Guard::~Guard()
{
	_statement.FinishProcessingStatement(_end_with_semicolon);
}

Tokens::Tokens(vector<Token>& tokens) :
	_tokens{tokens},
	_type{StatementUnknown}
{
	_begin = _iter = _tokens.cbegin();
}

void Tokens::StartProcessingStatement()
{
	_iter = _begin;
}

void Tokens::FinishProcessingStatement(bool check_semicolon)
{
	if (check_semicolon)
	{
		AssertToken(TokenSemiColon, true);
	}
	_begin = _iter;
	_type = StatementUnknown;
}

void Tokens::Next()
{
	++_iter;

	if (_iter == _tokens.end())
	{
		throw (CompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"Unexpected end of statement."));
	}
}

/**
	\brief Check to see whether next token in the statement is of specified type. 
	\remarks This function check to see whether next token in the statement is if specified type.
	If not, a compiler error exception will be thrown.
	\param type
	\param move_next, if \true, extract the next token.
*/
void Tokens::AssertToken(TokenType type, bool move_next)
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
bool Tokens::IsNextTokenOfType(TokenType type)
{
	if (_iter == _tokens.end())
	{
		throw (CompileError(*_iter, CompileErrorUnexpectedEndOfStatement, L"Unexpected end of file."));
	}

	return (((_iter + 1)->type == type) ||
		(type == TokenId && (_iter + 1)->type == TokenKeywordSelf)); // treat keyword 'self' as id
}

/// Check to see whether the current token is of the given type.
bool Tokens::IsTokenOfType(TokenType type, bool move_next)
{
	if (AtEnd())
		return false;

	auto result = (_iter->type == type);
	if (move_next && result)
	{
		++_iter;
	}

	return result;
}

bool Tokens::IsFirstTokenInStatement() const
{
	return _iter == _begin;
}

/**
	\brief Extract an Id from the statement.
	\remarks Id may be a class id, variable id, processor id, etc. Current token will be consumed.
	\return The Id extracted from the statement. 
*/
std::wstring Tokens::GetId()
{
	AssertToken(TokenId, false);

	return (_iter++)->text;
}

wstring Tokens::GetStringLiteral()
{
	const Token& token = GetCurrentToken();
	if (AtEnd() || token.type != TokenStringLiteral)
	{
		throw CompileError(token, CompileErrorStringExpected, L"String literals expected.");
	}
	return (_iter++)->text;
}

std::wstring Tokens::GetLiteralValue()
{
	if (!AtEnd())
	{
		if (_iter->type == TokenOperatorPlus || _iter->type == TokenOperatorMinus)
		{
			wstring result{ (_iter++)->text };
			return result + GetLiteralValue();
		}
		else if (_iter->type == TokenStringLiteral ||
			_iter->type == TokenNumericLiteral ||
			_iter->type == TokenKeywordTrue ||
			_iter->type == TokenKeywordFalse)
		{
			return (_iter++)->text;
		}
	}

	throw CompileError(*_iter, CompileErrorValueExpected, L"Property value expected.");
}

std::pair<std::wstring, std::wstring> Tokens::GetProcessorMember(bool empty_member_allowed)
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

/**
	\brief Get a variable id from the statement.

	A variable may be in the form "id|.id|0..n", or "namespace::id|.id|0..n" if namespace is supported;

	An id may include array indexing: "id[index];
*/
std::wstring Tokens::GetParamId()
{
	wstring param_id;
	bool id_expected = true;
	while (_iter != _tokens.end() && _iter->type != TokenComma && _iter->type != TokenRightParenthesis)
	{
		if (!((id_expected && _iter->type == TokenId) || (!id_expected && _iter->type == TokenOperatorDot)))
		{
			throw CompileError(*_iter, CompileErrorParamIdInvalid, L"Invalid form of variable id.");
		}
		param_id += (_iter++)->text;
		id_expected = !id_expected;
	}

	return param_id;
}

/** Try to extract namespace qualifier from the statement and move to next token.
    The final double colon is extracted from the statement but not included in the 
	return value.
*/
std::wstring Yap::Tokens::GetNamespaceQualifier()
{
	wstring result;
	while (_iter != _tokens.end())
	{
		if (_iter->type != TokenId)
		{
			throw CompileError(*_iter, CompileErrorParamIdInvalid, L"Invalid form of variable id.");
		}

		if (_iter + 1 != _tokens.end() && (_iter + 1)->type == TokenDoubleColon)
		{
			// if the id is not followed by ::, the id will not be extracted to the result.
			result += _iter->text;
			_iter += 2;
		}
		else
		{
			return result;
		}
	}

	return result;
}

bool Yap::Tokens::IsEmpty()
{
	return _tokens.empty();
}

void Yap::Tokens::SetType(StatementType type)
{
	_type = type;
}

Yap::StatementType Yap::Tokens::GetType() const
{
	return _type;
}

const Yap::Token& Yap::Tokens::GetToken(unsigned int index)
{
	assert(_begin + index < _tokens.end());
	return *(_begin + index);
}

const Yap::Token& Yap::Tokens::GetCurrentToken()
{
	return *_iter;
}

const Yap::Token& Yap::Tokens::GetLastToken() const
{
	return *(_tokens.end() - 1);
}

bool Yap::Tokens::AtEnd() const
{
	return _iter == _tokens.end();
}

std::wstring Yap::Tokens::GetVariableIdOld()
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

std::wstring Yap::Tokens::GetIdWithIndexing()
{
	wstring result;
	if (_iter == _tokens.end() || _iter->type != TokenId)
	{
		throw CompileError(*_iter, CompileErrorParamIdInvalid, L"Invalid format for system variable id.");
	}
	result += (_iter++)->text;

	while (!AtEnd() && IsTokenOfType(TokenLeftSquareBracket))
	{
		result += L"[";
		++_iter;
		result += GetLiteralValue();
		AssertToken(TokenRightSquareBracket, true);
		result += L"]";
	}

	return result;
}

std::wstring Yap::Tokens::GetVariableId()
{
	wstring variable_id = GetNamespaceQualifier();
	if (!variable_id.empty())
	{
		variable_id += L"::";
	}

	while(!AtEnd())
	{
		variable_id += GetIdWithIndexing();
		if (IsTokenOfType(TokenOperatorDot, true))
		{
			variable_id += L'.';
		}
		else
		{
			return variable_id;
		}
	}

	return variable_id;
}

/**
	\brief Extract a qualified namespace id from tokens.

	outer:: ... ::inner 
*/
std::wstring Yap::Tokens::GetNamespaceId()
{
	wstring namespace_id;
	while (!AtEnd())
	{
		namespace_id += GetId();
		if (IsTokenOfType(TokenDoubleColon, true))
		{
			namespace_id += L"::";
		}
		else
		{
			return namespace_id;
		}
	} 

	throw CompileError(*_iter, CompileErrorIdExpected, L"Namespace id expected after \"::\"");
}

void Yap::Tokens::DebugOutput(wostream& output)
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
				TokenOperatorPortLink,			// ->
				TokenOperatorInOutMapping,		// <=> 
				TokenOperatorInMapping,			// <==
				TokenOperatorOutMapping,		// ==>
				TokenOperatorDot,
				TokenOperatorAssign,
				TokenSemiColon,
				TokenDoubleColon,
				TokenComma,
				TokenLeftParenthesis,
				TokenRightParenthesis,
				TokenLeftSquareBracket,
				TokenRightSquareBracket,

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
				TokenGreaterThan,
				TokenLessThan,
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
				TokenKeywordNamespace,
				TokenKeywordUsing,
			};
			break;
		case PreprocessScan:
			_supported_tokens = {
				TokenId,
				TokenOperatorDot,
				TokenOperatorAssign,
				TokenSemiColon,
				TokenColon,
				TokenDoubleColon,
				TokenStringLiteral,
				TokenNumericLiteral,
				TokenLeftBrace,
				TokenRightBrace,

				TokenKeywordTrue,
				TokenKeywordFalse,
				TokenKeywordUsing,
				TokenKeywordNamespace,
			};
			break;
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
		{L"false", TokenKeywordFalse},
		{L"namespace", TokenKeywordNamespace},
		{L"using", TokenKeywordUsing},
	};

	_operators = {
		{L"->", TokenOperatorPortLink},
		{L"<=>", TokenOperatorInOutMapping},
		{L"<==", TokenOperatorInMapping},
		{L"==>", TokenOperatorOutMapping},
		{L".", TokenOperatorDot},
		{L"=", TokenOperatorAssign},
		{L";", TokenSemiColon},
		{L":", TokenColon},
		{L"::", TokenDoubleColon},
		{L",", TokenComma},
		{L"-", TokenOperatorMinus},
		{L"{", TokenLeftBrace},
		{L"}", TokenRightBrace},
		{L"(", TokenLeftParenthesis},
		{L")", TokenRightParenthesis},
		{L"[", TokenLeftSquareBracket},
		{L"]", TokenRightSquareBracket},
		{L">", TokenGreaterThan},
		{L"<", TokenLessThan},
		{L"#", TokenSharp},
	};
}

Tokens Preprocessor::GetTokens()
{
	return Tokens(_tokens);
}

bool Preprocessor::Preprocess(const wchar_t * const text)
{
	wistringstream input;
	input.str(text);

	return Preprocess(input);
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

	return true;
}

bool Preprocessor::PreprocessLine(std::wstring& line, 
	int line_number)
{
	int pos = 0;
	static set<wchar_t> braces = {L'(', L')', L'{', L'}', L'[', L']'};

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
			// YG: should be replaced with reg_ex
			token.type = TokenNumericLiteral;
			next_separator = line.find_first_of(L" \t\n\"{}()+-,*/=<>#;[]:", pos);
			token.length = int(((next_separator == -1) ? line.length() : next_separator) - token.column);
			token.text = line.substr(token.column, token.length);

			_tokens.push_back(token);
			pos = int(next_separator);

			if (next_separator == -1)
			{
				break;
			}
		}
		else if (isalpha(line[pos]) || line[pos] == L'_')
		{
			next_separator = line.find_first_of(L" \t\n\"{}()+-,*/=<>#;[]:.", pos);
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
	static map<wchar_t, TokenType> token_type_from_char {
		{L'(', TokenLeftParenthesis},
		{L')', TokenRightParenthesis},
		{L'[', TokenLeftSquareBracket},
		{L']', TokenRightSquareBracket},
		{L'{', TokenLeftBrace},
		{L'}', TokenRightBrace} };

	static map<wchar_t, wchar_t> bracket_matching  {
		{L'(', L')' }, {L'[', L']' }, {L'{', L'}' }, 
		{L')', L'(' }, {L']', L'[' }, {L'}', L'{' },
	};

	switch (c)
	{
		case '(':
		case '[':
		case '{':
			_tokens.push_back(MakeToken(line_number, pos, 1, token_type_from_char[c]));
			_matching_check.push(c);
			pos += 1;
			break;
		case ')':
		case ']':
		case '}':
		{
            Token token = MakeToken(line_number, pos, 1, token_type_from_char[c]);
			if (_matching_check.empty())
			{
				throw CompileError(token, CompileErrorNoMatchingLeftBracket, 
					L"No matching left bracket found: " + bracket_matching[c]);
			}
			else if (_matching_check.top() != bracket_matching[c])
			{
				throw CompileError(token, CompileErrorRightBracketExpected, 
					L"Right bracket expected: " + bracket_matching[_matching_check.top()]);
			}
			else
			{
				_matching_check.pop();
				pos += 1;
				_tokens.push_back(token);
			}
			break;
		}
		default:
			assert(0);
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
	static map<TokenCategory, wstring> category = {
		{TokenCategoryOperator, L"Operator"},
		{TokenCategoryKeyword, L"Keyword"},
		{TokenCategoryStringLiteral, L"String Literal"},
		{TokenCategoryNumericalLiteral, L"Numerical Literal"},
		{TokenCategoryId, L"Id"},
		{TokenCategorySeparator, L"Separator"},
		{TokenCategorySpecial, L"Special"},
	};

	for (auto iter = _tokens.begin(); iter != _tokens.end(); ++iter)
	{
		auto token = _script_lines[iter->line].substr(iter->column, iter->length);
		auto token_item = token_info.find(iter->type);
		if (token_item != token_info.end())
		{
			output << category[token_item->second.second].c_str() << "\t: " << token.c_str() << "\n";
		}
		else
		{
			assert(0);
		}
	}
}

Yap::CompileErrorTokenType::CompileErrorTokenType(const Token& token, TokenType expected_token) :
	CompileError{token, ErrorCodeTokenType, std::wstring()}
{
	_error_message = token_info[expected_token].first + (L" expected.");
}
