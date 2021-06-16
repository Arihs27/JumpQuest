#include <LobbyState.h>
#include <Server.h>
#include <iostream>
#include "Resources.h"
#include "MultiplayerMenuState.h"
#include "GameState.h"
#include "StateManager.h"
#include <cstring>
#include "ServerGameState.h"
#include "ClientGameState.h"

LobbyState::LobbyState(StateManager& manager, sf::RenderWindow& window, bool replace, std::shared_ptr<NetworkObject>& net) :
	MenuState(manager, window, replace, net, lobbyTitle, lobbyBackground), m_connected(false), m_isServer(false),
	m_listBackground({ window.getSize().x / 3.f, window.getSize().y / 3.f }), m_signedUp(false),
	m_nameTextBox({ window.getSize().x / 4.f,window.getSize().y / 4.f })/*,
	m_nameList(4)*/
{
	/*m_listBackground.setFillColor(sf::Color(255, 255, 255, 120));
	m_listBackground.setOrigin({ m_listBackground.getSize().x / 2.f, m_listBackground.getSize().y / 2.f });
	m_listBackground.setPosition(window.getSize().x/2, window.getSize().y/2);*/
	
	/*m_nameTextBox.setFillColor(sf::Color(102,0,0,150));
	m_nameTextBox.setOrigin({ m_nameTextBox.getSize().x / 2.f, m_nameTextBox.getSize().y / 2.f });
	m_nameTextBox.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);*/
	createBoxShape(m_listBackground, sf::Color(255, 255, 255, 120));
	createBoxShape(m_nameTextBox, sf::Color(102,0,0,150));
	auto pos = sf::Vector2f{ m_nameTextBox.getPosition().x - (m_nameTextBox.getSize().x / 2) + 10,
		m_nameTextBox.getPosition().y - m_nameTextBox.getSize().y / 2 + 10 };
	m_text = createText(lobbyFont, 24, sf::Color::Black, "Enter your nickname:\n",pos);
	//m_text.setFont(Resources::getResourceRef().getFont(lobbyFont));
	//m_text.setCharacterSize(24);
	//m_text.setFillColor(sf::Color::Black);
	//- (m_nameTextBox.getSize().x / 2)+10
	//m_text.setPosition({ m_nameTextBox.getPosition().x - (m_nameTextBox.getSize().x / 2) + 10,
	//	m_nameTextBox.getPosition().y - m_nameTextBox.getSize().y / 2+10 });
	//m_text.setString("Enter your nickname:\n");
	pos = { m_text.getPosition().x ,m_text.getPosition().y +
		m_text.getGlobalBounds().height + 10 };
	m_inputText = createText(lobbyFont, 24, sf::Color::Black, "", pos);
	/*m_inputText.setFont(Resources::getResourceRef().getFont(lobbyFont));
	m_inputText.setCharacterSize(24);
	m_inputText.setFillColor(sf::Color::Black);
	m_inputText.setPosition({ m_text.getPosition().x ,
		m_text.getPosition().y + m_text.getGlobalBounds().height + 10 });*/
	//m_inputText
	pos = m_nameTextBox.getPosition();
	/*auto org = sf::Vector2f({ m_waitingText.getGlobalBounds().width / 2.f,
			m_waitingText.getGlobalBounds().height / 2.f });*/
	m_waitingText = createText(lobbyFont,50,sf::Color::Black,"Waiting for Host\n",pos,true);
	//for waiting
	/*m_waitingText.setFont(Resources::getResourceRef().getFont(lobbyFont));
	m_waitingText.setCharacterSize(50);
	m_waitingText.setFillColor(sf::Color::Black);
	m_waitingText.setString("Waiting for Host\n");
	m_waitingText.setOrigin({ m_waitingText.getGlobalBounds().width / 2.f, m_waitingText.getGlobalBounds().height / 2.f });
	m_waitingText.setPosition(m_nameTextBox.getPosition());*/

	float width = Resources::getResourceRef().getButLen(back) * PIX4LET * 1.3;
	pos = { width, m_window.getSize().y - m_window.getSize().y / 10.f };
	//sf::Vector2f pos = { 0,0 };
	float butHeight = m_window.getSize().y - pos.y;
	addButton<MultiplayerMenuState>(back, pos, width, butHeight);
	//build prompt
	if ( typeid(*m_networkObj.get()).name() == typeid(Server).name()){
		m_isServer = true;	
		std::cout << "is server\n";
		width= Resources::getResourceRef().getButLen(start)* PIX4LET * 1.3;
		pos.x = m_window.getSize().x - width;
		addButton<ServerGameState>(start, pos, width, butHeight);
		m_connected = m_networkObj->launch();
	}
	setNameListText();
}

