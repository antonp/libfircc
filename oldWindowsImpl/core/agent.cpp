#include "agent.h"
#include "tcpconnection.h"
//#include "outputprocessor.h"
#include "eventprocessor.h"
#include "optionmanager.h"
#include "messagereceiver.h"
#include "messagesender.h"
#include "pluginmanager.h"
#include "../../settingtreelib/settingtreeutility.h"

/*
COutputHelper &operator<<(COutputHelper &o, const std::string &str) {
	o.m_out.addOutput(str, o.m_sourceID);
	return o;
}

COutputHelper &operator<<(COutputHelper &o, const char *str) {
	o.m_out.addOutput(str, o.m_sourceID);
	return o;
}

COutputHelper &operator<<(COutputHelper &o, char ch) {
	std::string str;
	str = ch;
	o.m_out.addOutput(str, o.m_sourceID);
	return o;
}*/

CAgent::CAgent(CEventProcessor *eventHandler,
			   const std::string &nick,
			   const std::string &realname,
			   settingtree::CGroupNode *agentRoot,
			   int agentID):
m_connection(NULL),
m_agentRoot(agentRoot),
m_eventHandler(*eventHandler),
m_nickname(nick),
m_realname(realname),
m_messageReceiver(NULL),
m_messageSender(NULL),
m_pluginManager(NULL),
m_agentID(agentID) {
	std::string server_address, channel_file, user_file, plugin_file;
	int server_port = 0;

	m_agentRoot->getString("server_address", server_address);
	m_agentRoot->getInt("server_port", server_port);
	m_agentRoot->getString("channel_file", channel_file);
	m_agentRoot->getString("user_file", user_file);
	m_agentRoot->getString("plugin_file", plugin_file);
	m_label = *m_agentRoot->getLabel();

	m_connection = new CTCPConnection(server_address, server_port);
	loadChannels(channel_file);
	loadAdmins(user_file);

	std::stringstream s;
	s << "Connecting to " << server_address << ':' << server_port << '\n';

	m_eventHandler.addEvent(agentID, -1, -1, s.str());
//	m_out << "Connecting to " << server_address
//		<< ':' << server_port << "...\n";

	bool connected = registerConnection();
	m_cache.init("TODO!!!", m_nickname);

	if ( connected ) {
		m_messageSender = new CMessageSender(m_connection, eventHandler, agentID);
		m_pluginManager = new CPluginManager(*m_messageSender, *eventHandler, agentID, plugin_file.c_str(), &m_cache);
		m_messageReceiver = new CMessageReceiver(m_connection, eventHandler, m_pluginManager, agentID, &m_cache);

		// Join some channels
		//m_out << "Joining channels...\n";
		m_eventHandler.addEvent(agentID, -1, -1, "Joining channels...\n");

		for ( unsigned int i=0; i<m_channelsToJoin.size(); i++ ) {
			m_messageSender->sendMessage("JOIN "+m_channelsToJoin.at(i)+"\r\n");
		}
	}
}

CAgent::~CAgent() {
	//m_out << "entering CAgent's destructor\n";
	deinit();
}

void CAgent::getLabel(std::string &dest) {
	dest = m_label;
}

bool CAgent::init() {
//	m_messageReceiver = new CMessageReceiver(m_connection);
//	m_actionTaker = new CActionTaker(m_connection);

	return true;
}

void CAgent::deinit() {
	if ( m_pluginManager != NULL ) {
		delete m_pluginManager;
		m_pluginManager = NULL;
	}
	if ( m_messageReceiver != NULL ) {
		if ( m_messageSender != NULL )
			m_messageSender->waitForLastMessage();
		m_messageReceiver->kill();
		delete m_messageReceiver;
		m_messageReceiver = NULL;
	}
	if ( m_messageSender != NULL ) {
		m_messageSender->kill();
		delete m_messageSender;
		m_messageSender = NULL;
	}
	if ( m_connection != NULL ) {
		m_connection->CloseSocket();
		m_connection->Clean();
		delete m_connection;
		m_connection = NULL;
	}
}

