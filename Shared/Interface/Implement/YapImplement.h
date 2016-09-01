#pragma once

#ifndef YapImplement_h_20160831
#define YapImplement_h_20160831

#define BEGIN_DECL_PROCESSORS extern "C" {\
	__declspec(dllexport) IProcessorContainer * GetProcessorManager()\
	{\
		auto processor_manager = new ContainerImpl<IProcessor>;

#define ADD_PROCESSOR(class_name) auto * my##class_name = new class_name; \
if (!my##class_name->Init())\
{\
	return nullptr;\
}\
processor_manager->Add(my##class_name->GetClassId(), my##class_name);

#define ADD(a, b) processor_manager->Add((a), (b));

#define END_DECL_PROCESSORS return processor_manager;\
}\
} //end extern "C"

#endif