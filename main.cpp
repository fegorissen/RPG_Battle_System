#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

namespace rpg {

// ----------------------------
// Template functie
// ----------------------------
template<typename T>
T calculateDamage(T baseDamage, double multiplier = 1.0) {
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

    // ✅ Memory-efficiënte types
    unsigned char level;           // 0–255 is genoeg voor level
    unsigned char criticalChance;  // 0–100 (%)

public:
    Character(const std::string& n, int h, int a,
              unsigned char lvl = 1,
              unsigned char crit = 10)
        : name(n),
        health(h),
        attackPower(a),
        maxHealth(h),
        level(lvl),
        criticalChance(crit) {}

    Character(const Character& other)
        : name(other.name),
        health(other.health),
        attackPower(other.attackPower),
        maxHealth(other.maxHealth),
        level(other.level),
        criticalChance(other.criticalChance) {}

    virtual ~Character() {}

    inline std::string getName() const { return name; }
    inline int getHealth() const { return health; }
    inline int getMaxHealth() const { return maxHealth; }
    inline bool isAlive() const { return health > 0; }
    inline unsigned char getLevel() const { return level; }

    virtual void attack(Character& target, int multiplier = 1) {
        static std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> variation(8, 12);
        std::uniform_int_distribution<int> critRoll(1, 100);

        bool criticalHit = critRoll(rng) <= criticalChance;

        int rawDamage = calculateDamage<int>(attackPower, multiplier);
        int damage = calculateDamage<int>(
            rawDamage,
            variation(rng) / 10.0 * (criticalHit ? 2.0 : 1.0)
            );

        target.health -= damage;
        if (target.health < 0) target.health = 0;

        std::cout << name << " attacks " << target.getName()
                  << " for " << damage << " damage";
        if (criticalHit) std::cout << " (CRITICAL HIT!)";
        std::cout << "\n";
    }
};

// ----------------------------
// Friend class
// ----------------------------
class BattleLogger {
public:
    static void logStatus(const Character& c) {
        std::cout << "[LOG] " << c.name
                  << " (Lv " << static_cast<int>(c.level) << ") "
                  << "HP: " << c.health
                  << "/" << c.maxHealth << "\n";
    }
};

// ----------------------------
// Player class
// ----------------------------
class Player : public Character {
private:
    std::vector<std::string> inventory;

public:
    Player() : Player("Hero", 100, 18, 1, 20) {}

    Player(const std::string& n, int h, int a,
           unsigned char lvl,
           unsigned char crit)
        : Character(n, h, a, lvl, crit), inventory() {}

    Player(const Player& other)
        : Character(other), inventory(other.inventory) {}

    ~Player() {}

    void attack(Character& target, int multiplier = 1) override {
        std::cout << name << " bravely attacks!\n";
        Character::attack(target, multiplier);
    }

    void heal(int amount = 20) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP!\n";
    }
};

// ----------------------------
// Monster class
// ----------------------------
class Monster : public Character {
public:
    Monster() : Monster("Goblin", 100, 15, 1, 10) {}

    Monster(const std::string& n, int h, int a,
            unsigned char lvl,
            unsigned char crit)
        : Character(n, h, a, lvl, crit) {}

    Monster(const Monster& other)
        : Character(other) {}

    ~Monster() {}

    void attack(Character& target, int multiplier = 1) override {
        std::cout << name << " fiercely attacks!\n";
        Character::attack(target, multiplier);
    }
};

// ----------------------------
// Game class
// ----------------------------
class Game {
private:
    Character* player;
    Character* monster;

public:
    Game() {
        player = new Player();
        monster = new Monster();
    }

    ~Game() {
        delete player;
        delete monster;
    }

    void start() {
        std::cout << "Battle start!\n\n";

        while (player->isAlive() && monster->isAlive()) {
            player->attack(*monster);
            BattleLogger::logStatus(*monster);

            if (!monster->isAlive()) break;

            monster->attack(*player);
            BattleLogger::logStatus(*player);

            if (player->getHealth() < 40) {
                dynamic_cast<Player*>(player)->heal();
                BattleLogger::logStatus(*player);
            }

            std::cout << "--------------------\n";
        }

        std::cout << "\nWinner: "
                  << (player->isAlive()
                          ? player->getName()
                          : monster->getName())
                  << "\n";
    }
};

} // namespace rpg

// ----------------------------
// main
// ----------------------------
int main() {
    using namespace rpg;

    Game game;
    game.start();
    return 0;
}
