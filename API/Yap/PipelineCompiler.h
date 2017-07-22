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

		bool ProcessImport(Tokens& statement);
		bool ProcessPortLink(Tokens& statement);
		bool ProcessDeclaration(Tokens& statement);
		bool ProcessPropertyMapping(Tokens& statement);
		bool ProcessAssignment(Tokens& statement);

		std::shared_ptr<Preprocessor> _preprocessor;
		std::shared_ptr<PipelineConstructor> _constructor;

		bool Process();
	};
}

#endif // PipelineCompiler_h__
