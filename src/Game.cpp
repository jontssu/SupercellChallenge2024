#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "ResourceManager.h"
#include "InputHandler.h"
#include "Weapon.h"
#include "Player.h"
#include "Rectangle.h"
#include "Vampire.h"
#include "Base.h"
#include "Constants.h"

Game::Game() :
    m_state(State::WAITING),
    m_pClock(std::make_unique<sf::Clock>()),
    m_pPlayer(std::make_unique<Player>(this)),
    m_pBase(std::make_unique<Base>(this)),
    m_vampireCooldown(2.0f),
    m_nextVampireCooldown(2.0f)
{
    m_pGameInput = std::make_unique<GameInput>(this, m_pPlayer.get());
}

Game::~Game()
{
}

bool Game::initialise()
{
    /*
        PAY ATTENTION HIVER!
            If you want to load any assets (fonts, textures) please use the pattern shown below
    */

    if (!m_font.loadFromFile(ResourceManager::getFilePath("Lavigne.ttf")))
    {
        std::cerr << "Unable to load font" << std::endl;
        return false;
    }
    if (!m_vampTexture.loadFromFile(ResourceManager::getFilePath("vampire.png")))
    {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }
    if (!m_playerTexture.loadFromFile(ResourceManager::getFilePath("player.png")))
    {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }
    if (!m_gunTexture.loadFromFile(ResourceManager::getFilePath("gun.png")))
    {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }
    if (!m_baseTexture.loadFromFile(ResourceManager::getFilePath("player.png")))
    {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }   

    resetLevel();
    return true;
}

void Game::resetLevel()
{
    m_pVampires.clear();
    m_nextVampireCooldown = StartNextVampireCooldown;

    m_pBase->initialise();
    m_pPlayer->initialise();
    m_pClock->restart();
}

