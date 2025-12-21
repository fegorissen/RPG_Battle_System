#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

// ============================
// Zelfgemaakte namespace
// ============================
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
    // OOP-eis: nuttige friend class
    friend class BattleLogger;

protected:
    std::string name;
    int health;
    int attackPower;
    const int maxHealth;

public:
    Character(const std::string& n, int h, int a)
        : name(n), health(h), attackPower(a), maxHealth(h) {}

    Character(const Character& other)
        : name(other.name),
        health(other.health),
        attackPower(other.attackPower),
        maxHealth(other.maxHealth) {}

    virtual ~Character() {}

    inline std::string getName() const { return name; }
    inline int getHealth() const { return health; }
    inline int getMaxHealth() const { return maxHealth; }
    inline bool isAlive() const { return health > 0; }

    virtual void attack(Character& target, int multiplier = 1) {
        static std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> variation(8, 12);

        int rawDamage = calculateDamage<int>(attackPower, multiplier);
        int damage = calculateDamage<int>(rawDamage, variation(rng) / 10.0);

        target.health -= damage;
        if (target.health < 0) target.health = 0;

        std::cout << name << " attacks " << target.getName()
                  << " for " << damage << " damage!\n";
    }
};

// ----------------------------
// Friend class
// ----------------------------
class BattleLogger {
public:
    static void logStatus(const Character& c) {
        std::cout << "[LOG] " << c.name
                  << " HP: " << c.health
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
    Player() : Player("Hero", 100, 15) {}

    Player(const std::string& n, int h, int a)
        : Character(n, h, a), inventory() {}

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
    Monster() : Monster("Goblin", 100, 12) {}

    Monster(const std::string& n, int h, int a)
        : Character(n, h, a) {}

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

} // einde namespace rpg

// ----------------------------
// main
// ----------------------------
int main() {
    using namespace rpg;

    Game game;
    game.start();
    return 0;
}
