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

    float getDifficultyScaling() const { return difficultyScaling; }
    void setDifficultyScaling(float scale) { difficultyScaling *= scale; }

private:
    float vampireDamage = VampireBaseDamage;
    float vampireAttackCooldown = 0.0f;
    float difficultyScaling = 1.0f;
    Game* m_pGame;
    bool m_isKilled = false;
};
