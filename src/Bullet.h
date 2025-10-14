#pragma once
#include "Rectangle.h"
#include <SFML/Graphics.hpp>
#include "Constants.h"

class Bullet : public Rectangle
{
public:
    Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, float speed = 800.f, float lifetime = 2.f);
    virtual ~Bullet() {}

    void update(float deltaTime);

    bool isDead() const { return m_isDead; }
    void setDead(bool dead) { m_isDead = dead; }

    int getPierceCount() const { return m_pierceCount; }
    void setPierceCount(int count) { m_pierceCount = count; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Vector2f m_velocity;
    float m_lifetime;
    bool m_isDead = false;
    int m_pierceCount = WeaponPierceCount;
};
