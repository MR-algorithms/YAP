// PipelineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winbase.h>
#include <iostream>
#include "..\Interface\YapInterfaces.h"
#include "../Interface/DataImp.h"
#include <string>
#include <vector>

using namespace std;
using namespace Yap;

#include "ProcessorDebugger.h"


int main()
{
	CProcessorDebugger debugger;
	debugger.DebugPlugin(L"BasicRecon.dll");

// 	vector<double> image(256 * 256);
// 	for (unsigned int i = 0; i < image.size(); ++i)
// 	{
// 		image[i] = 1.0;
// 	}
// 	Yap::CDimensions dimension;
// 	dimension(DimensionReadout, 0, 256)
// 		(DimensionPhaseEncoding, 0, 256);
// 
// 	auto * data = new Yap::CDoubleData(image.data(), dimension, nullptr, false);

	// processor->Input(L"Input", data);

    return 0;
}

