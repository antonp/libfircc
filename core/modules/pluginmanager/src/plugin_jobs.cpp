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
		PF_irc_onJoin f = (PF_irc_onJoin)m_func;
		f(m_network, m_channel.c_str(), m_user.c_str());
	}

	// PrivMsg
	PrivMsgJob::PrivMsgJob(
					Plugin *plugin,
					INetworkManagerFrontend &network,
					const int8 *nick,
					const int8 *user,
					const int8 *host,
					const int8 *target,
					const int8 *message):
	PluginJob(plugin),
	m_network(network),
	m_nick(nick),
	m_user(user),
	m_host(host),
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
		f(m_network, m_nick.c_str(), m_user.c_str(),
			m_host.c_str(), m_target.c_str(), m_message.c_str());
	}

	TopicJob::TopicJob(	Plugin *plugin,
						INetworkManagerFrontend &network,
						const std::string &nick,
						const std::string &user,
						const std::string &host,
						const std::string &channel,
						const std::string &topic):
	PluginJob(plugin),
	m_network(network),
	m_nick(nick),
	m_user(user),
	m_host(host),
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
		f(m_network, m_nick.c_str(), m_user.c_str(), m_host.c_str(),
			m_channel.c_str(), m_topic.c_str());
	}

} // namespace firc
} // namespace anp
