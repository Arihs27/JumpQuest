#pragma once
#include "StaticObj.h"
class b2Body;
class b2World;

class Block : public StaticObj {
public:
	Block() = default;
	Block(b2World&, const sf::Vector2f&, const sf::Vector2f&, int,int);
	void updateAnim(float deltaTime);
	virtual ~Block() = default;
private:
	static bool m_registerit;
};

