/**
 * @file plugin_jobs.cpp
 * Implementation of the plugin job classes' member functions.
 */

#include "../plugin_jobs.h"
#include "../inc/plugin.h"

namespace anp
{
namespace firc
{

	// PluginJob
	PluginJob::PluginJob(Plugin *plugin):
	Job(),
	m_plugin(plugin),
	m_func(NULL)
	{
		
	}

	PluginJob::~PluginJob()
	{
		
	}
	
	void PluginJob::setPlugin(Plugin *plugin)
	{
		m_plugin = plugin;
	}
	
	void PluginJob::setFunc(void *func)
	{
		m_func = func;
	}

	// JoinJob
	JoinJob::JoinJob(Plugin *plugin,
			INetworkManagerFrontend &network,
			const MsgPrefix &origin,
			const int8 *channel):
	PluginJob(plugin),
	m_network(network),
	m_origin(origin),
	m_channel(channel)
	{
		
	}
	JoinJob::~JoinJob()
	{
		
	}
	
	void JoinJob::executeCustom()
	{
		PF_irc_onJoin f = (PF_irc_onJoin)m_func;
		f(m_network, m_origin, m_channel.c_str());
	}

	// PartJob
	PartJob::PartJob(Plugin *plugin,
			INetworkManagerFrontend &network,
			const MsgPrefix &origin,
			const int8 *channel,
			const int8 *message):
	PluginJob(plugin),
	m_network(network),
	m_origin(origin),
	m_channel(channel),
	m_message(message)
	{
		
	}
	PartJob::~PartJob()
	{
		
	}
	
	void PartJob::executeCustom()
	{
		PF_irc_onPart f = (PF_irc_onPart)m_func;
		f(m_network, m_origin, m_channel.c_str(), m_message.c_str());
	}

	// PrivMsg
	PrivMsgJob::PrivMsgJob(
					Plugin *plugin,
					INetworkManagerFrontend &network,
					const MsgPrefix &origin,
					const int8 *target,
					const int8 *message):
	PluginJob(plugin),
	m_network(network),
	m_origin(origin),
	m_target(target),
	m_message(message)
	{
		
	}

	PrivMsgJob::~PrivMsgJob()
	{
		
	}

	void PrivMsgJob::executeCustom()
	{
		PF_irc_onPrivMsg f = (PF_irc_onPrivMsg)m_func;
		f(m_network, m_origin, m_target.c_str(), m_message.c_str());
	}

	TopicJob::TopicJob(	Plugin *plugin,
						INetworkManagerFrontend &network,
						const MsgPrefix &origin,
						const std::string &channel,
						const std::string &topic):
	PluginJob(plugin),
	m_network(network),
	m_origin(origin),
	m_channel(channel),
	m_topic(topic)
	{

	}

	TopicJob::~TopicJob()
	{

	}

	void TopicJob::executeCustom()
	{
		PF_irc_onTopic f = (PF_irc_onTopic)m_func;
		f(m_network, m_origin, m_channel.c_str(), m_topic.c_str());
	}

} // namespace firc
} // namespace anp