void Game::update(float deltaTime)
{
    m_fpsElapsed += deltaTime;
    m_fpsFrames++;  
    if (m_fpsElapsed >= 1.0f) {
        m_fps = m_fpsFrames;
        m_fpsFrames = 0;
        m_fpsElapsed = 0.0f;
    }
    switch (m_state)
    {
        case State::WAITING:
        {
            if (m_pClock->getElapsedTime().asSeconds() >= GameStartWaitingTime)
            {
                m_state = State::ACTIVE;
                m_pClock->restart();
            }
        }
        break;

        case State::ACTIVE:
        {
            m_pGameInput->update(deltaTime);
            m_pPlayer->update(deltaTime);
            m_pBase->update(deltaTime);

            vampireSpawner(deltaTime);
            for (auto& temp : m_pVampires)
            {
                temp->update(deltaTime);
            }

            if (m_pPlayer->isDead())
            {
                m_state = State::WAITING;
                resetLevel();
            }

            if (m_pBase->isDestroyed())
            {
                m_state = State::WAITING;
                resetLevel();
            }
        }
        break;

        case State::PAUSED:
        {
            // Do nothing, game is paused
        }
        break;
    }

    int i = 0;
    while (i < m_pVampires.size())
    {
        if (m_pVampires[i]->isKilled())
        {
            m_playerMoney += m_pVampires[i]->getDifficultyScaling() * VampireBaseReward;
            m_totalKills += 1;
            std::swap(m_pVampires[i], m_pVampires.back());
            m_pVampires.pop_back();
            continue;
        }
        i++;
    }

}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_state == State::WAITING)
    {
        sf::Text startText;
        startText.setFont(m_font);
        startText.setString("Game Start!");
        startText.setFillColor(sf::Color::White);
        startText.setPosition(80.0f, 80.0f);
        startText.setStyle(sf::Text::Bold);
        target.draw(startText);
        return;
    }

    // Draw base, player, vampires for ACTIVE and PAUSED
    m_pBase->draw(target, states);
    m_pPlayer->draw(target, states);
    for (auto& temp : m_pVampires)
        temp->draw(target, states);

    // GO SHOPPING YE
    if (m_state == State::PAUSED)
    {
        // Shop background
        sf::RectangleShape shopBg(ShopBgSize);
        shopBg.setFillColor(sf::Color(30, 30, 30, 220));
        shopBg.setOutlineColor(sf::Color::Yellow);
        shopBg.setOutlineThickness(4);
        shopBg.setPosition((ScreenWidth - ShopBgSize.x) / 2, (ScreenHeight - ShopBgSize.y) / 2);
        target.draw(shopBg);

        // Shop text
        sf::Text shopText;
        shopText.setFont(m_font);
        shopText.setString("SHOP\nPress B to resume");
        shopText.setFillColor(sf::Color::White);
        shopText.setCharacterSize(32);
        shopText.setStyle(sf::Text::Bold);
        shopText.setPosition((ScreenWidth - ShopBgSize.x) / 2 + 40, (ScreenHeight - ShopBgSize.y) / 2 + 40);
        target.draw(shopText);

        // Shop UI: 10 clickable icons for all ShopOption enums except NONE
        int numIcons = m_baseUnlocked ? 10 : 6;
        int iconsPerRow = 5;
        float iconSize = 64.0f;
        float iconSpacing = 18.0f;
        float totalWidth = iconSize * iconsPerRow + iconSpacing * (iconsPerRow - 1);
        float shopBgX = (ScreenWidth - ShopBgSize.x) / 2;
        float startX = shopBgX + (ShopBgSize.x - totalWidth) / 2;
        float startY = (ScreenHeight - ShopBgSize.y) / 2 + 120;
        const char* shopNames[] = {
            "Health", "Damage", "FireRate", "Piercing", "Player\nSpeed",
            "Unlock\nBase", "Base\nHealth", "Base\nDamage", "Base\nFireRate", "Base\nPiercing"
        };
        float rowCushion = 32.0f; // Extra space between rows for prices
        float row2Y = startY + iconSize + iconSpacing + rowCushion;
        // First row
        for (int i = 0; i < iconsPerRow; ++i)
        {
            sf::RectangleShape icon(sf::Vector2f(iconSize, iconSize));
            icon.setFillColor(sf::Color(80, 80, 200, 220));
            icon.setOutlineColor(sf::Color::White);
            icon.setOutlineThickness(3);
            icon.setPosition(startX + i * (iconSize + iconSpacing), startY);
            target.draw(icon);

            sf::Text label;
            label.setFont(m_font);
            label.setString(shopNames[i]);
            label.setCharacterSize(16);
            label.setFillColor(sf::Color::White);
            label.setPosition(startX + i * (iconSize + iconSpacing) + 8, startY + iconSize/2 - 12);
            target.draw(label);

            // Draw price below icon
            sf::Text priceText;
            priceText.setFont(m_font);
            int price = 0;
            switch (i) {
                case 0: price = m_upgradeMaxHealthCost; break;
                case 1: price = m_upgradeDamageCost; break;
                case 2: price = m_upgradeFireRateCost; break;
                case 3: price = m_upgradePiercingCost; break;
                case 4: price = m_upgradePlayerSpeedCost; break;
            }
            priceText.setString("$" + std::to_string(price));
            priceText.setCharacterSize(14);
            priceText.setFillColor(sf::Color::Yellow);
            priceText.setPosition(startX + i * (iconSize + iconSpacing) + 8, startY + iconSize + 4);
            target.draw(priceText);
        }
        // Second row
        for (int i = iconsPerRow; i < numIcons; ++i)
        {
            int rowIdx = i - iconsPerRow;
            sf::RectangleShape icon(sf::Vector2f(iconSize, iconSize));
            icon.setFillColor(sf::Color(80, 80, 200, 220));
            icon.setOutlineColor(sf::Color::White);
            icon.setOutlineThickness(3);
            icon.setPosition(startX + rowIdx * (iconSize + iconSpacing), row2Y);
            target.draw(icon);

            sf::Text label;
            label.setFont(m_font);
            label.setString(shopNames[i]);
            label.setCharacterSize(16);
            label.setFillColor(sf::Color::White);
            label.setPosition(startX + rowIdx * (iconSize + iconSpacing) + 8, row2Y + iconSize/2 - 12);
            target.draw(label);

            // Draw price below icon
            sf::Text priceText;
            priceText.setFont(m_font);
            int price = 0;
            switch (i) {
                case 5: price = m_upgradeBaseUnlockCost; break;
                case 6: price = m_upgradeBaseHealthCost; break;
                case 7: price = m_upgradeBaseDamageCost; break;
                case 8: price = m_upgradeBaseFireRateCost; break;
                case 9: price = m_upgradeBasePiercingCost; break;
            }
            priceText.setString("$" + std::to_string(price));
            priceText.setCharacterSize(14);
            priceText.setFillColor(sf::Color::Yellow);
            priceText.setPosition(startX + rowIdx * (iconSize + iconSpacing) + 8, row2Y + iconSize + 4);
            target.draw(priceText);

            //  Draw stats
            sf::Text statText;
            statText.setFont(m_font);
            statText.setFillColor(sf::Color::White);
            statText.setPosition((ScreenWidth + ShopBgSize.x) / 2 + 128, (ScreenHeight - ShopBgSize.y) / 2 - 64);

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << m_pPlayer->getWeapon()->getFireRate();
            std::string fireRateStr = oss.str();
            oss.str("");
            oss.clear();
            oss << std::fixed << std::setprecision(2) << m_pBase->getWeapon()->getFireRate();
            std::string baseFireRateStr = oss.str();
            

            statText.setString("Player\n\nMax HP: " + std::to_string(static_cast<int>(m_pPlayer->getMaxHealth())) +
                "\nDamage: " + std::to_string(static_cast<int>(m_pPlayer->getWeapon()->getBulletDamage())) +
                "\nFire Rate: " + fireRateStr + "/s" +
                "\nPiercing: " + std::to_string(static_cast<int>(m_pPlayer->getWeapon()->getBulletPierce())) +
                "\nSpeed: " + std::to_string(static_cast<int>(m_pPlayer->getSpeed())) +
                "\n\nBase\n\nMax HP: " + std::to_string(static_cast<int>(m_pBase->getMaxHealth())) +
                "\nDamage: " + std::to_string(static_cast<int>(m_pBase->getWeapon()->getBulletDamage())) +
                "\nFire Rate: " + baseFireRateStr + "/s" +
                "\nPiercing: " + std::to_string(static_cast<int>(m_pBase->getWeapon()->getBulletPierce())));
            target.draw(statText);
        }
    }
    else
    {
        sf::Text timerText;
        timerText.setFont(m_font);
        timerText.setFillColor(sf::Color::White);
        timerText.setStyle(sf::Text::Bold);
        timerText.setString("Kills " + std::to_string(m_totalKills));
        timerText.setPosition(sf::Vector2f((ScreenWidth - timerText.getLocalBounds().getSize().x) * 0.5, 20));
        target.draw(timerText);
    }
    
    // Draw FPS
    sf::Text fpsText;
    fpsText.setFont(m_font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::Yellow);
    fpsText.setString("FPS: " + std::to_string(m_fps));
    fpsText.setPosition(10, 10);
    target.draw(fpsText);

    // Draw Money
    sf::Text moneyText;
    moneyText.setFont(m_font);
    moneyText.setCharacterSize(16);
    moneyText.setFillColor(sf::Color::Yellow);
    moneyText.setString("Money: " + std::to_string(m_playerMoney));
    moneyText.setPosition(10, 30);
    target.draw(moneyText);
}


