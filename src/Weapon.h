#pragma once

#include "Rectangle.h"
#include "Game.h"
#include "Bullet.h"

class Weapon : public Rectangle
{
public:
    Weapon(Game* pGame);
    virtual ~Weapon() {}

    bool initialise(Game* pGame);
    void setActive(bool isActive);
    void update(float deltaTime);
    bool isActive() { return m_isActive; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void bulletSpawner(float deltaTime);

    const std::vector<std::unique_ptr<Bullet>>& getBullets() const { return m_pBullets; }

private:
    bool m_isActive = false;
    float m_timer = 0.0f;

    float m_bulletCooldown = 0.0f;
    float m_nextBulletCooldown = StartBulletCooldown;
    
    float m_laserActiveTime = WeaponLaserActiveTime;
    float m_laserCooldown = WeaponLaserCoolDown;

    float m_bulletSpeed = WeaponProjectileSpeed;

    Game* m_pGame;
    std::vector<std::unique_ptr<Bullet>> m_pBullets;
};