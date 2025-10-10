/* #include "Bullet.h"
#include "Constants.h"

Bullet::Bullet() : Rectangle(sf::Vector2f(0, 0))
{
    setPosition(sf::Vector2f(ScreenWidth * 0.5f, ScreenHeight * 0.5f));
    setOrigin(sf::Vector2f(0.0f, 0.0f));
    setColor(sf::Color::Red);
}

void Bullet::update(float deltaTime)
{

} */

#include "Bullet.h"
#include "Constants.h"

Bullet::Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, float speed, float lifetime)
    : Rectangle(sf::Vector2f(10.f, 4.f)), m_lifetime(lifetime)
{
    setPosition(position);
    setColor(sf::Color::Red);
    m_velocity = direction * speed;
}

void Bullet::update(float deltaTime)
{
    // Move bullet
    move(m_velocity * deltaTime);

    // Countdown lifetime
    m_lifetime -= deltaTime;
    if (m_lifetime <= 0.f)
        m_isDead = true;

    // Optional: mark as dead if off-screen
    sf::Vector2f pos = getPosition();
    if (pos.x < 0 || pos.x > ScreenWidth || pos.y < 0 || pos.y > ScreenHeight)
        m_isDead = true;
}

void Bullet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape shape(getSize());
    shape.setFillColor(getColor());
    shape.setPosition(getPosition());
    target.draw(shape, states);
}
