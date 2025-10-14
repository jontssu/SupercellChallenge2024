#pragma once

const int ScreenWidth = 1200;
const int ScreenHeight = 800;

const float PlayerHeight = 55.0f;
const float PlayerWidth = 55.0f;
const float PlayerSpeed = 155.0f;

const float VampireHeight = 32.0f;
const float VampireWidth = 32.0f;
const float VampireSpeed = 100.0f;
const float VampireBaseDamage = 1.0f;
const float VampireBaseReward = 5.0f;

const float WeaponHeight = 10.0f;
const float WeaponWidth = 100.0f;
const float WeaponActiveTime = 0.05f;
const float WeaponPierceCount = 1;
const float WeaponProjectileSpeed = 200.0f;
const float WeaponLaserActiveTime = 1.0f;
const float WeaponLaserCoolDown = 5.0f;

const float StartBulletCooldown = 2.5f;
const float StartNextVampireCooldown = 3.0f;

const float BaseHeight = 50.0f;
const float BaseWidth = 50.0f;	
const float BaseStartingHealth = 100.0f;

const sf::Vector2f ShopBgSize = sf::Vector2f(450.0f, 300.0f);

const float GameStartWaitingTime = 1.0f;
const bool PlayerImmunity = false;
const bool BaseImmunity = false;
const bool DisableWeapons = false;
const bool DisableLaser = true;
const bool DisableGun = false;
