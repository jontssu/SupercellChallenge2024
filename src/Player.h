#pragma once

#include "Rectangle.h"
#include "Constants.h"
#include "Weapon.h"

#include <memory>

struct InputData;

class Game;
class Weapon;

enum eDirection
{
    LEFT,
    RIGHT
};

class Player : public Rectangle
{
public:
    Player(Game* pGame);
    virtual ~Player() {}
    
    bool initialise();
    void move(InputData inputData, float deltaTime);
    void attack();
    void update(float deltaTime);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void heal(float amount) { m_currentHealth = m_currentHealth + amount; }
    void increaseMaxHealth(float amount) { m_maxHealth += amount; m_currentHealth += amount; }
    float getMaxHealth() const { return m_maxHealth; }
    float getCurrentHealth() const { return m_currentHealth; }

    void increaseDamage(float amount) { m_pWeapon->increaseDamage(amount); }

    void increaseSpeed(float amount) { m_playerSpeed += amount; }
    float getSpeed() const { return m_playerSpeed; }

    bool isDead() const { return m_isDead; }
    void setIsDead(bool isDead) { m_isDead = PlayerImmunity == true ? false : isDead; }

    Weapon* getWeapon() { return m_pWeapon.get(); }
    eDirection getDirection() const { return m_direction; }

private:
    float   m_maxHealth = PlayerMaxHealth;
    float   m_currentHealth = m_maxHealth;
    float   m_playerSpeed = PlayerSpeed;
    bool    m_isDead = false;

    eDirection m_direction = LEFT;

    Game*   m_pGame;
    std::unique_ptr<Weapon> m_pWeapon;
};
