/**
 * @file plugin_jobs.h
 * Private header defining a number of Job subclasses.
 */

#ifndef _PLUGIN_JOBS_H_
#define _PLUGIN_JOBS_H_

#include <basedefs.h>
#include <anp_jobqueue.h>
#include <plugin_functions.h>

#include <string>

namespace anp
{
namespace firc
{
	class Plugin;
	
	class PluginJob: public threading::Job
	{
	public:
		PluginJob(Plugin *plugin);
		virtual ~PluginJob();
	protected:
		Plugin *m_plugin;
	};
	
	class JoinJob: public PluginJob
	{
		JoinJob(Plugin *plugin,
				PF_irc_onJoin,
				void *network,
				const int8 *channel,
				const int8 *user);
		virtual ~JoinJob();
	protected:
		PF_irc_onJoin m_func;
		void *m_network;
		std::string m_channel;
		std::string m_user;
		
		void executeCustom();
	};
	
	class PrivMsgJob: public PluginJob
	{
	public:
		PrivMsgJob(Plugin *plugin,
					PF_irc_onPrivMsg func,
					void *network,
					const int8 *sender,
					const int8 *target,
					const int8 *message);
		
		virtual ~PrivMsgJob();
	protected:
		PF_irc_onPrivMsg m_func;
		void *m_network;
		std::string m_sender;
		std::string m_target;
		std::string m_message;

		void executeCustom();
	};
} // namespace firc
} // namespace anp

#endif // _PLUGIN_JOBS_H_
