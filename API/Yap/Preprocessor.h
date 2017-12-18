#ifndef PREPROCESSOR_YAP_H
#define PREPROCESSOR_YAP_H

#pragma once

#include <string>
#include <vector>
#include <set>
#include <stack>
#include <memory>

#include "Implement/CompositeProcessor.h"

namespace Yap
{
	enum TokenCategory
	{
		TokenCategoryOperator,
		TokenCategoryKeyword,
		TokenCategoryStringLiteral,
		TokenCategoryNumericalLiteral,
		TokenCategoryId,
		TokenCategorySeparator,
		TokenCategorySpecial,
	};

	enum TokenType
	{
		TokenUnknown,
		TokenId,
		TokenOperatorPortLink,			///< ->
		TokenOperatorInOutMapping,		///< <=> 
		TokenOperatorInMapping,			///< <==
		TokenOperatorOutMapping,		///< ==>
        TokenOperatorDot,               ///< .
        TokenOperatorAssign,            ///< =
        TokenSemiColon,                 ///< ;
        TokenComma,                     ///< ,
		TokenColon,						///< :
		TokenDoubleColon,				///< ::
        TokenOperatorMinus,             ///< -
		TokenOperatorPlus,				///< +
		TokenOperatorMultiply,			///< *
		TokenOperatorDivide,			///< /
        TokenLeftBrace,                 ///< {
        TokenRightBrace,                ///< }

        TokenLeftParenthesis,           ///< (
        TokenRightParenthesis,          ///< )

        TokenLeftSquareBracket,         ///< [
        TokenRightSquareBracket,        ///< ]

        TokenGreaterThan,               ///< >
        TokenLessThan,                  ///< <
        TokenSharp,                     ///< #

		TokenStringLiteral,
		TokenNumericLiteral,

		// keywords: bool, float, int, string, enum, array, struct, 
		// import, include
		// self
		// true, false
		// namespace, using
		TokenKeywordBool,
		TokenKeywordFloat,
		TokenKeywordInt,
		TokenKeywordString,
		TokenKeywordArray,
		TokenKeywordEnum,
		TokenKeywordStruct,
		TokenKeywordImport,
		TokenKeywordInclude,
		TokenKeywordSelf,
		TokenKeywordTrue,
		TokenKeywordFalse,
		TokenKeywordNamespace,
		TokenKeywordUsing,
	};

	struct Token
	{
		unsigned int line;
		unsigned int column;
		unsigned length;
		std::wstring text;
		TokenType type;

		Token() : line(0), column(0), length(0), type(TokenUnknown) {}
	private:
		Token(unsigned int token_line, unsigned token_column, unsigned token_length, TokenType token_type) :
			line(token_line), column(token_column), length(token_length), type(token_type) {}
		friend class Preprocessor;
	};

	const int CompileErrorSuccess					= 0;
	const int CompileErrorAddLink					= 1001;
	const int CompileErrorCommaExpected				= 1002;
	const int CompileErrorCreateProcessor			= 1003;
	const int CompileErrorIdExists					= 1004;
	const int CompileErrorIdExpected				= 1005;
	const int CompileErrorIncompleteStatement		= 1006;
	const int CompileErrorLinkOperatorExpected		= 1007;
	const int CompileErrorLinkNotEnoughArguments	= 1008;
	const int CompileErrorLoadModule				= 1009;
	const int CompileErrorNoMatchingQuote			= 1010;
	const int CompileErrorNoMatchingLeftBracket		= 1011;

	const int CompileErrorPortExpected				= 1013;
	const int CompileErrorProcessorExpected			= 1014;
	const int CompileErrorProcessorNotFound			= 1015;
	const int CompileErrorPropertyLink				= 1016;
	const int CompileErrorPropertyNotFound			= 1017;
	const int CompileErrorPropertyOperatorExpected	= 1018;
	const int CompileErrorPropertyValueNotString	= 1019;
	const int CompileErrorPropertySet				= 1020;
	const int CompileErrorRightBracketExpected		= 1021;
	const int CompileErrorSelfLink					= 1023;
	const int CompileErrorSemicolonExpected			= 1024;
	const int CompileErrorStringExpected			= 1025;
	const int CompileErrorTooManyTokens				= 1026;
	const int ErrorCodeTokenType					= 1027;
	const int CompileErrorUnexpectedEndOfStatement	= 1028;
	const int CompileErrorUnrecognizedSymbol		= 1029;
	const int CompileErrorValueExpected				= 1030;
	const int CompileErrorParamIdInvalid			= 1031;
	const int CompileErrorFailedOpenFile			= 1032;

    const int CompileErrorUnknownToken				= 1033;
    const int CompileErrorInvalidImport             = 1034;
    const int CompileErrorTypeExpected              = 1035;
    const int CompileErrorTokenExpected             = 1036;
    const int CompileErrorTypeNotFound              = 1037;
    const int CompileErrorNestStruct                = 1038;
    const int CompileErrorMemeberExists             = 1039;
	const int CompileErrorAmbiguousId               = 1040;

