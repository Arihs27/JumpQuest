#include "ClientGameState.h"
#include "Client.h"


//-----------------------------------------------------------------------------
void ClientGameState::updateNetwork() {
	//((Client*)m_networkObj.get())->sendIAmReady();
	m_networkObj->handleRequests(300);
	//if (m_networkObj->gameStarted())
		m_started = true;
}
