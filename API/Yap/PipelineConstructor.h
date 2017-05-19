#pragma once

#ifndef PipelineConstructor_h__20160813
#define PipelineConstructor_h__20160813

#include <memory>
#include <string>

#include "Implement/CompositeProcessor.h"

namespace Yap
{

	const int ConstructErrorSuccess					= 0;
	const int ConstructErrorCreateProcessor			= 1;
	const int ConstructErrorAddLink					= 2;
	const int ConstructErrorLinkNotEnoughArguments	= 3;
	const int ConstructErrorUnknownStatement		= 4;
	const int ConstructErrorLoadModule				= 5;
	const int ConstructErrorIdExists				= 6;
	const int ConstructErrorPropertiesNotFound		= 7;
	const int ConstructErrorPropertyNotFound		= 8;
	const int ConstructErrorProcessorNotFound		= 9;
	const int ConstructErrorPropertyValueNotString	= 10;
	const int ConstructErrorPropertyLink			= 11;
	const int ConstructErrorPropertySet				= 12;
	const int ConstructErrorSelfLink				= 13;
	const int ConstructErrorOutOfMemory				= 100;

	class ConstructError
	{
	public:
		ConstructError();
		ConstructError(unsigned int line_number, int error_number, const std::wstring& error_message);
		unsigned int GetLineNumber() const;
		int	GetErrorNumber() const;
		std::wstring GetErrorMessage() const;

	protected:
		std::wstring _error_message;
		int _error_number;
		unsigned int _line_number;
	};

	class ModuleManager;
	class ProcessorAgent;
	struct IProcessor;

	class PipelineConstructor
	{
	public:
		PipelineConstructor();
		virtual ~PipelineConstructor();

		void Reset(bool reset_module_manager = true);

		bool LoadModule(const wchar_t * module_name);

		void SetPluginFolder(const wchar_t * path);

		const wchar_t * GetPluginFolder() const;

		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);

		bool Link(const wchar_t * source, const wchar_t * source_port, const wchar_t * dest, const wchar_t * dest_port);

		bool Link(const wchar_t * source, const wchar_t * dest);

		bool SetProperty(const wchar_t * processor_id, const wchar_t * property_id, const wchar_t * value);

		bool MapProperty(const wchar_t * processor_id, const wchar_t * property_id, const wchar_t * param_id, 
			bool input, bool output);

		bool MapInput(const wchar_t * pipeline_port, const wchar_t * inner_processor, const wchar_t * inner_port);

		bool MapOutput(const wchar_t * pipeline_port, const wchar_t * inner_processor, const wchar_t * inner_port);

		bool InstanceIdExists(const wchar_t * id);

        Yap::SmartPtr<Pipeline> GetPipeline();

	protected:
        Yap::SmartPtr<Pipeline> _pipeline;
		std::shared_ptr<ModuleManager> _module_manager;
		std::wstring _plugin_folder;
	};

}
#endif // PipelineConstructor_h__
