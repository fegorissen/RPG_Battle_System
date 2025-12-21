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

    // Nuttige bool variabelen
    bool isStunned;
    bool hasShield;
    bool criticalActive;
    bool isPoisoned;

public:
    Character(const std::string& n,
              const int& h,
              const int& a,
              const unsigned char& lvl = 1,
              const unsigned char& crit = 10)
        : name(n), health(h), attackPower(a), maxHealth(h),
        level(lvl), criticalChance(crit),
        isStunned(false), hasShield(false),
        criticalActive(false), isPoisoned(false) {}

    Character(const Character& other)
        : name(other.name), health(other.health),
        attackPower(other.attackPower), maxHealth(other.maxHealth),
        level(other.level), criticalChance(other.criticalChance),
        isStunned(other.isStunned), hasShield(other.hasShield),
        criticalActive(other.criticalActive), isPoisoned(other.isPoisoned) {}

    virtual ~Character() {}

    inline std::string getName() const { return name; }
    inline int getHealth() const { return health; }
    inline int getMaxHealth() const { return maxHealth; }
    inline bool isAlive() const { return health > 0; }
    inline unsigned char getLevel() const { return level; }

    // Modern call-by-reference (1)
    virtual void attack(Character& target, const int& multiplier = 1) {
        if(isStunned) {
            std::cout << name << " is stunned and cannot attack!\n";
            isStunned = false;
            return;
        }

        static std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> variation(8, 12);
        std::uniform_int_distribution<int> critRoll(1, 100);

        bool criticalHit = critRoll(rng) <= criticalChance;
        criticalActive = criticalHit;

        int rawDamage = calculateDamage<int>(attackPower, multiplier);
        int damage = calculateDamage<int>(
            rawDamage,
            variation(rng) / 10.0 * (criticalHit ? 2.0 : 1.0)
            );

        if(target.hasShield) damage /= 2;

        target.health -= damage;
        if(target.health < 0) target.health = 0;

        std::cout << name << " attacks " << target.getName()
                  << " for " << damage << " damage";
        if(criticalHit) std::cout << " (CRITICAL HIT!)";
        if(target.hasShield) std::cout << " [Blocked by shield]";
        std::cout << "\n";
    }

    // Modern call-by-reference (2)
    void applyDamage(int& targetHealth, const int& damage) {
        targetHealth -= damage;
        if(targetHealth < 0) targetHealth = 0;
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

    Player(const std::string& n,
           const int& h,
           const int& a,
           const unsigned char& lvl,
           const unsigned char& crit)
        : Character(n, h, a, lvl, crit), inventory() {}

    Player(const Player& other)
        : Character(other), inventory(other.inventory) {}

    ~Player() {}

    void attack(Character& target, const int& multiplier = 1) override {
        std::cout << name << " bravely attacks!\n";
        Character::attack(target, multiplier);
    }

    void heal(const int& amount = 20) {
        health += amount;
        if(health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP!\n";
    }

    void addItem(std::string& item) { // call-by-reference
        inventory.push_back(item);
    }
};

// ----------------------------
// Monster class
// ----------------------------
class Monster : public Character {
public:
    Monster() : Monster("Goblin", 100, 15, 1, 10) {}

    Monster(const std::string& n,
            const int& h,
            const int& a,
            const unsigned char& lvl,
            const unsigned char& crit)
        : Character(n, h, a, lvl, crit) {}

    Monster(const Monster& other)
        : Character(other) {}

    ~Monster() {}

    void attack(Character& target, const int& multiplier = 1) override {
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
    std::vector<Character*> monsters;

public:
    Game() {
        player = new Player();

        monsters.push_back(new Monster("Goblin", 80, 12, 1, 10));
        monsters.push_back(new Monster("Orc", 120, 18, 2, 15));
        monsters.push_back(new Monster("Troll", 150, 20, 3, 5));
    }

    ~Game() {
        delete player;

        for(auto m : monsters) {
            delete m;
        }
        monsters.clear();
    }

    void start() {
        std::cout << "Battle start!\n\n";

        for(auto m : monsters) {
            std::cout << "Next battle: " << m->getName() << "\n";

            while(player->isAlive() && m->isAlive()) {
                player->attack(*m);
                BattleLogger::logStatus(*m);

                if(!m->isAlive()) break;

                m->attack(*player);
                BattleLogger::logStatus(*player);

                if(player->getHealth() < 40) {
                    dynamic_cast<Player*>(player)->heal();
                    BattleLogger::logStatus(*player);
                }

                std::cout << "--------------------\n";
            }

            if(!player->isAlive()) break;
        }

        std::cout << "\nWinner: "
                  << (player->isAlive() ? player->getName() : "Monsters")
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
