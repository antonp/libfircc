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
		
		void setPlugin(Plugin *plugin);
		void setFunc(void *func);
	protected:
		Plugin *m_plugin;
		void *m_func;
	};
	
	class JoinJob: public PluginJob
	{
	public:
		JoinJob(Plugin *plugin,
				void *network,
				const int8 *channel,
				const int8 *user);
		virtual ~JoinJob();
	protected:	
		void *m_network;
		std::string m_channel;
		std::string m_user;
		
		void executeCustom();
	};
	
	class PrivMsgJob: public PluginJob
	{
	public:
		PrivMsgJob(Plugin *plugin,
					INetworkManagerFrontend &network,
					const int8 *sender,
					const int8 *target,
					const int8 *message);
		
		virtual ~PrivMsgJob();
	protected:
		INetworkManagerFrontend &m_network;
		std::string m_sender;
		std::string m_target;
		std::string m_message;

		void executeCustom();
	};
} // namespace firc
} // namespace anp

#endif // _PLUGIN_JOBS_H_
