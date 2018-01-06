// PipelineUnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

#include "Yap/Preprocessor.h"

using namespace Yap;

Yap::Tokens GetTokens(const wchar_t * const script)
{
	Preprocessor preprocessor(PreprocessVariableDefinition);

	preprocessor.Preprocess(script);
	return preprocessor.GetTokens();
}

BOOST_AUTO_TEST_CASE(preprocessor_tokens)
{
	BOOST_CHECK(GetTokens(L"namespace").GetCurrentToken().type == TokenKeywordNamespace);
	BOOST_CHECK(GetTokens(L"true").GetCurrentToken().type == TokenKeywordTrue);
	BOOST_CHECK(GetTokens(L"false").GetCurrentToken().type == TokenKeywordFalse);
	BOOST_CHECK(GetTokens(L"using").GetCurrentToken().type == TokenKeywordUsing);
	BOOST_CHECK(GetTokens(L"import").GetCurrentToken().type == TokenKeywordImport);
	BOOST_CHECK(GetTokens(L"bool").GetCurrentToken().type == TokenKeywordBool);
	BOOST_CHECK(GetTokens(L"float").GetCurrentToken().type == TokenKeywordFloat);
	BOOST_CHECK(GetTokens(L"int").GetCurrentToken().type == TokenKeywordInt);
	BOOST_CHECK(GetTokens(L"string").GetCurrentToken().type == TokenKeywordString);
	BOOST_CHECK(GetTokens(L"array").GetCurrentToken().type == TokenKeywordArray);
	BOOST_CHECK(GetTokens(L"struct").GetCurrentToken().type == TokenKeywordStruct);
	BOOST_CHECK(GetTokens(L"include").GetCurrentToken().type == TokenKeywordInclude);
	BOOST_CHECK(GetTokens(L"self").GetCurrentToken().type == TokenKeywordSelf);

	BOOST_CHECK(GetTokens(L"Id1").GetCurrentToken().type == TokenId);

	BOOST_CHECK(GetTokens(L"3.14").GetCurrentToken().type == TokenNumericLiteral);
	BOOST_CHECK(GetTokens(L"14").GetCurrentToken().type == TokenNumericLiteral);
	BOOST_CHECK(GetTokens(L"14.0e-7").GetCurrentToken().type == TokenNumericLiteral);
	BOOST_CHECK(GetTokens(L"\"Test hello\"").GetCurrentToken().type == TokenStringLiteral);
	BOOST_CHECK(GetTokens(L"->").GetCurrentToken().type == TokenOperatorPortLink);
	BOOST_CHECK(GetTokens(L"<=>").GetCurrentToken().type == TokenOperatorInOutMapping);
	BOOST_CHECK(GetTokens(L"<==").GetCurrentToken().type == TokenOperatorInMapping);
	BOOST_CHECK(GetTokens(L"==>").GetCurrentToken().type == TokenOperatorOutMapping);
	BOOST_CHECK(GetTokens(L".").GetCurrentToken().type == TokenOperatorDot);
	BOOST_CHECK(GetTokens(L"=").GetCurrentToken().type == TokenOperatorAssign);
	BOOST_CHECK(GetTokens(L";").GetCurrentToken().type == TokenSemiColon);
	BOOST_CHECK(GetTokens(L",").GetCurrentToken().type == TokenComma);
	BOOST_CHECK(GetTokens(L"::").GetCurrentToken().type == TokenDoubleColon);
	BOOST_CHECK(GetTokens(L"-").GetCurrentToken().type == TokenOperatorMinus);

	auto tokens = GetTokens(L"()");
	BOOST_CHECK(tokens.GetCurrentToken().type == TokenLeftParenthesis &&
		tokens.IsNextTokenOfType(TokenRightParenthesis));

	tokens = GetTokens(L"{}");
	BOOST_CHECK(tokens.GetCurrentToken().type == TokenLeftBrace &&
		tokens.IsNextTokenOfType(TokenRightBrace));

	tokens = GetTokens(L"[]");
	BOOST_CHECK(tokens.GetCurrentToken().type == TokenLeftSquareBracket &&
		tokens.IsNextTokenOfType(TokenRightSquareBracket));

	BOOST_CHECK(GetTokens(L">").GetCurrentToken().type == TokenGreaterThan);
	BOOST_CHECK(GetTokens(L"<").GetCurrentToken().type == TokenLessThan);
	BOOST_CHECK(GetTokens(L"#").GetCurrentToken().type == TokenSharp);
}

BOOST_AUTO_TEST_CASE(preprocessor_test)
{
	BOOST_CHECK(GetTokens(L"test_namespace::v").GetNamespaceQualifier() == L"test_namespace");
	BOOST_CHECK(GetTokens(L"test_namespace::v.a[1]").GetVariableId() == L"test_namespace::v.a[1]");

	// not support in file format, but expected in processor.
	BOOST_WARN(GetTokens(L"t::v.m1.m2.m3[1][2]").GetVariableId() == L"t::v.m1.m2.m3[1][2]");

	auto tokens = GetTokens(L"test = 3.14");
	BOOST_CHECK(tokens.GetVariableId() == L"test");
	tokens.AssertToken(TokenOperatorAssign, true);
	BOOST_CHECK(tokens.GetLiteralValue() == L"3.14");

	tokens = GetTokens(L"name = \"test\";");
	BOOST_CHECK(tokens.GetVariableId() == L"name");
	tokens.AssertToken(TokenOperatorAssign, true);
	BOOST_CHECK(tokens.GetLiteralValue() == L"test");

	BOOST_CHECK(GetTokens(L"v.a").GetId() == L"v");
}

Yap::Tokens GetPipelineTokens(const wchar_t * const script)
{
	Preprocessor preprocessor(PreprocessPipeline);

	preprocessor.Preprocess(script);
	return preprocessor.GetTokens();
}
BOOST_AUTO_TEST_CASE(pipiline_preprocessor_test)
{
}