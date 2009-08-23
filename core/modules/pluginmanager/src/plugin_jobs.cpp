/**
 * @file plugin_jobs.cpp
 * Implementation of the plugin job classes' member functions.
 */

#include "../inc/plugin_jobs.h"
#include "../inc/plugin.h"

namespace anp
{
namespace firc
{

	// PluginJob
	PluginJob::PluginJob(Plugin *plugin):
	Job(),
	m_plugin(plugin)
	{
		
	}

	PluginJob::~PluginJob()
	{
		
	}

	// JoinJob
	JoinJob::JoinJob(Plugin *plugin,
			PF_irc_onJoin,
			void *network,
			const int8 *channel,
			const int8 *user):
	PluginJob(plugin),
	m_network(network),
	m_channel(channel),
	m_user(user)
	{
		
	}
	JoinJob::~JoinJob()
	{
		
	}
	
	void JoinJob::executeCustom()
	{
		m_func(m_network, m_channel.c_str(), m_user.c_str());
	}

	// PrivMsg
	PrivMsgJob::PrivMsgJob(
					Plugin *plugin,
					PF_irc_onPrivMsg func,
					void *network,
					const int8 *sender,
					const int8 *target,
					const int8 *message):
	PluginJob(plugin),
	m_func(func),
	m_network(network),
	m_sender(sender),
	m_target(target),
	m_message(message)
	{
		
	}

	PrivMsgJob::~PrivMsgJob()
	{
		
	}

	void PrivMsgJob::executeCustom()
	{
		m_func(m_network, m_sender.c_str(), m_target.c_str(), m_message.c_str());
	}

} // namespace firc
} // namespace anp