	class CompileError
	{
	public:
		CompileError() : _error_number(CompileErrorSuccess) {}
		~CompileError() {}
		CompileError(const Token& token, int error_number, const std::wstring& error_message) :
			_error_message(error_message), _error_number(error_number), _token(token) {}

		const Token& GetToken() const { return _token; }
		int GetErrorNumber() const { return _error_number; }
		const std::wstring& GetErrorMessage() const { return _error_message; }
	protected:
		std::wstring _error_message;
		int _error_number;
		Token _token;
	};

	class CompileErrorTokenType : public CompileError
	{
	public:
		CompileErrorTokenType(const Token& token, TokenType expected_token);
	};

	enum StatementType
	{
		StatementUnknown,
		StatementImport,
		StatementDeclaration,
		StatementPortLink,
		StatementPropertyMapping,
		StatementAssign,

		StatementVariableDeclaration,
		StatementVariableAssign,

	};

	class Tokens
	{
	public:
		class Guard
		{
		public: 
			explicit Guard(Tokens& statement, bool end_with_semicolon = true);
			~Guard();
		private:
			Tokens& _statement;
			bool _end_with_semicolon;
		};

		explicit Tokens(std::vector<Token>& tokens);

		void StartProcessingStatement();
		/// Clear the tokens in the statement.
		void FinishProcessingStatement(bool check_semicolon = true);

		/// Check to see if the statement is empty.
		bool IsEmpty();

		/// Set the type of the statement.
		void SetType(StatementType type);

		/// Get the type of the statement.
		StatementType GetType() const;

		/// Move the iterator to the next token.
		void Next();

		/// Get the token with the given index.
		const Token& GetToken(unsigned int index);

		/// Get the last token in the statement.
        const Token& GetLastToken() const;

		/// Output the information about the statement for debugging purpose.
		void DebugOutput(std::wostream& output);

		const Token& GetCurrentToken();
		bool AtEnd() const;

		/// 如果迭代其指向的token的类型不是type，则抛出编译错误。缺省情况下该函数不引起迭代器变化。
		void AssertToken(TokenType type, bool move_next = false);

		bool IsNextTokenOfType(TokenType type);

		/// 检查迭代器指向的token是否指定的类型。该函数不引起迭代器变化。
		bool IsTokenOfType(TokenType type, bool move_next = false);

		/// 试图提取一个Id，迭代器移动到提取内容之后。
		std::wstring GetId();
		std::wstring GetLiteralValue();
		std::wstring GetStringLiteral();


		/// 试图提取处理器/成员（属性或者端口）对，迭代器移动到提取内容之后。
		std::pair<std::wstring, std::wstring> GetProcessorMember(bool empty_member_allowed = false);

		/// Try to extract a variable id from the statement and move to next token.
		std::wstring GetVariableIdOld();
		std::wstring GetVariableId();
		std::wstring GetIdWithIndexing(); 

		/// 试图提取参数id。迭代器移动到提取内容之后。
		std::wstring GetParamId();

		std::wstring GetNamespaceId();

		/// Try to extract namespace qualifier from the statement and move to next token.
		std::wstring GetNamespaceQualifier();
	
		bool IsFirstTokenInStatement() const;
	protected:
		StatementType _type;

		std::vector<Token> _tokens;
		std::vector<Token>::const_iterator _begin;
		std::vector<Token>::const_iterator _iter;
		std::vector<Token>::const_iterator _peek_iter;
	};

	enum PreprocessType
	{
		PreprocessPipeline,
		PreprocessVariableDefinition,
		PreprocessScan,
	};

	class Preprocessor
	{
	public:
		explicit Preprocessor(PreprocessType type);
		bool Preprocess(std::wistream& input_stream);
		bool Preprocess(const wchar_t * const input);

		Tokens GetTokens();
	protected:
		Token MakeToken(unsigned int token_line, unsigned token_column, unsigned token_length, TokenType token_type);

		std::vector<std::wstring> _script_lines;
		std::vector<Token> _tokens;

		std::set<TokenType> _supported_tokens; ///< Tokens used in current type of definition file.
		
		std::map<std::wstring, TokenType> _operators;
		std::map<std::wstring, TokenType> _keywords;

		/** Used for matching braces/brackets. When the left ones are encountered, push then into the 
		stack and pop one out when the right ones are encountered. 
		If the types of braces/brackets are not matching, or if the stack is not empty at the end of 
		file, then there must be a matching error.
		Possible values in the stack: (, {, [.
		*/
		std::stack<wchar_t> _matching_check;

		bool PreprocessLine(std::wstring& line, int line_number);

		int CheckBraceMatching(wchar_t c, int line_number, int pos);

		void DebugOutputTokens(std::wostream& output);
	};
}

#endif // PREPROCESSOR_YAP_H
