#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

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

        // Shop UI: 5 clickable icons
        int numIcons = 5;
        float iconSize = 64.0f;
        float iconSpacing = 24.0f;
        float totalWidth = iconSize * numIcons + iconSpacing * (numIcons - 1);
        float shopBgX = (ScreenWidth - ShopBgSize.x) / 2;
        float startX = shopBgX + (ShopBgSize.x - totalWidth) / 2;
        float startY = (ScreenHeight - ShopBgSize.y) / 2 + 120;
        for (int i = 0; i < numIcons; ++i)
        {
            sf::RectangleShape icon(sf::Vector2f(iconSize, iconSize));
            icon.setFillColor(sf::Color(80, 80, 200, 220));
            icon.setOutlineColor(sf::Color::White);
            icon.setOutlineThickness(3);
            icon.setPosition(startX + i * (iconSize + iconSpacing), startY);
            target.draw(icon);

            sf::Text label;
            label.setFont(m_font);
            label.setString("Item " + std::to_string(i+1));
            label.setCharacterSize(18);
            label.setFillColor(sf::Color::White);
            label.setPosition(startX + i * (iconSize + iconSpacing) + 8, startY + iconSize/2 - 12);
            target.draw(label);
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