void LobbyState::update(){
	if (!m_connected) {
		if (!m_networkObj->launch())
			m_networkObj->handleRequests(10);
		else
			m_connected = true;
		MenuState::update();
		return;
	}
	if (!m_signedUp)
		signUp();
	if (m_connected && m_signedUp)
		MenuState::update();
	if (m_networkObj->handleRequests())
		updateList();
	if (m_networkObj->getStarted()) {
		if (m_isServer)
			m_next = StateManager::build<ServerGameState>(m_manager, m_window, true, m_networkObj);
		else
			m_next = StateManager::build<ClientGameState>(m_manager, m_window, true, m_networkObj);
		return;
	}
}

void LobbyState::signUp() {
	for (auto event = sf::Event{}; m_window.pollEvent(event);) {
		if (event.type == sf::Event::TextEntered) {
			//auto key=
			if (event.text.unicode < 128) {
				//event.text.unicode == sf::Keyboard::BackSpace;
			//	if (sf::Keyboard::isKeyPressed( sf::Keyboard::BackSpace))
				if (event.text.unicode == 8) {
					if (m_inputStr.size() > 0)
						m_inputStr.pop_back();
				}
				else if (m_inputText.getString().getSize() < PLAYER_NAME_LEN) 
						m_inputStr += event.text.unicode;
				m_inputText.setString(m_inputStr);
				
			}
		}
		else if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Enter) {
				m_networkObj->setName(m_inputStr.c_str());
				m_signedUp = true;
				updateList();
				break;
			}
		}
	}
}

void LobbyState::draw() {
	MenuState::draw();
	if (!m_connected) {
		m_window.draw(m_waitingText);
		return;
	}
	m_window.draw(m_listBackground);
	for (auto& name : m_nameList)
		m_window.draw(name);
	if(!m_signedUp){
		m_window.draw(m_nameTextBox);
		m_window.draw(m_text);
		m_window.draw(m_inputText);
	}
}

void LobbyState::updateList(){
	auto it = m_nameList.begin();
	for (int i = 0 ; i < MAX_SERVER_PLAYERS ; ++i) {
		if (m_networkObj->getMembers(i)) {
			auto str = std::string(m_networkObj->getMembers(i)->m_name);
			if (str != "") {
				it->setString(str);
				++it;
			}
		}
	}
	std::for_each(it, m_nameList.end(), [&](sf::Text&) { it->setString(""); });
}
void LobbyState::updateNextState(const sf::Vector2f& loc) {
	if (m_buttons[0]->checkCollision(loc)) {
		m_next = m_buttons[0]->ButtonState(m_manager, m_window, true, m_networkObj);
	}
	if (m_isServer && m_buttons[1]->checkCollision(loc)) {
		static_cast<Server*>(m_networkObj.get())->startGame();
		m_next = m_buttons[1]->ButtonState(m_manager, m_window, true, m_networkObj);
	}
}

void LobbyState::drawList(){
	for (auto& name : m_nameList) {
		m_window.draw(name);
	}
}
void LobbyState::setNameListText() {
	auto textHeight = (m_listBackground.getSize().y - 10 * MAX_LIST_NAMES_SIZE) / MAX_LIST_NAMES_SIZE;
	auto startPos = sf::Vector2f{
		m_listBackground.getPosition().x - m_listBackground.getSize().x / 2 + 10,
		m_listBackground.getPosition().y - m_listBackground.getSize().y / 2 + 10 };
	for (int i = 0; i < MAX_LIST_NAMES_SIZE; i++) {
		sf::Text text;
		text.setFont(Resources::getResourceRef().getFont(lobbyFont));
		text.setCharacterSize(24);
		text.setPosition(startPos);
		m_nameList.emplace_back(std::move(text));
		startPos.y += textHeight + 10;
	}
}
void LobbyState::createBoxShape(sf::RectangleShape& rec,const sf::Color& clr){
	rec.setFillColor(clr);
	rec.setOrigin({ rec.getSize().x / 2.f, rec.getSize().y / 2.f });
	rec.setPosition(m_window.getSize().x / 2.f,m_window.getSize().y / 2.f);
}
sf::Text LobbyState::createText(int font, int size,const sf::Color& clr, const std::string& str,
		const sf::Vector2f& pos, bool org){
	sf::Text txt;
	txt.setFont(Resources::getResourceRef().getFont(font));
	txt.setCharacterSize(size);
	txt.setString(str);
	txt.setFillColor(clr);
	if (org)
		txt.setOrigin({ txt.getGlobalBounds().width / 2.f, txt.getGlobalBounds().height / 2.f });
	txt.setPosition(pos);
	return std::move(txt);
}