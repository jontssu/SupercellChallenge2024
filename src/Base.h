#pragma once

#include "Rectangle.h"
#include "Constants.h"

class Game;
class Weapon;

class Base : public Rectangle
{
public:
	Base(Game* pGame);
	virtual ~Base() {}

	bool takeDamage(float damage);

	bool initialise();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update(float deltaTime);

	void increaseMaxHealth(float amount) { m_maxHealth += amount; }
	void increaseDamage(float amount) { m_damage += amount; }

	void unlockBase(bool unlock) { m_isUnlocked = unlock; }

	bool isDestroyed() const { return m_isDestroyed; }
	void setIsDestroyed(bool isDestroyed) { m_isDestroyed = BaseImmunity == true ? false : isDestroyed; }

	Weapon* getWeapon() { return m_pWeapon.get(); }

private:
	Game* m_pGame;
	std::unique_ptr<Weapon> m_pWeapon;
	
	bool m_isDestroyed;
	bool m_isUnlocked = false;

	float m_flashTimer = 0.0f;
	sf::Color m_normalColor = sf::Color::Green;

	float m_maxHealth;
	float m_hpBarWidth = 100.0f;
	float m_hpBarHeight = 10.0f;
	sf::Color m_hpBarColor = sf::Color::Red;

	float m_damage = BaseDamage;
};
