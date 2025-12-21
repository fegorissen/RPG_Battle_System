#include "rpg_classes.h"
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>

namespace rpg {

// ----------------------------
// Character
// ----------------------------
void Character::attack(Character& target, int multiplier) {
    if (isStunned) {
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
        if (target.hasShield) dmg /= 2;
        return dmg;
    };

    int rawDamage = calculateDamage<int>(attackPower, multiplier);
    int damage = computeDamage(rawDamage, variation(rng)/10.0 * (criticalHit ? 2.0 : 1.0));

    target.health -= damage;
    if (target.health < 0) target.health = 0;

    std::cout << name << " attacks " << target.getName()
              << " for " << damage << " damage";
    if (criticalHit) std::cout << " (CRITICAL HIT!)";
    if (target.hasShield) std::cout << " [Blocked by shield]";
    std::cout << "\n";

    try {
        std::ofstream logfile("battle_log.txt", std::ios::app);
        if (!logfile) throw std::ios_base::failure("Cannot open battle_log.txt");
        logfile << name << " attacks " << target.getName()
                << " for " << damage << " damage";
        if (criticalHit) logfile << " (CRITICAL HIT!)";
        if (target.hasShield) logfile << " [Blocked by shield]";
        logfile << "\n";
    } catch (const std::ios_base::failure& e) {
        std::cerr << "File I/O error: " << e.what() << "\n";
    }
}

// ----------------------------
// BattleLogger
// ----------------------------
void BattleLogger::logStatus(const Character& c) {
    std::cout << "[LOG] " << c.getName()
    << " (Lv " << static_cast<int>(c.getLevel()) << ") "
    << "HP: " << c.getHealth() << "/" << c.getMaxHealth() << "\n";

    try {
        std::ofstream logfile("battle_log.txt", std::ios::app);
        if (!logfile) throw std::ios_base::failure("Cannot open battle_log.txt");
        logfile << "[LOG] " << c.getName()
                << " (Lv " << static_cast<int>(c.getLevel()) << ") "
                << "HP: " << c.getHealth() << "/" << c.getMaxHealth() << "\n";
    } catch (const std::ios_base::failure& e) {
        std::cerr << "File I/O error: " << e.what() << "\n";
    }
}

// ----------------------------
// Player
// ----------------------------
Player::Player() : Player("Hero", 100, 18, 1, 20) {}
Player::Player(const std::string& n, int h, int a, unsigned char lvl, unsigned char crit)
    : Character(n, h, a, lvl, crit), inventory() {}

void Player::attack(Character& target, int multiplier) {
    std::cout << name << " bravely attacks!\n";
    Character::attack(target, multiplier);
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
    std::cout << name << " heals for " << amount << " HP!\n";
}

void Player::addItem(const std::string& item) {
    inventory.push_back(item);
}

std::string Player::showInventory() const {
    if (inventory.empty()) return name + "'s inventory is empty.";
    std::stringstream ss;
    ss << name << "'s Inventory: ";
    for (auto& item : inventory) ss << item << " ";
    return ss.str();
}

// ----------------------------
// Monster
// ----------------------------
Monster::Monster() : Monster("Goblin", 100, 15, 1, 10) {}
Monster::Monster(const std::string& n, int h, int a, unsigned char lvl, unsigned char crit)
    : Character(n, h, a, lvl, crit) {}

void Monster::attack(Character& target, int multiplier) {
    std::cout << name << " fiercely attacks!\n";
    Character::attack(target, multiplier);
}

// ----------------------------
// Game
// ----------------------------
Game::Game() {
    player = new Player();
    monsters.push_back(new Monster("Goblin", 80, 12, 1, 10));
    monsters.push_back(new Monster("Orc", 120, 18, 2, 15));
    monsters.push_back(new Monster("Troll", 150, 20, 3, 5));
}

Game::~Game() {
    delete player;
    for (auto& m : monsters) delete m;
    monsters.clear();
}

Player* Game::getPlayer() const {
    return dynamic_cast<Player*>(player);
}

void Game::showAllMonsters() const {
    std::cout << "Monsters in the game:\n";
    for (auto& m : monsters)
        std::cout << "- " << m->getName() << " (HP: " << m->getHealth() << ")\n";
}

void Game::start() {
    showAllMonsters();

    for (auto& m : monsters) {
        if (!m) continue;
        std::cout << "\nNext battle: " << m->getName() << "\n";

        while(player && player->isAlive() && m->isAlive()) {
            player->attack(*m);
            BattleLogger::logStatus(*m);

            if (!m->isAlive()) break;

            m->attack(*player);
            BattleLogger::logStatus(*player);

            if (player->getHealth() < 40) {
                if (Player* p = dynamic_cast<Player*>(player)) {
                    p->heal();
                }
                BattleLogger::logStatus(*player);
            }

            std::cout << "--------------------\n";
        }

        if (!player || !player->isAlive()) break;
    }

    if (player) {
        if (Player* p = dynamic_cast<Player*>(player)) {
            std::cout << "\n" << p->showInventory() << "\n";
        }
    }

    std::cout << "\nWinner: "
              << (player && player->isAlive() ? player->getName() : "Monsters")
              << "\n";
}

} // namespace rpg
