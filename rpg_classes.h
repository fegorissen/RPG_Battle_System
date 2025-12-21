#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

namespace rpg {

// ----------------------------
// Template functie
// ----------------------------
template<typename T>
T calculateDamage(const T& baseDamage, const double& multiplier = 1.0) {
    return static_cast<T>(baseDamage * multiplier);
}

// Forward declaration
class BattleLogger;

// ----------------------------
// Abstracte basisklasse Character
// ----------------------------
class Character {
    friend class BattleLogger;

protected:
    std::string name;
    int health;
    int attackPower;
    const int maxHealth;

    unsigned char level;
    unsigned char criticalChance;

    bool isStunned;
    bool hasShield;
    bool criticalActive;
    bool isPoisoned;

public:
    Character(const std::string& n, int h, int a, unsigned char lvl = 1, unsigned char crit = 10)
        : name(n), health(h), attackPower(a), maxHealth(h),
        level(lvl), criticalChance(crit),
        isStunned(false), hasShield(false),
        criticalActive(false), isPoisoned(false) {}

    virtual ~Character() {}

    inline std::string getName() const { return name; }
    inline int getHealth() const { return health; }
    inline int getMaxHealth() const { return maxHealth; }
    inline bool isAlive() const { return health > 0; }
    inline unsigned char getLevel() const { return level; }

    virtual void attack(Character& target, int multiplier = 1);
};

// ----------------------------
// Friend class BattleLogger
// ----------------------------
class BattleLogger {
public:
    static void logStatus(const Character& c);
};

// ----------------------------
// Player class
// ----------------------------
class Player : public Character {
private:
    std::vector<std::string> inventory;

public:
    Player();
    Player(const std::string& n, int h, int a, unsigned char lvl, unsigned char crit);

    void attack(Character& target, int multiplier = 1) override;
    void heal(int amount = 20);
    void addItem(const std::string& item);
    std::string showInventory() const;
};

// ----------------------------
// Monster class
// ----------------------------
class Monster : public Character {
public:
    Monster();
    Monster(const std::string& n, int h, int a, unsigned char lvl, unsigned char crit);

    void attack(Character& target, int multiplier = 1) override;
};

// ----------------------------
// Game class
// ----------------------------
class Game {
private:
    Character* player;
    std::vector<Character*> monsters;

public:
    Game();
    ~Game();

    Player* getPlayer() const;
    void showAllMonsters() const;
    void start();
};

} // namespace rpg
