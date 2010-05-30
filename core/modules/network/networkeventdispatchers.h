#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <eventdispatcher.h>
#include <networkevents.h>

namespace anp
{
namespace firc
{
namespace eventdispatchers
{

typedef IEventDispatcherSubscriber<ISubscriber<events::Join> > Join;
typedef IEventDispatcherSubscriber<ISubscriber<events::Part> > Part;
typedef IEventDispatcherSubscriber<ISubscriber<events::PrivMsg> > PrivMsg;
typedef IEventDispatcherSubscriber<ISubscriber<events::Topic> > Topic;
typedef IEventDispatcherSubscriber<ISubscriber<events::NumericReply> > NumericReply;
typedef IEventDispatcherSubscriber<ISubscriber<events::Ping> > Ping;
typedef IEventDispatcherSubscriber<ISubscriber<events::NewNetwork> > NewNetwork;
typedef IEventDispatcherSubscriber<ISubscriber<events::RemovingNetwork> > RemovingNetwork;

} // namespace dispatchers
namespace dispatchers = eventdispatchers;
} // namespace firc
} // namespace anp

#endif // _NETWORK_H_
