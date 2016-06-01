#pragma once

struct IPort;

struct IPortEnumerator
{
	virtual IPort * GetFirstPort() = 0;
	virtual IPort * GetNextPort() = 0;
};

struct IData;

struct IProcessor
{
	virtual char * GetId() = 0;
	virtual IPortEnumerator * GetInputPortEnumerator() = 0;
	virtual IPortEnumerator * GetOutputPortEnumerator() = 0;

	virtual bool Init() = 0;
	// virtual bool Process() = 0;
	virtual bool Input(const char * port, IData * data) = 0;
};


struct IProcessorManager
{
	virtual IProcessor * GetFirstProcessor() = 0;
	virtual IProcessor * GetNextProcessor() = 0;
	virtual IProcessor * GetProcessor(const char * name) = 0;
};

