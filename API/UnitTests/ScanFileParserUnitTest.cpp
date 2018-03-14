#include "stdafx.h"

#include <boost/test/unit_test.hpp>

#include "Yap/ScanFileParser.h"
#include "Yap/VdfParser.h"
#include <string>

using namespace Yap;
using namespace std;

std::shared_ptr<VariableSpace> CompileSF(const wchar_t * script)
{
	VdfParser vdfp;
	auto variable = vdfp.Compile(
		L"int i;"
		L"float f;"
		L"string s;"
		L"bool b;"
		L"array<int> ia;"
		L"array<float> fa;"
		L"array<string> sa;"
		L"array<bool> ba;"
		L"struct sn"
		L"{"
		L"	int si;"
		L"	float sf;"
		L"	string ss;"
		L"	bool sb;"
		L"};"
		L"sn st;"
		L"array<sn> sta;");

	if (variable == nullptr)
		return nullptr;

	ScanFileParser sfp;
	sfp.SetVariableSpace(variable);

	if (!sfp.Compile(script))
		return nullptr;

	return sfp.GetVariableSpace();
}

BOOST_AUTO_TEST_CASE(sfp_test_basic)
{
	auto variables = CompileSF(L"i = 1;");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->GetVariable(L"i") != nullptr);
	BOOST_CHECK(variables->GetVariable(L"i")->ToString() == wstring(L"1"));

	variables = CompileSF(L"f = 1.1;");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<double>(L"f") == 1.1);

	variables = CompileSF(L"s = \"abcd\";");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<wstring>(L"s") == wstring(L"abcd"));

	variables = CompileSF(L"b = false;");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<bool>(L"b") == false);

	variables = CompileSF(
		L"ia.resize(3);"
		L"ia = [0, 1];"
		L"ia[2] = 2;");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<int>(L"ia[0]") == 0);
	BOOST_CHECK(variables->Get<int>(L"ia[1]") == 1);
	BOOST_CHECK(variables->Get<int>(L"ia[2]") == 2);

	variables = CompileSF(
		L"fa.resize(3);"
		L"fa = [0.1, 1.2];"
		L"fa[2] = 2.3;");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<double>(L"fa[0]") == 0.1);
	BOOST_CHECK(variables->Get<double>(L"fa[1]") == 1.2);
	BOOST_CHECK(variables->Get<double>(L"fa[2]") == 2.3);

	// 暂时没支持array<string>类型的resize
// 	variables = CompileSF(
// 		L"sa.resize(3);"
// 		L"sa = [\"0\", \"1\"];"
// 		L"sa[2] = \"2\";");
// 	BOOST_CHECK(variables != nullptr);
// 	BOOST_CHECK(variables->Get<wstring>(L"sa[0]") == wstring(L"0"));
// 	BOOST_CHECK(variables->Get<wstring>(L"sa[1]") == wstring(L"1"));
// 	BOOST_CHECK(variables->Get<wstring>(L"sa[2]") == wstring(L"2"));

// 	variables = CompileSF(
// 		L"ba.resize(3);"
// 		L"ba = [false, true];"
// 		L"ba[2] = true;");
// 	BOOST_CHECK(variables != nullptr);
// 	BOOST_CHECK(variables->Get<bool>(L"ba[0]") == false);
// 	BOOST_CHECK(variables->Get<bool>(L"ba[1]") == true);
// 	BOOST_CHECK(variables->Get<bool>(L"ba[2]") == true);

	variables = CompileSF(
		L"st = {\"si\" : 1, \"sf\" : 1.1};"
		L"st.ss = \"abc\";");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<int>(L"st.si") == 1);
	BOOST_CHECK(variables->Get<double>(L"st.sf") == 1.1);
	BOOST_CHECK(variables->Get<wstring>(L"st.ss") == wstring(L"abc"));

	variables = CompileSF(
		L"sta = [{\"si\" : 1, \"sf\" : 1.1}];"
		L"sta[0].ss = \"abc\";"
		L"sta.resize(2);"
		L"sta[1] = {\"si\" : 2, \"sf\" : 2.2};");
	BOOST_CHECK(variables != nullptr);
	BOOST_CHECK(variables->Get<int>(L"sta[0].si") == 1);
	BOOST_CHECK(variables->Get<double>(L"sta[0].sf") == 1.1);
	BOOST_CHECK(variables->Get<wstring>(L"sta[0].ss") == wstring(L"abc"));
	BOOST_CHECK(variables->Get<int>(L"sta[1].si") == 2);
	BOOST_CHECK(variables->Get<double>(L"sta[1].sf") == 2.2);
}