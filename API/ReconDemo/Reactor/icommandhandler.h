#ifndef ICOMMANDHANDLER_H
#define ICOMMANDHANDLER_H

#include "stdint.h"

namespace cmr
{
    class MsgPack;
    class MsgUnpack;
    class ICommandHandler
    {
    public:
            virtual void DoCommand(MsgUnpack& command, MsgPack& result, uint16_t session_id) = 0;
            virtual ~ICommandHandler() = 0{}
    };
}

#endif // ICOMMANDHANDLER_H
