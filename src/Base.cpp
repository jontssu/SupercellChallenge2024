#include "Base.h"
#include "Game.h"
#include "Constants.h"
#include "Weapon.h"
#include <iostream>

Base::Base(Game* pGame) :
	Rectangle(sf::Vector2f(BaseWidth, BaseHeight)),
	m_pGame(pGame),
	m_pWeapon(std::make_unique<Weapon>(pGame))
{
    setOrigin(sf::Vector2f(BaseWidth * 0.5f, BaseHeight * 0.5f));
    setPosition(sf::Vector2f(ScreenWidth * 0.5f, ScreenHeight * 0.5f));
    setSize(sf::Vector2f(BaseWidth, BaseHeight));
    setColor(sf::Color::Green);
}	

bool Base::initialise()
{
	m_sprite.setTexture(*m_pGame->getBaseTexture());
	m_sprite.setScale(3.0f, 3.0f);
	m_maxHealth = BaseStartingHealth;
	m_isDestroyed = false;
	// Set weapon stats for base
	m_pWeapon->setFireRate(BaseFireRate);
	m_pWeapon->setBulletDamage(BaseDamage);
	m_pWeapon->setBulletPierce(BasePierceCount);
	return true;
}   

void Base::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Draw base
    sf::RectangleShape graphicsRect(getSize());
    graphicsRect.setFillColor(getColor());
    graphicsRect.setOrigin(getSize().x / 2, getSize().y / 2);
    graphicsRect.setPosition(getPosition());
    target.draw(graphicsRect, states);

	//Draw health bar
	float healthPercent = m_maxHealth / BaseStartingHealth;
	if (healthPercent < 0.0f)
		healthPercent = 0.0f;
	sf::RectangleShape healthBarBack(sf::Vector2f(m_hpBarWidth * healthPercent, m_hpBarHeight));
	healthBarBack.setFillColor(m_hpBarColor);
	healthBarBack.setPosition(getPosition().x - m_hpBarWidth / 2, getPosition().y - getSize().y / 2 - 10 - 10);
	target.draw(healthBarBack, states);	

	if (!m_isUnlocked)
		return;
	m_pWeapon->setPosition(getPosition());
	m_pWeapon->draw(target, states);
}

void Base::update(float deltaTime)
{
	if (m_flashTimer > 0.0f) {
		m_flashTimer -= deltaTime;
		if (m_flashTimer <= 0.0f) {
			setColor(m_normalColor);
		}
	}

	if (!m_isUnlocked)
		return;

	// Update weapon position to base center
	sf::Vector2f weaponSize = m_pWeapon->getSize();
	m_pWeapon->setPosition(sf::Vector2f(
		getCenter().x - weaponSize.x / 2.0f,
		getCenter().y - weaponSize.y / 2.0f));
	m_pWeapon->update(deltaTime);
}

bool Base::takeDamage(float damage)
{
	setColor(sf::Color::Red);
	m_flashTimer = 0.2f; // flash for 0.2 seconds
	m_maxHealth -= damage;
	if (m_maxHealth <= 0.0f)
		return true; // Base destroyed
	return false;
}
