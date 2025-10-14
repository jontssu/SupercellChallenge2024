#include "Weapon.h"
#include "Constants.h"
#include "Player.h"
#include "Vampire.h"
#include <iostream>

Weapon::Weapon(Game* pGame) : Rectangle(sf::Vector2f(0, 0)), m_pGame(pGame)
{
    // std::cout << "Weapon constructed, m_pGame: " << m_pGame << std::endl;
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
    if (DisableWeapons)
        return;
    if (DisableLaser)
    {
        
    }
    else if (!m_isActive)
    {
        m_laserCooldown -= deltaTime;
        if (m_laserCooldown <= 0.0f)
            setActive(true);
    }
    else
    {
        m_laserActiveTime -= deltaTime;
        if (m_laserActiveTime <= 0.0f)
        {
            setActive(false);
            m_laserActiveTime = WeaponLaserActiveTime;
            m_laserCooldown = WeaponLaserCoolDown;
        }
    }


    if (DisableGun)
        return; 
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
    if (m_bulletCooldown > 1.0f)
    {
        m_bulletCooldown -= deltaTime;
        return;
    }

    sf::Vector2f weaponPos = getPosition();
    float minDistSq = std::numeric_limits<float>::max();
    Vampire* closestVamp = nullptr;
    if (m_pGame) {
        for (const auto& vampPtr : m_pGame->getVampires()) {
            if (vampPtr->isKilled()) continue;
            sf::Vector2f vampPos = vampPtr->getCenter();
            float distSq = (vampPos.x - weaponPos.x) * (vampPos.x - weaponPos.x) + (vampPos.y - weaponPos.y) * (vampPos.y - weaponPos.y);
            if (distSq < minDistSq) {
                minDistSq = distSq;
                closestVamp = vampPtr.get();
            }
        }
        if (closestVamp) {
            sf::Vector2f vampPos = closestVamp->getCenter();
            sf::Vector2f dir = vampPos - weaponPos;
            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            sf::Vector2f bulletDir(0.f, -1.f); // default upward
            if (len > 0.01f)
                bulletDir = dir / len;
            m_pBullets.push_back(std::make_unique<Bullet>(weaponPos, bulletDir, m_bulletSpeed));
            m_bulletCooldown = m_nextBulletCooldown;
        }
    }
}