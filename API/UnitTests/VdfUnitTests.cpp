#include "stdafx.h"

#include <boost/test/unit_test.hpp>

#include "Yap/VdfParser.h"
#include <string>

using namespace Yap;

std::shared_ptr<VariableSpace> Compile(const wchar_t * script)
{
	VdfParser parser;
	return parser.Compile(script);
}

BOOST_AUTO_TEST_CASE(vdf_test_basic)
{
	auto variables = Compile(
		L"int i;"
		L"float f;"
		L"string s;"
		L"bool b;");

	BOOST_CHECK(variables->GetVariable(L"i") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"f") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"s") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"b") != nullptr);

	variables = Compile(
		L"int n_1;"
		L"float _n1;"
		L"float d31;");
	BOOST_CHECK(variables->GetVariable(L"n_1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"_n1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"d31") != nullptr);

	variables = Compile(
		L"int a#<0, 2>;"
		L"float f#<0, 2>;"
		L"string s#<0, 2>;"
		L"bool b#<0, 2>;");

	BOOST_CHECK(variables->GetVariable(L"a0") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"a1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"a2") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"a3") == nullptr);

	BOOST_CHECK(variables->GetVariable(L"f0") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"f1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"f2") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"f3") == nullptr);

	BOOST_CHECK(variables->GetVariable(L"s0") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"s1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"s2") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"s3") == nullptr);

	BOOST_CHECK(variables->GetVariable(L"b0") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"b1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"b2") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"b3") == nullptr);
}

BOOST_AUTO_TEST_CASE(vdf_test_namespace)
{
	auto variables = Compile(
		L"namespace my_namespace {"
		L"int a;"
		L"float f;"
		L"bool b;"
		L"string s;}"
	);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::a") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::f") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::b") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::s") != nullptr);

	variables = Compile(
		L"namespace my_namespace {"
		L"int a#<0, 2>;}"
	);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::a0") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::a1") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::a2") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"my_namespace::a3") == nullptr);

	variables = Compile(
		L"namespace n{ struct my_struct { int n;};}"
		L"using namespace n;"
		L"my_struct ms;");

	BOOST_CHECK(variables->GetVariable(L"ms.n") != nullptr);
}

BOOST_AUTO_TEST_CASE(vdf_test_array)
{
	auto variables = Compile(
		L"array<int> i;"
		L"array<float> f;"
		L"array<string> s;"
		L"array<bool> b;"
	);

	BOOST_CHECK(variables->GetVariable(L"i") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"f") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"s") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"b") != nullptr);

	variables->Set<int>(L"i[0]", 1);
	BOOST_CHECK_EQUAL(variables->Get<int>(L"i[0]"), 1);
	variables->Set<double>(L"f[0]", 1.0);
	BOOST_CHECK_EQUAL(variables->Get<double>(L"f[0]"), 1.0);
	variables->Set<const wchar_t * const>(L"s[0]", L"Test");
	BOOST_CHECK_EQUAL(variables->Get<const wchar_t * const>(L"s[0]"), L"Test");

	variables->Set<bool>(L"b[0]", true);
	BOOST_CHECK_EQUAL(variables->Get<bool>(L"b[0]"), true);
	variables->Set<bool>(L"b[0]", false);
	BOOST_CHECK_EQUAL(variables->Get<bool>(L"b[0]"), false);

	variables = Compile(
		L"namespace n{"
		L"array<int> i;"
		L"array<float> f;"
		L"array<string> s;"
		L"array<bool> b;}"
	);
	BOOST_CHECK(variables->GetVariable(L"n::i") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"n::f") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"n::s") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"n::b") != nullptr);

	variables->Set<int>(L"n::i[0]", 1);
	BOOST_CHECK_EQUAL(variables->Get<int>(L"n::i[0]"), 1);
	variables->Set<double>(L"n::f[0]", 1.0);
	BOOST_CHECK_EQUAL(variables->Get<double>(L"n::f[0]"), 1.0);
	variables->Set<const wchar_t * const>(L"n::s[0]", L"Test");
	BOOST_CHECK_EQUAL(variables->Get<const wchar_t * const>(L"n::s[0]"), L"Test");

	variables->Set<bool>(L"n::b[0]", true);
	BOOST_CHECK_EQUAL(variables->Get<bool>(L"n::b[0]"), true);
	variables->Set<bool>(L"n::b[0]", false);
	BOOST_CHECK_EQUAL(variables->Get<bool>(L"n::b[0]"), false);
}

