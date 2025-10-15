#pragma once

#include "Rectangle.h"
#include "Constants.h"

class Game;

class Vampire : public Rectangle
{
public:
    Vampire(Game* game, sf::Vector2f position);
    virtual ~Vampire() {}
    
    void update(float deltaTime);

    void setIsKilled(bool isKilled) { m_isKilled = isKilled; }
    bool isKilled() { return m_isKilled; }

    float getDifficultyScaling() const { return m_difficultyScaling; }
    void setDifficultyScaling(float scale) { m_difficultyScaling *= scale; }

private:
    float m_vampireHp = VampireBaseHealth;
    float m_vampireDamage = VampireBaseDamage;
    float m_vampireSpeed = VampireSpeed;
    float m_vampireAttackCooldown = 0.0f;
    float m_difficultyScaling = 1.0f;

    bool m_isKilled = false;

    Game* m_pGame;
};
