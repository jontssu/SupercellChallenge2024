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
            std::swap(m_pVampires[i], m_pVampires.back());
            m_pVampires.pop_back();
            continue;
        }
        i++;
    }

}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    //  Draw texts.
    if (m_state == State::WAITING)
    {
        sf::Text startText;
        startText.setFont(m_font);
        startText.setString("Game Start!");
        startText.setFillColor(sf::Color::White);
        startText.setPosition(80.0f, 80.0f);
        startText.setStyle(sf::Text::Bold);
        target.draw(startText);
    }
    else if (m_state == State::PAUSED)
    {
        sf::RectangleShape shopBg(sf::Vector2f(400, 300));
        shopBg.setFillColor(sf::Color(30, 30, 30, 220));
        shopBg.setOutlineColor(sf::Color::Yellow);
        shopBg.setOutlineThickness(4);
        shopBg.setPosition((ScreenWidth - 400) / 2, (ScreenHeight - 300) / 2);
        target.draw(shopBg);

        sf::Text shopText;
        shopText.setFont(m_font);
        shopText.setString("SHOP\nPress B to resume");
        shopText.setFillColor(sf::Color::White);
        shopText.setCharacterSize(32);
        shopText.setStyle(sf::Text::Bold);
        shopText.setPosition((ScreenWidth - 400) / 2 + 40, (ScreenHeight - 300) / 2 + 40);
        target.draw(shopText);
        // You can add more shop UI here
    }
    else
    {
        sf::Text timerText;
        timerText.setFont(m_font);
        timerText.setFillColor(sf::Color::White);
        timerText.setStyle(sf::Text::Bold);
        timerText.setString(std::to_string((int)m_pClock->getElapsedTime().asSeconds()));
        timerText.setPosition(sf::Vector2f((ScreenWidth - timerText.getLocalBounds().getSize().x) * 0.5, 20));
        target.draw(timerText);
    }

    //  Draw base.
    m_pBase->draw(target, states);  

    // Draw player.
    m_pPlayer->draw(target, states);

    //  Draw world.
    for (auto& temp : m_pVampires)
    {
        temp->draw(target, states);
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