#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

// ----------------------------
// Abstracte basisklasse Character
// ----------------------------
class Character {
protected:
    std::string name;
    int health;
    int attackPower;
    const int maxHealth;

public:
    // Parameterized constructor
    Character(const std::string& n, int h, int a)
        : name(n), health(h), attackPower(a), maxHealth(h) {}

    // Copy constructor
    Character(const Character& other)
        : name(other.name), health(other.health), attackPower(other.attackPower), maxHealth(other.maxHealth) {}

    // Virtual destructor
    virtual ~Character() {}

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void setHealth(int h) { health = h; }

    virtual void attack(Character& target) {
        static std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> damageDist(static_cast<int>(attackPower * 0.8), static_cast<int>(attackPower * 1.2));

        int damage = damageDist(rng);
        target.health -= damage;
        if (target.health < 0) target.health = 0;

        std::cout << name << " attacks " << target.getName()
                  << " for " << damage << " damage! (Target HP: " << target.getHealth() << ")\n";
    }

    bool isAlive() const { return health > 0; }
};

// ----------------------------
// Player class
// ----------------------------
class Player : public Character {
private:
    std::vector<std::string> inventory;

public:
    // Default constructor
    Player() : Character("DefaultHero", 100, 12) {}

    // Parameterized constructor
    Player(const std::string& n, int h, int a) : Character(n, h, a) {}

    // Copy constructor
    Player(const Player& other) : Character(other), inventory(other.inventory) {}

    // Destructor
    ~Player() {
        std::cout << name << " has been destroyed.\n";
    }

    void addItem(const std::string& item) { inventory.push_back(item); }

    void showInventory() const {
        std::cout << name << "'s Inventory: ";
        for (auto& item : inventory) std::cout << item << " ";
        std::cout << "\n";
    }

    void attack(Character& target) override {
        std::cout << name << " bravely attacks!\n";
        Character::attack(target);
    }
};

// ----------------------------
// Monster class
// ----------------------------
class Monster : public Character {
public:
    // Default constructor
    Monster() : Character("DefaultGoblin", 100, 10) {}

    // Parameterized constructor
    Monster(const std::string& n, int h, int a) : Character(n, h, a) {}

    // Copy constructor
    Monster(const Monster& other) : Character(other) {}

    // Destructor
    ~Monster() {
        std::cout << name << " has been destroyed.\n";
    }

    void attack(Character& target) override {
        std::cout << name << " fiercely attacks!\n";
        Character::attack(target);
    }
};

// ----------------------------
// Game class
// ----------------------------
class Game {
private:
    Player player;
    Monster monster;

public:
    Game() : player("Hero", 100, 12), monster("Goblin", 100, 10) {}

    void start() {
        std::cout << "Battle begins between " << player.getName()
        << " (HP: " << player.getHealth() << "/" << player.getMaxHealth() << ") and "
        << monster.getName() << " (HP: " << monster.getHealth() << "/" << monster.getMaxHealth() << ")!\n\n";

        std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> coin(0,1);
        bool playerTurn = coin(rng) == 0;

        while (player.isAlive() && monster.isAlive()) {
            if (playerTurn) {
                player.attack(monster);
                if (monster.isAlive())
                    monster.attack(player);
            } else {
                monster.attack(player);
                if (player.isAlive())
                    player.attack(monster);
            }
            playerTurn = !playerTurn;
        }

        std::cout << "\nBattle Result: ";
        if (player.isAlive())
            std::cout << player.getName() << " wins!\n";
        else
            std::cout << monster.getName() << " wins!\n";
    }
};

// ----------------------------
// main functie
// ----------------------------
int main() {
    Game game;
    game.start();
    return 0;
}
