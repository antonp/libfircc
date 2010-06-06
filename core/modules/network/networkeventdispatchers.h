/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <eventdispatcher.h>
#include <networkevents.h>

namespace anp
{
namespace firc
{
	
/**
	Collection of typedefs for various event dispatcher subscriber interfaces.
	Provided for increased readability and to avoid having to type too much.
*/
namespace eventdispatchers
{

typedef IEventDispatcherSubscriber<ISubscriber<events::Join> > Join;
typedef IEventDispatcherSubscriber<ISubscriber<events::Part> > Part;
typedef IEventDispatcherSubscriber<ISubscriber<events::PrivMsg> > PrivMsg;
typedef IEventDispatcherSubscriber<ISubscriber<events::Topic> > Topic;
typedef IEventDispatcherSubscriber<ISubscriber<events::NumericReply> > NumericReply;
typedef IEventDispatcherSubscriber<ISubscriber<events::Command> > Command;
typedef IEventDispatcherSubscriber<ISubscriber<events::Ping> > Ping;
typedef IEventDispatcherSubscriber<ISubscriber<events::NewNetwork> > NewNetwork;
typedef IEventDispatcherSubscriber<ISubscriber<events::RemovingNetwork> > RemovingNetwork;
typedef IEventDispatcherSubscriber<ISubscriber<events::ExceptionOccured> > ExceptionOccured;

} // namespace dispatchers
namespace dispatchers = eventdispatchers;
} // namespace firc
} // namespace anp

#endif // _NETWORK_H_
