#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Constants.h"

class Player;
class Game;
class GameInput;
class Vampire;
class Weapon;
class Base;

namespace sf { class Clock; }

class Game : public sf::Drawable
{
public:
    
    enum class State
    {
        WAITING,
        ACTIVE,
        PAUSED,
    };
    
    Game();
    ~Game();
    
    bool initialise();
    void update(float deltaTime);
    void resetLevel();
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    
    State getState() const { return m_state; }
    
    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);

    Player* getPlayer() const;
    Base* getBase() const;
    sf::Texture* getPlayerTexture() { return &m_playerTexture; }
    sf::Texture* getVampireTexture() { return &m_vampTexture; }
    sf::Texture* getGunTexture() { return &m_gunTexture;}
    sf::Texture* getBaseTexture() { return &m_baseTexture; }

    void vampireSpawner(float deltaTime);

    const std::vector<std::unique_ptr<Vampire>>& getVampires() const { return m_pVampires; }

private:
    std::unique_ptr<Player> m_pPlayer;
    std::unique_ptr<Base> m_pBase;

    std::vector<std::unique_ptr<Vampire>> m_pVampires;

    State m_state;
    std::unique_ptr<sf::Clock> m_pClock;
    std::unique_ptr<GameInput> m_pGameInput;

    float m_vampireCooldown = 0.0f;
    float m_nextVampireCooldown = StartNextVampireCooldown;
    int m_spawnCount = 0;
    
    sf::Font m_font;
    sf::Texture m_vampTexture;
    sf::Texture m_playerTexture;
    sf::Texture m_gunTexture;
    sf::Texture m_baseTexture;

    float m_fpsElapsed = 0.0f;
    int m_fpsFrames = 0;
    int m_fps = 0;

    int m_playerMoney = 0;

    bool m_shopOpen = false;
};
