#include "stdafx.h"
#include "PipelineConstructor.h"

using namespace Yap;
using namespace std;

CPipelineConstructor::CPipelineConstructor()
{
}


CPipelineConstructor::~CPipelineConstructor()
{
}

std::shared_ptr<CProcessorAgent> Yap::CPipelineConstructor::CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id)
{
	_module_manager->CreateProcessor(class_id, instance_id);

}
