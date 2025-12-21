#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <sstream>
#include <fstream>
#include <exception>
#include <thread>
#include <chrono>

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

        auto computeDamage = [&](int base, double mod) -> int {
            int dmg = static_cast<int>(base * mod);
            if(target.hasShield) dmg /= 2;
            return dmg;
        };

        int rawDamage = calculateDamage<int>(attackPower, multiplier);
        int damage = computeDamage(rawDamage, variation(rng) / 10.0 * (criticalHit ? 2.0 : 1.0));

        target.health -= damage;
        if(target.health < 0) target.health = 0;

        std::cout << name << " attacks " << target.getName()
                  << " for " << damage << " damage";
        if(criticalHit) std::cout << " (CRITICAL HIT!)";
        if(target.hasShield) std::cout << " [Blocked by shield]";
        std::cout << "\n";

        try {
            std::ofstream logfile("battle_log.txt", std::ios::app);
            if (!logfile) throw std::ios_base::failure("Cannot open battle_log.txt");
            logfile << name << " attacks " << target.getName()
                    << " for " << damage << " damage";
            if(criticalHit) logfile << " (CRITICAL HIT!)";
            if(target.hasShield) logfile << " [Blocked by shield]";
            logfile << "\n";
        } catch (const std::ios_base::failure& e) {
            std::cerr << "File I/O error: " << e.what() << "\n";
        }
    }
};

// ----------------------------
// Friend class BattleLogger
// ----------------------------
class BattleLogger {
public:
    static void logStatus(const Character& c) {
        std::cout << "[LOG] " << c.name
                  << " (Lv " << static_cast<int>(c.level) << ") "
                  << "HP: " << c.health
                  << "/" << c.maxHealth << "\n";

        try {
            std::ofstream logfile("battle_log.txt", std::ios::app);
            if (!logfile) throw std::ios_base::failure("Cannot open battle_log.txt");
            logfile << "[LOG] " << c.name
                    << " (Lv " << static_cast<int>(c.level) << ") "
                    << "HP: " << c.health
                    << "/" << c.maxHealth << "\n";
        } catch (const std::ios_base::failure& e) {
            std::cerr << "File I/O error: " << e.what() << "\n";
        }
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
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP!\n";

        try {
            std::ofstream logfile("battle_log.txt", std::ios::app);
            if (!logfile) throw std::ios_base::failure("Cannot open battle_log.txt");
            logfile << name << " heals for " << amount << " HP\n";
        } catch (const std::ios_base::failure& e) {
            std::cerr << "File I/O error: " << e.what() << "\n";
        }
    }

    void addItem(std::string& item) { inventory.push_back(item); }

    std::string showInventory() const {
        if (inventory.empty()) return name + "'s inventory is empty.";
        std::stringstream ss;
        ss << name << "'s Inventory: ";
        auto addItemToStream = [&](const std::string& item){ ss << item << " "; };
        for (const auto& item : inventory) addItemToStream(item);
        return ss.str();
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
// Game class met threads
// ----------------------------
class Game {
private:
    Character* player;
    std::vector<Character*> monsters;

public:
    Game() {
        try {
            player = new Player();
            monsters.push_back(new Monster("Goblin", 80, 12, 1, 10));
            monsters.push_back(new Monster("Orc", 120, 18, 2, 15));
            monsters.push_back(new Monster("Troll", 150, 20, 3, 5));
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << "\n";
            player = nullptr;
            monsters.clear();
        }
    }

    ~Game() {
        if (player) { delete player; player = nullptr; }
        for (auto& m : monsters) { if(m) { delete m; m=nullptr; } }
        monsters.clear();
    }

    Player* getPlayer() const { return dynamic_cast<Player*>(player); }

    void showAllMonsters() const {
        std::cout << "Monsters in the game:\n";
        for (const auto& m : monsters) {
            if (m) {
                std::cout << "- " << m->getName() << " (HP: " << m->getHealth() << ")\n";
            }
        }
    }

    void start() {
        showAllMonsters();

        for (auto& m : monsters) {
            if (!m) continue;

            std::cout << "\nNext battle: " << m->getName() << "\n";

            while(player && player->isAlive() && m->isAlive()) {

                std::thread playerThread([&]() {
                    player->attack(*m);
                    BattleLogger::logStatus(*m);
                });

                std::thread monsterThread([&]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    if (m->isAlive()) {
                        m->attack(*player);
                        BattleLogger::logStatus(*player);
                    }
                });

                playerThread.join();
                monsterThread.join();

                if (player->getHealth() < 40) {
                    dynamic_cast<Player*>(player)->heal();
                    BattleLogger::logStatus(*player);
                }

                std::cout << "--------------------\n";
            }

            if (!player || !player->isAlive()) break;
        }

        if (player)
            std::cout << "\n" << dynamic_cast<Player*>(player)->showInventory() << "\n";

        std::cout << "\nWinner: "
                  << (player && player->isAlive() ? player->getName() : "Monsters")
                  << "\n";
    }
};

} // namespace rpg

// ----------------------------
// main
// ----------------------------
int main() {
    try {
        using namespace rpg;

        Game game;

        std::string sword = "Sword";
        std::string shield = "Shield";

        if(game.getPlayer()) {
            game.getPlayer()->addItem(sword);
            game.getPlayer()->addItem(shield);
        }

        game.start();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown error occurred!\n";
    }

    return 0;
}
