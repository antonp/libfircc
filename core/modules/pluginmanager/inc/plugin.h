#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <basedefs.h>
#include <anp_dynamic_library.h>
#include "plugin_functions.h"

#include <string>

namespace anp
{
namespace firc
{
	class Plugin
	{
	public:
		Plugin(
			const int8 *fileName,
			anp::EventDispatcher<
				events::ISubscriber<events::NewSession>,
				events::NewSession
			> &newSessionDispatcher,
			void *appContext
		);
		~Plugin();
		
		void setUnloadReason(uint32 reason);
		void setUnloading(bool32 unloading);
		bool32 isUnloading() const;
		
		const std::string &getName() const;
		
		// Event handler get functions
		Result getEventHandler(PF_irc_onJoin *dest) const;
		Result getEventHandler(PF_irc_onPrivMsg *dest) const;
		
		// Execution, status, reloading
		void increaseExecutionCount();
		void decreaseExecutionCount();
	private:	
		DynamicLibrary		m_lib;
		
		PF_pluginDeinit		m_pf_pluginDeinit;
		
		// Pure irc functions
		PF_irc_onJoin		m_pf_irc_onJoin;
		PF_irc_onPrivMsg	m_pf_irc_onPrivMsg;
		
		// Info
		std::string			m_name;
		
		// Execution
		uint32				m_executionCount;
		bool32				m_unloading;
		uint32				m_unloadReason;
		
	};
} // namespace firc
} // namespace anp

#endif // _PLUGIN_H_
