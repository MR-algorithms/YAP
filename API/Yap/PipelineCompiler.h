#ifndef PipelineCompiler_h__
#define PipelineCompiler_h__

#pragma once

#include <string>
#include <vector>
#include <set>
#include <stack>
#include <memory>

#include "Implement/CompositeProcessor.h"
#include "Preprocessor.h"

namespace Yap
{
	class CompositeProcessor;
	class PipelineConstructor;

	class PipelineCompiler
	{
	public:
		PipelineCompiler();
		~PipelineCompiler();

        SmartPtr<CompositeProcessor> CompileFile(const wchar_t * path);
        SmartPtr<CompositeProcessor> Compile(const wchar_t * text);

	protected:
        SmartPtr<CompositeProcessor> DoCompile(std::wistream& input);

		bool ProcessImport(Statement& statement);
		bool ProcessPortLink(Statement& statement);
		bool ProcessDeclaration(Statement& statement);
		bool ProcessPropertyLink(Statement& statement);
		bool ProcessAssignment(Statement& statement);

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<PipelineConstructor> _constructor;

		bool Process();
	};
}

#endif // PipelineCompiler_h__
