#pragma once

#include "../Interface/Interface.h"
#include <map>

class CProcessorManager :
	public IProcessorManager
{
public:
	CProcessorManager();
	~CProcessorManager();

	virtual IProcessor * GetFirstProcessor() override;
	virtual IProcessor * GetNextProcessor() override;
	virtual IProcessor * GetProcessor(const char * name) override;

	bool AddProcessor(IProcessor * processor);

private:
	std::map<std::string, IProcessor*> _processors;
	std::map<std::string, IProcessor*>::iterator _current;
};

