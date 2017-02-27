#ifndef PipelineCompiler_h__
#define PipelineCompiler_h__

#pragma once

#include <string>
#include <vector>
#include <set>
#include <stack>
#include <memory>

#include "../shared/Interface/IMemory.h"
#include "../shared/Interface/Implement/CompositeProcessor.h"

namespace Yap
{
	class CompositeProcessor;
	class PipelineConstructor;

	enum TokenType
	{
		TokenUnknown,
		TokenId,
		TokenOperatorPointer,
		TokenOperatorLink,
		TokenOperatorDot,
		TokenOperatorAssign,
		TokenSemiColon,
		TokenComma,
		TokenLeftBrace,
		TokenRightBrace,
		TokenLeftParenthesis,
		TokenRightParenthesis,
		TokenStringLiteral,
		TokenOperatorMinus,
		TokenNumericLiteral,
		TokenKeyword,
	};

	struct Token
	{
		unsigned int line;
		unsigned int column;
		unsigned length;
		std::wstring text;
		TokenType type;

		Token() : line(0), column(0), length(0), type(TokenUnknown) {}
		Token(unsigned int token_line, unsigned token_column, unsigned token_length, TokenType token_type) :
			line(token_line), column(token_column), length(token_length), type(token_type) {}
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
	const int CompileErrorNoMatchingLeftBrace		= 1011;
	const int CompileErrorNoMatchingLeftParenthesis = 1012;
	const int CompileErrorPortExpected				= 1013;
	const int CompileErrorProcessorExpected			= 1014;
	const int CompileErrorProcessorNotFound			= 1015;
	const int CompileErrorPropertyLink				= 1016;
	const int CompileErrorPropertyNotFound			= 1017;
	const int CompileErrorPropertyOperatorExpected	= 1018;
	const int CompileErrorPropertyValueNotString	= 1019;
	const int CompileErrorPropertySet				= 1020;
	const int CompileErrorRightBraceExpected		= 1021;
	const int CompileErrorRightParenthesisExpected	= 1022;
	const int CompileErrorSelfLink					= 1023;
	const int CompileErrorSemicolonExpected			= 1024;
	const int CompileErrorStringExpected			= 1025;
	const int CompileErrorTooManyTokens				= 1026;
	const int CompileErrorTokenType					= 1027;
	const int CompileErrorUnexpectedEndOfStatement	= 1028;
	const int CompileErrorUnrecognizedSymbol		= 1029;
	const int CompileErrorValueExpected				= 1030;
	const int CompileErrorParamIdInvalid			= 1031;
	const int CompileErrorFailedOpenFile			= 1032;

	class CompileError
	{
	public:
		CompileError() : _error_number(CompileErrorSuccess) {}
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

	enum StatementType
	{
		StatementUnknown,
		StatementImport,
		StatementDeclaration,
		StatementPortLink,
		StatementPropertyLink,
		StatementAssign,
	};

	class Statement
	{
	public:
		explicit Statement(PipelineConstructor& constructor);

		/// Clear the tokens in the statement.
		void Reset();

		/// Check to see if the statement is empty.
		bool IsEmpty();

		/// Set the type of the statement.
		void SetType(StatementType type);

		/// Get the type of the statement.
		StatementType GetType() const;

		/// Process the statement to construc
		bool Process();

		/// Add the token to the statement.
		void AddToken(const Token& token);

		/// Get the total count of tokens in the statement.
		size_t GetTokenCount() const;

		/// Get the token with the given index.
		const Token& GetToken(unsigned int index);

		/// Get the last token in the statement.
        const Token& GetLastToken() const;

		/// Output the information about the statement for debugging purpose.
		void DebugOutput(std::wostream& output);

	protected:
		const Token& GetCurrentToken();
		bool AtEnd() const;

		/// 如果迭代其指向的token的类型不是type，则抛出编译错误。缺省情况下该函数不引起迭代器变化。
		void CheckFor(TokenType type, bool move_next = false);

		/// 检查迭代器指向的token是否指定的类型。该函数不引起迭代器变化。
		bool IsType(TokenType type);

		/// 试图提取一个Id，迭代器移动到提取内容之后。
		std::wstring GetId();
		std::wstring GetLiteralValue();
		/// Try to extract a variable id from the statement and move to next token.
		std::wstring GetVariableId();

		/// 试图提取处理器/成员（属性或者端口）对，迭代器移动到提取内容之后。
		std::pair<std::wstring, std::wstring> GetProcessorMember(bool empty_member_allowed = false);

		/// 试图提取参数id。迭代器移动到提取内容之后。
		std::wstring GetParamId();

		bool ProcessImport();
		bool ProcessPortLink();
		bool ProcessDeclaration();
		bool ProcessPropertyLink();
		bool ProcessAssignment();

		StatementType _type;
		std::vector<Token> _tokens;
		std::vector<Token>::iterator _iter;
		PipelineConstructor& _constructor;
	};

	class PipelineCompiler
	{
	public:
		PipelineCompiler();
		~PipelineCompiler();

        SmartPtr<CompositeProcessor> CompileFile(const wchar_t * path);
        SmartPtr<CompositeProcessor> Compile(const wchar_t * text);

	protected:
        SmartPtr<CompositeProcessor> DoCompile(std::wistream& input);

		std::vector<std::wstring> _script_lines;
		std::vector<Token> _tokens;
		std::set<std::wstring> _key_words;

		/** 这个变量用于括号匹配检查，遇到左括号时压入，遇到右括号弹出，同时检查类型是否一致。
		如果类型不一致，或者文件处理完毕后该栈不为空，则说明匹配错误。*/
		std::stack<Token> _matching_check;

		std::shared_ptr<PipelineConstructor> _constructor;

		bool Preprocess(std::wistream& input_stream);
		bool PreprocessLine(std::wstring& line, int line_number);

		bool Process();

		std::wstring GetTokenString(const Token& token) const;
		void DebugOutputTokens(std::wostream& output);
		void TestTokens();
	};
}

#endif // PipelineCompiler_h__
