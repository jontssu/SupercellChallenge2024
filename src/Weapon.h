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

    void setBulletDamage(float amount) { m_bulletDamage += amount; } 
    float getBulletDamage() const { return m_bulletDamage; }


    float getFireRate() const { return m_nextBulletCooldown; }
    void setFireRate(float cooldown) { m_nextBulletCooldown = cooldown; }

    void setBulletPierce(int amount) { m_piercing += amount; }
    float getBulletPierce() const { return m_piercing; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void bulletSpawner(float deltaTime);
    void clearBullets() { m_pBullets.clear(); }

    const std::vector<std::unique_ptr<Bullet>>& getBullets() const { return m_pBullets; }

private:
    bool m_isActive = false;
    float m_timer = 0.0f;

    float m_bulletCooldown = 0.0f;
    float m_nextBulletCooldown = WeaponFireRate;
    float m_bulletDamage = WeaponBulletDamage;
    
    float m_laserActiveTime = WeaponLaserActiveTime;
    float m_laserCooldown = WeaponLaserCoolDown;

    float m_bulletSpeed = WeaponProjectileSpeed;
    int m_piercing = WeaponPierceCount;

    Game* m_pGame;
    std::vector<std::unique_ptr<Bullet>> m_pBullets;
};