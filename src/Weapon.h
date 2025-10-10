#pragma once

#include "Rectangle.h"
#include "Game.h"
#include "Bullet.h"

class Weapon : public Rectangle
{
public:
    Weapon();
    virtual ~Weapon() {}

    bool initialise(Game* pGame);
    void setActive(bool isActive);
    void update(float deltaTime);
    bool isActive() { return m_isActive; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    bool m_isActive = false;
    float m_timer = 0.0f;

    std::vector<std::unique_ptr<Bullet>> m_pBullets;
};