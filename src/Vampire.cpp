#include "Vampire.h"
#include "Constants.h"
#include "Game.h"
#include "Weapon.h"
#include "Player.h"
#include "MathUtils.h"
#include "Base.h"
#include "Bullet.h"

Vampire::Vampire(Game* game, sf::Vector2f position) :
    Rectangle(sf::Vector2f(VampireWidth, VampireHeight)),
    m_pGame(game)
{
    setPosition(position);
    setOrigin(sf::Vector2f(0.0f, 0.0f));
    setIsKilled(false);

    m_sprite.setTexture(*m_pGame->getVampireTexture());
    m_sprite.setScale(2.0f, 2.0f);
}

void Vampire::update(float deltaTime)
{
    if (m_isKilled)
        return;
    
    Player* pPlayer = m_pGame->getPlayer();
    Base* pBase = m_pGame->getBase();

    if (collidesWith(pPlayer->getWeapon()))
    {
        setIsKilled(true);
        return;
    }

    // Check collision with bullets
    for (const auto& bullet : pPlayer->getWeapon()->getBullets()) {
        if (collidesWith(bullet.get())) {
            if (bullet->getPierceCount() > 0) {
                setIsKilled(true);
                bullet->setPierceCount(bullet->getPierceCount() - 1);
            }
            if (bullet->getPierceCount() <= 0) {
                bullet->setDead(true);
            }
            break;
        }
    }

    if (collidesWith(pPlayer))
        pPlayer->setIsDead(true);
    if (collidesWith(pBase)) {
        if (vampireAttackCooldown <= 0.0f) {
            if (pBase->takeDamage(vampireDamage))
                pBase->setIsDestroyed(true);
            vampireAttackCooldown = 1.0f; // Attack cooldown of 1 second
        } else {
            vampireAttackCooldown -= deltaTime;
        }   
    }
    else
    {
        pBase->setColor(sf::Color::Green);
    }

    // Move towards player if closer than base, else move towards base
    sf::Vector2f playerCenter = pPlayer->getCenter();
    sf::Vector2f baseCenter = pBase->getCenter();
    sf::Vector2f targetCenter;
    float distToPlayer = VecLength(playerCenter - getCenter());
    float distToBase = VecLength(baseCenter - getCenter());
    if (distToPlayer < distToBase)
        targetCenter = pPlayer->getCenter();
    else
        targetCenter = pBase->getCenter();

    // Anti-stacking: push away from nearby vampires
    for (const auto& other : m_pGame->getVampires()) {
        if (other.get() == this || other->isKilled()) continue;
        sf::Vector2f toOther = getCenter() - other->getCenter();
        float dist = VecLength(toOther);
        float minDist = VampireWidth * 0.8f; // threshold for repulsion
        if (dist < minDist && dist > 0.01f) {
            sf::Vector2f repulse = VecNormalized(toOther) * ((minDist - dist) * 0.5f);
            sf::Transformable::move(repulse);
        }
    }

    // Move towards target
    sf::Vector2f direction = VecNormalized(targetCenter - getCenter());
    direction *= VampireSpeed * deltaTime;
    sf::Transformable::move(direction);
    m_sprite.setPosition(getPosition());
}