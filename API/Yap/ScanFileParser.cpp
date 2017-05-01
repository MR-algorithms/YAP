#include "ScanFileParser.h"
#include "Implement/VariableManager.h"
#include "Preprocessor.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <ctype.h>

using namespace Yap;
using namespace std;

ScanFileParser::ScanFileParser() :
	_preprocessor {	make_shared<Preprocessor>(PreprocessScan)},
	_variables{ make_shared<VariableManager>() }
{}


ScanFileParser::~ScanFileParser()
{
}
