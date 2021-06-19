#include <Client.h>
#include <iostream>
#include <Board.h>

Client::Client() : NetworkObject(), m_serverIP(), m_servers(), 
m_isLinked(false){
	m_packet.clear();
}
//============================================================================
Client::~Client() {
	notifyClosing();
}

/*============================================================================
* The method is receiving all the messeges the client received and handle them
* as needed.
*/
bool Client::handleRequests(int max) {
	int counter = 0;
	while (receivedMessege()&& counter++ < max) {
		//std::cout << "udp messege received.\n";
			switch (receiveValue<Messege_type>())
			{
			case networkMessege:
				switch (receiveValue<Network_messeges>()){
				case iAmFree:
					sendGameMembership("client");
					break;
				case startGame:
					setStarted(true);
					break;
				case closing:
					throw std::exception(SERVER_CONNECTION_LOST);
					break;
				default:
					break;
				}
				break;
			case memberId:
				regesterServer();
				break;
			case addMember:
				addMemberToList();
				break;
			case memberInfo:
				updateMember(receiveValue<MemberInfo>());
				break;
			case movingObj:
				updateMovingObj();
				break;
			case staticObjInfo:
				getBoard()->updateStaticMsgCollision(receiveValue<StaticObjInfo>().m_index);
				break;
			case closer:
				setMember(receiveValue<int>(), nullptr);
			default:
				break;
			}
	}
	if (counter == 0)
		return false;
	return true;
}
/*============================================================================
* The method send to all the avaible servers message informing them that
* the Client is searchig for hosting Server.
* to receive the servers answer run handleRequests method.
*/
void Client::searchForServers() {
	m_packet.clear();
	sendMessege(networkMessege, whoIsFreeServer, 
		sf::IpAddress::Broadcast, SERVERS_PORT);
	m_packet.clear();
}
/*============================================================================
* The method notify the host Server that the client is disconnecting.
*/
void Client::notifyClosing() {
	sendMessege<int>(closer, getInfo().m_info.m_id, m_serverIP, SERVERS_PORT);
}
/*==========================================================================*/
void Client::updateLoc( const MemberInfo& member){
	sendMessege<MemberInfo>(memberInfo, member, m_serverIP, SERVERS_PORT);
}
/*==========================================================================*/
bool Client::launch()
{
	if (!receivedMessege()){
		searchForServers();
	}
	return getInfo().m_info.m_id != 0;
}
/*============================================================================
* The method is regester to the server the client reseived messege from the last.
*/
void Client::regesterServer() {
	setId(receiveValue<int>());
	m_serverIP = getSenderIP();
	m_isLinked = true;
}
/*============================================================================
* The method sets a new name for the client and singIn to the server.
*/
void Client::setName(const char name[PLAYER_NAME_LEN], int index) {
	NetworkObject::setName(name);
	sendMessege<GameMember>(singMeIn, getInfo());
}
/*============================================================================
* The method is notify the server about collision with static obj
*/
void Client::sendStaticCollision(int index){
	sendMessege<StaticObjInfo>(staticObjInfo, 
		staticObjInfoCreator(getInfo().m_info.m_id, index));
}
/*============================================================================
* The method is update the Board's moving objects as the server reported.
*/
void Client::updateMovingObj() {
	MovingObjMembersRoport messege = receiveValue<MovingObjMembersRoport>();
	for (int i = 0; i < messege.m_size; ++i)
		getBoard()->setInfo(i + 1, messege.m_locs[i]);
}
/*============================================================================
* The method is singIn to the server.
*/
void Client::sendGameMembership(const char name[]) {
	sendMessege<GameMember>(singMeIn, gameMemberCreator(getIP(), getPort(), name));
}