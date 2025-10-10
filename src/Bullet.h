#pragma once
#include "Rectangle.h"
#include <SFML/Graphics.hpp>

class Bullet : public Rectangle
{
public:
    Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, float speed = 800.f, float lifetime = 2.f);
    virtual ~Bullet() {}

    void update(float deltaTime);

    bool isDead() const { return m_isDead; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Vector2f m_velocity;
    float m_lifetime;
    bool m_isDead = false;
};
