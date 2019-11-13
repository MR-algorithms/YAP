#pragma once
#include "Implement/processorImpl.h"
#include "SpecControl/TransmitterProtocol.h"
#include <thread>

using namespace TransmitterProtocol;
namespace Yap
{
	class IceReceiver :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(IceReceiver)
	public:
		IceReceiver(void);
		IceReceiver(const IceReceiver& rhs);

	private:
		~IceReceiver();

		virtual bool Input(const wchar_t * name, IData * data) override;
		
		void OutputDatablock(DataBlock &data_block);
		bool Input_Reserve(const wchar_t * name, IData * data);

		static bool ReceivedataThreadfunc();

		std::shared_ptr<std::thread> _thread;
	
	};
}



