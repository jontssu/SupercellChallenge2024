#include "Weapon.h"
#include "Constants.h"
#include "Player.h"

Weapon::Weapon(Game* pGame) : Rectangle(sf::Vector2f(0, 0)) 
{
    setPosition(sf::Vector2f(ScreenWidth * 0.5f, ScreenHeight * 0.5f));
    setOrigin(sf::Vector2f(0.0f, 0.0f));
    setColor(sf::Color::Blue);
}

void Weapon::setActive(bool isActive)
{
    m_isActive = isActive;
    if (isActive)
    {
        setSize(sf::Vector2f(WeaponWidth, WeaponHeight));
        m_timer = WeaponActiveTime;
    }
    else
    {
        setSize(sf::Vector2f(0.0f, 0.0f));
        m_timer = 0.0f;
    }
}

void Weapon::update(float deltaTime)
{
    if (m_isActive)
    {
        m_timer -= deltaTime;
        if (m_timer <= 0.0f)
        {
            setActive(false);
        }
    }
    //Spawn bullets
    bulletSpawner(deltaTime);

    //Loop update bullets
    for (auto& bullet : m_pBullets)
        bullet->update(deltaTime);

    //Remove dead bullets
    for (int i = 0; i < m_pBullets.size(); i++)
    {
        if (m_pBullets[i]->isDead())
        {
            std::swap(m_pBullets[i], m_pBullets.back());
            m_pBullets.pop_back();
            continue;
        }
    }
}

void Weapon::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::RectangleShape graphicsRect(getSize());
    graphicsRect.setFillColor(getColor());
    graphicsRect.setPosition(getPosition());
    target.draw(graphicsRect);
    //Loop draw all bullets
    for (const auto& bullet : m_pBullets)
        bullet->draw(target, states);
}

void Weapon::bulletSpawner(float deltaTime) 
{
    if (m_bulletCooldown > 0.0f)
    {
        m_bulletCooldown -= deltaTime;
        return;
    }

    sf::Vector2f bulletDir(0.f, -1.f); // shoot upward
    m_pBullets.push_back(std::make_unique<Bullet>(getPosition(), bulletDir));
    m_bulletCooldown = m_nextBulletCooldown;
}