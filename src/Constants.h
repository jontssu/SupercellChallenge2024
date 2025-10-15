#pragma once

const int ScreenWidth = 1200;
const int ScreenHeight = 800;
const float GlobalScale = 1.0f;

const float PlayerHeight = 55.0f;
const float PlayerWidth = 55.0f;
const float PlayerSpeed = 200.0f;
const float PlayerMaxHealth = 100.0f;
const sf::Vector2f PlayerScale = sf::Vector2f(3.5f * GlobalScale, 3.5f * GlobalScale);

const float VampireHeight = 32.0f;
const float VampireWidth = 32.0f;
const float VampireBaseHealth = 10.0f;
const float VampireSpeed = 50.0f;
const float VampireBaseDamage = 1.0f;
const float VampireBaseReward = 5.0f;
const float VampireSpawnCooldown = 2.0f;
const sf::Vector2f VampireScale = sf::Vector2f(2.0f * GlobalScale, 2.0f * GlobalScale);

const float WeaponHeight = 10.0f;
const float WeaponWidth = 100.0f;
const float WeaponActiveTime = 0.05f;
const int	WeaponPierceCount = 1;
const float WeaponBulletDamage = 5.0f;
const float WeaponProjectileSpeed = 300.0f;
const float WeaponLaserActiveTime = 1.0f;
const float WeaponLaserCoolDown = 5.0f;
const float WeaponFireRate = 2.5f;

const float BaseHeight = 50.0f;
const float BaseWidth = 50.0f;	
const float BaseStartingHealth = 100.0f;
const float BaseDamage = 10.0f;
const float BaseFireRate = 5.0f;
const float BasePierceCount = 1.0f;

const sf::Vector2f ShopBgSize = sf::Vector2f(450.0f, 350.0f);

const float GameStartWaitingTime = 1.0f;
const bool PlayerImmunity = false;
const bool BaseImmunity = false;
const bool DisableWeapons = false;
const bool DisableLaser = true;
const bool DisableGun = false;
const bool InfiniteMoney = false;
const bool InfiniteEnemies = false;