void Game::onKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::B)
    {
        if (m_state == State::ACTIVE)
        {
            m_state = State::PAUSED;
        }
        else if (m_state == State::PAUSED)
        {
            m_state = State::ACTIVE;
        }
        return;
    }
    m_pGameInput->onKeyPressed(key);
}

void Game::onKeyReleased(sf::Keyboard::Key key)
{
    m_pGameInput->onKeyReleased(key);
}

Player* Game::getPlayer() const 
{
    return m_pPlayer.get();
}

Base* Game::getBase() const
{
    return m_pBase.get();
}

void Game::vampireSpawner(float deltaTime)
{
    // Infinite spawning mode
    if (InfiniteEnemies)
    {
        float randomXPos = rand() % ScreenWidth;
        float randomYPos = rand() % ScreenHeight;
        float distToRight = (float) ScreenWidth - randomXPos;
        float distToBottom = (float) ScreenHeight - randomYPos;
        float xMinDist = randomXPos < distToRight ? -randomXPos : distToRight;
        float yMinDist = randomYPos < distToBottom ? -randomYPos : distToBottom;
        if (abs(xMinDist) < abs(yMinDist))
            randomXPos += xMinDist;
        else
            randomYPos += yMinDist;
        sf::Vector2f spawnPosition = sf::Vector2f(randomXPos, randomYPos);
        m_pVampires.push_back(std::make_unique<Vampire>(this, spawnPosition));
            
        m_spawnCount++;
        m_vampireCooldown = 0.0f;
        return;
    }

    // Normal spawning mode
    if (m_vampireCooldown > 0.0f)
    {
        m_vampireCooldown -= deltaTime;
        return;
    }
    float randomXPos = rand() % ScreenWidth;
    float randomYPos = rand() % ScreenHeight;
    float distToRight = (float) ScreenWidth - randomXPos;
    float distToBottom = (float) ScreenHeight - randomYPos;
    float xMinDist = randomXPos < distToRight ? -randomXPos : distToRight;
    float yMinDist = randomYPos < distToBottom ? -randomYPos : distToBottom;
    if (abs(xMinDist) < abs(yMinDist))
        randomXPos += xMinDist;
    else
        randomYPos += yMinDist;
    sf::Vector2f spawnPosition = sf::Vector2f(randomXPos, randomYPos);
    m_pVampires.push_back(std::make_unique<Vampire>(this, spawnPosition));
    m_spawnCount++;
    if (m_spawnCount % 5 == 0)
    {
        if (m_nextVampireCooldown > 0.2f)
            m_nextVampireCooldown -= 0.1f;
    }
    m_vampireCooldown = m_nextVampireCooldown;
}

