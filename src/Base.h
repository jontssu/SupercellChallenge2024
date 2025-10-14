#pragma once

#include "Rectangle.h"
#include "Constants.h"

class Game;

class Base : public Rectangle
{
public:
	Base(Game* pGame);
	virtual ~Base() {}

	bool takeDamage(float damage);

	bool initialise();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update(float deltaTime);

	bool isDestroyed() const { return m_isDestroyed; }
	void setIsDestroyed(bool isDestroyed) { m_isDestroyed = BaseImmunity == true ? false : isDestroyed; }

private:
	Game* m_pGame;
	
	bool m_isDestroyed;

	float m_flashTimer = 0.0f;
	sf::Color m_normalColor = sf::Color::Green;

	float m_health;

	float m_hpBarWidth = 100.0f;
	float m_hpBarHeight = 10.0f;
	sf::Color m_hpBarColor = sf::Color::Red;
};