bool CAgent::loadChannels(const std::string &channelFile) {
	std::ifstream fin;
	std::string line;
	char buffer[100];

	fin.open(channelFile.c_str(), std::ios_base::in);
	if ( !fin.is_open() )
		return false;

	while ( !fin.eof() ) {
		fin.getline(buffer, 100);
		line = buffer;
		if ( line.find('#') == std::string::npos ) {
			line.insert(0, "#");
		}
		m_channelsToJoin.push_back(line);
	}

	// We're done with the file
	fin.close();
	return true;
}

bool CAgent::loadAdmins(const std::string &adminFile) {
	std::ifstream fin;
	char buffer[100];

	fin.open(adminFile.c_str(), std::ios_base::in);
	if ( !fin.is_open() )
		return false;

	while ( !fin.eof() ) {
		fin.getline(buffer, 100);
		parseAdminInfo(buffer);
	}

	// We're done with the file
	fin.close();
	return true;
}

void CAgent::parseAdminInfo(std::string line) {
	using namespace std;

	/*
		TODO: Check for invalid entries.. perhaps rewrite this part with the help of a tokenizer
	*/
	AdminData temp;
	string::size_type pos;
	string del = ":";

	// First we want the username...
	pos = line.find_first_of(del);
	temp.user = line.substr(0, pos);
	line.erase(0, pos+1);
	// Then we want the password...
	pos = line.find_first_of(del);
	temp.password = line.substr(0, pos);
	line.erase(0, pos+1);
	// And finally the access level
//	temp.access = ( s == "normal" ? 1 : s == "privileged" ? 2 : s == "owner" ? 3 : 0 );
	temp.access = atoi(line.c_str());
//	DEBUGMSG("Added user '" << temp.user << "' with password '" << temp.password << "' and access " << temp.access);

	// Set the default values
	temp.currNick = temp.currHost = "unknown";
	temp.isOn = false;

	m_adminRegistry.push_back(temp);
}

bool CAgent::registerConnection() {
	std::string in, cmd;
	std::string::size_type pos;
	char in_raw[MAX_DATA_LENGTH];

	if ( N_ERROR_CONNECTION_FAILED == m_connection->Connect() ) {
		// Ok what to do now?
		//m_out << "Failed to connect... !%#£€@=!(%\n!";
		m_eventHandler.addEvent(m_agentID, -1, -1, "Failed to connect...");
		return false;
	}
//	INFOMSG("Sending info...");
	//m_out->addOutput("Sending info...\n");
	//m_out << "Sending info...\n";

//	con::adjustCursor(1);
	// Nick
	m_outStr = "NICK "+m_nickname+"\r\n";
	m_connection->Send(m_outStr);
	// User
	m_outStr = "USER "+m_nickname+" 0 * :"+m_realname+"\r\n";
	m_connection->Send(m_outStr);
	// Handle the first ping command
	cmd = "PING";

	ZeroMemory(in_raw, MAX_DATA_LENGTH);
	while ( m_connection->Receive(in_raw) == N_OK ) {
		in = in_raw; ZeroMemory(in_raw, MAX_DATA_LENGTH);
		std::string tempDbgOutput = "CAgent::registerConnection() received: "; tempDbgOutput += in;
		m_eventHandler.addEvent(m_agentID, -1, -1, tempDbgOutput);

//		DEBUGMSG("SERVER: " << in);
//		m_out << "SERVER: " << in;
		if ( pos = in.find(cmd,  0) != std::string::npos ) {
			// "PONG" was found, let's find our id
			cmd = ":";
			pos = in.find_last_of(cmd);
			in.erase(0, pos+1);
			m_pongStr = "PONG "+in+"\r\n";
			// Send the pong command with our id
			m_connection->Send(m_pongStr);

//			m_out << "Sent: " << m_pongStr;
			break;
		}
	}
	if ( m_pongStr.length() == 0 ) {
//		m_out << "Error: Failed to receive the first ping command.\n";
	} else {
//		m_out << "OK\n";
	}
		
	// Perhaps treat this as OnConnect()
	return true;

}

int CAgent::join(const std::string &channel) {
	m_outStr = "JOIN "+channel+"\r\n";
	m_connection->Send(m_outStr);

	return 0;
}