void Game::onMousePressed(sf::Vector2i mousePos)
{
    if (m_state != State::PAUSED)
        return;

    // Shop UI: 10 clickable icons for all ShopOption enums except NONE
    int numIcons = 10;
    int iconsPerRow = 5;
    float iconSize = 64.0f;
    float iconSpacing = 18.0f;
    float totalWidth = iconSize * iconsPerRow + iconSpacing * (iconsPerRow - 1);
    float shopBgX = (ScreenWidth - ShopBgSize.x) / 2;
    float startX = shopBgX + (ShopBgSize.x - totalWidth) / 2;
    float startY = (ScreenHeight - ShopBgSize.y) / 2 + 120;
    float rowCushion = 32.0f; // Extra space between rows for prices
    float row2Y = startY + iconSize + iconSpacing + rowCushion;
    for (int i = 0; i < numIcons; ++i)
    {
        int rowIdx = i % iconsPerRow;
        float iconY = (i < iconsPerRow) ? startY : row2Y;
        sf::FloatRect iconRect(startX + rowIdx * (iconSize + iconSpacing), iconY, iconSize, iconSize);
        if (iconRect.contains((float)mousePos.x, (float)mousePos.y))
                    {
                        // Handle shop purchase for icon i (corresponds to ShopOption enum order, skipping NONE)
                        switch (i)
                        {
                            case 0: // HEALTH
                                if (m_playerMoney >= m_upgradeMaxHealthCost)
                                {
                                    std::cout << "Upgrading Health" << std::endl;
                                    m_playerMoney -= m_upgradeMaxHealthCost;
                                    m_pPlayer->increaseMaxHealth(20);
                                    m_upgradeMaxHealthCost += 25;
                                }
                                break;
                            case 1: // DAMAGE
                                if (m_playerMoney >= m_upgradeDamageCost)
                                {
                                    std::cout << "Upgrading Damage" << std::endl;
                                    m_playerMoney -= m_upgradeDamageCost;
                                    m_pPlayer->increaseDamage(5);
                                    m_upgradeDamageCost += 25;
                                }
                                break;
                            case 2: // FIRE_RATE
                                if (m_playerMoney >= m_upgradeFireRateCost)
                                {
                                    std::cout << "Upgrading Fire Rate" << std::endl;
                                    m_playerMoney -= m_upgradeFireRateCost;
                                    float currentCooldown = m_pPlayer->getWeapon()->getFireRate();
                                    currentCooldown *= 0.9f;
                                    if (currentCooldown < 0.01f)
                                        currentCooldown = 0.01f;
                                    m_pPlayer->getWeapon()->setFireRate(currentCooldown);
                                    m_upgradeFireRateCost += 25;
                                }
                                break;
                            case 3: // PIERCING
                                if (m_playerMoney >= m_upgradePiercingCost)
                                {
                                    std::cout << "Upgrading Piercing" << std::endl;
                                    m_playerMoney -= m_upgradePiercingCost;
                                    m_pPlayer->getWeapon()->setBulletPierce(1); // Increases by one
                                    m_upgradePiercingCost += 100;
                                }
                                break;
                            case 4: // PLAYER_SPEED
                                if (m_playerMoney >= m_upgradePlayerSpeedCost)
                                {
                                    std::cout << "Upgrading Player Speed" << std::endl;
                                    m_playerMoney -= m_upgradePlayerSpeedCost;
                                    m_pPlayer->increaseSpeed(20.0f);
                                    m_upgradePlayerSpeedCost += 50;
                                }
                                break;
                            case 5: // UNLOCK_BASE
                                if (m_playerMoney >= m_upgradeBaseUnlockCost && !m_baseUnlocked)
                                {
                                    std::cout << "Unlocking Base" << std::endl;
                                    m_playerMoney -= m_upgradeBaseUnlockCost;
                                    m_pBase->unlockBase(true);
                                    m_baseUnlocked = true;
                                }
                                break;
                            case 6: // BASE_BEALTH
                                if (m_playerMoney >= m_upgradeBaseHealthCost && m_baseUnlocked)
                                {
                                    std::cout << "Upgrading Base Health" << std::endl;
                                    m_playerMoney -= m_upgradeBaseHealthCost;
                                    m_pBase->increaseMaxHealth(20);
                                    m_upgradeBaseHealthCost += 25;
                                }
                                break;
                            case 7: // BASE_DAMAGE
                                if (m_playerMoney >= m_upgradeBaseDamageCost && m_baseUnlocked)
                                {
                                    std::cout << "Upgrading Base Damage" << std::endl;
                                    m_playerMoney -= m_upgradeBaseDamageCost;
                                    m_pBase->getWeapon()->setBulletDamage(5); // Increases by five
                                    m_upgradeBaseDamageCost += 25;
                                }
                                break;
                            case 8: // BASE_FIRE_RATE
                                if (m_playerMoney >= m_upgradeBaseFireRateCost && m_baseUnlocked)
                                {
                                    std::cout << "Upgrading Base Fire Rate" << std::endl;
                                    m_playerMoney -= m_upgradeBaseFireRateCost;
                                    float currentCooldown = m_pBase->getWeapon()->getFireRate();
                                    currentCooldown *= 0.9f;
                                    if (currentCooldown < 0.01f)
                                        currentCooldown = 0.01f;
                                    m_pBase->getWeapon()->setFireRate(currentCooldown);
                                    m_upgradeBaseFireRateCost += 25;
                                }
                                break;
                            case 9: // BASE_PIERCING
                                if (m_playerMoney >= m_upgradeBasePiercingCost && m_baseUnlocked)
                                {
                                    std::cout << "Upgrading Base Piercing" << std::endl;
                                    m_playerMoney -= m_upgradeBasePiercingCost;
                                    m_pBase->getWeapon()->setBulletPierce(1); // Increases by one
                                    m_upgradeBasePiercingCost += 100;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }