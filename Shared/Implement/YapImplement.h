#pragma once

#ifndef YapImplement_h_20160831
#define YapImplement_h_20160831

#include "ContainerImpl.h"

#define BEGIN_DECL_PROCESSORS ContainerImpl<IProcessor> * g_processor_manager;\
	extern "C" {\
	__declspec(dllexport) void ReleaseProcessManager(){\
		if (g_processor_manager) {\
			ISharedObject* obj = g_processor_manager;\
			obj->Release();\
			g_processor_manager = nullptr;\
		}\
	}\
	__declspec(dllexport) IProcessorContainer * GetProcessorManager()\
	{\
		if (g_processor_manager)\
			return g_processor_manager;\
		g_processor_manager = new (std::nothrow) ContainerImpl<IProcessor>;\
		if (!g_processor_manager) return nullptr;\
		((ISharedObject*)  g_processor_manager)->Lock();

#define ADD_PROCESSOR(class_name) try{\
	auto * my##class_name = new class_name; \
	g_processor_manager->Add(my##class_name->GetClassId(), my##class_name);\
}\
catch(...){\
	return nullptr;\
}\

#define ADD(a, b) auto pb = b; \
g_processor_manager->Add((a), pb);

#define END_DECL_PROCESSORS return g_processor_manager;\
}\
} //end extern "C"

#endif