BOOST_AUTO_TEST_CASE(vdf_test_struct)
{
	auto variables = Compile(
		L"struct my_struct{"
		L"int i;"
		L"float f;"
		L"string s;"
		L"bool b;"
		L"};"
		L"my_struct ms;"
	);
	BOOST_CHECK(variables->GetVariable(L"ms.i") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"ms.f") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"ms.s") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"ms.b") != nullptr);

	struct my_struct {
		int i;
		float f;
		bool b;
	};
	auto a = variables->GetVariable(L"ms");
}

BOOST_AUTO_TEST_CASE(vdf_array_in_struct)
{
	auto variables = Compile(
		L"struct my_struct{"
		L"int size;"
		L"array<float> data;"
		L"};"
		L"my_struct ms;");
	BOOST_CHECK(variables->GetVariable(L"ms.size") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"ms.data") != nullptr);
	variables->Set<double>(L"ms.data[0]", 1.6);
	BOOST_CHECK_EQUAL(variables->Get<double>(L"ms.data[0]"), 1.6);

	variables = Compile(
		L"struct struct_inner{ int a;};"
		L"struct struct_outer{struct_inner s;};"
		L"struct_outer nest_struct;");
	BOOST_CHECK(variables->GetVariable(L"nest_struct.s.a") != nullptr);
	variables->Set<int>(L"nest_struct.s.a", 3);
	BOOST_CHECK_EQUAL(variables->Get<int>(L"nest_struct.s.a"), 3);
}

BOOST_AUTO_TEST_CASE(vdf_test_nested_struct)
{
	auto variables = Compile(
		L"namespace n{"
		L"struct struct_inner{ int a;};"
		L"struct struct_outer{struct_inner s;};"
		L"struct_outer nest_struct;"
		L"}");
	BOOST_CHECK(variables->GetVariable(L"n::nest_struct.s.a") != nullptr);
	variables->Set<int>(L"n::nest_struct.s.a", 3);
	BOOST_CHECK_EQUAL(variables->Get<int>(L"n::nest_struct.s.a"), 3);
}

BOOST_AUTO_TEST_CASE(vdf_test_array_of_struct)
{
	auto variables = Compile(
		L"struct my_struct{"
		L"int size;};"
		L"array<my_struct> arr;");
	BOOST_CHECK(variables->GetVariable(L"arr[0].size") != nullptr);
	variables->Set<int>(L"arr[0].size", 1);
	BOOST_CHECK(variables->Get<int>(L"arr[0].size") == 1);
	BOOST_CHECK(variables->Variables() != nullptr);
	BOOST_CHECK(variables->Variables()->GetIterator() != nullptr);
	auto iter = variables->Variables()->GetIterator();
	BOOST_CHECK(iter->GetFirst() != nullptr);
	auto first = iter->GetFirst();
	BOOST_CHECK(std::wstring(first->GetId()) == std::wstring(L"arr"));

	BOOST_CHECK(variables->GetArrayWithSize<IVariable*>(L"arr").first != nullptr);

	variables = Compile(
		L"namespace n{"
		L"struct my_struct{"
		L"int size;};"
		L"array<my_struct> arr;"
		L"}");
	BOOST_CHECK(variables->GetVariable(L"n::arr[0].size") != nullptr);

}