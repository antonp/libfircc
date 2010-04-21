#ifndef _APPEVENTS_H_
#define _APPEVENTS_H_

#include <networkmanager_frontend.h>

namespace app
{
	namespace events
	{
		class NewSession
		{
		public:
			NewSession(anp::firc::INetworkManagerFrontend &session):
			m_session(session)
			{
			}

			anp::firc::INetworkManagerFrontend &session()
			{
				return m_session;
			}
		protected:
			anp::firc::INetworkManagerFrontend &m_session;
		};
	}
}

#endif // _APPEVENTS_H_
