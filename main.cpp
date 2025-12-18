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
    Character(const std::string& n, int h, int a)
        : name(n), health(h), attackPower(a), maxHealth(h) {}

    Character(const Character& other)
        : name(other.name), health(other.health), attackPower(other.attackPower), maxHealth(other.maxHealth) {}

    virtual ~Character() {}

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void setHealth(int h) { health = h; }

    virtual void attack(Character& target, int multiplier = 1) { // Standaardwaarde toegevoegd
        static std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> damageDist(static_cast<int>(attackPower * 0.8 * multiplier),
                                                      static_cast<int>(attackPower * 1.2 * multiplier));
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
    Player() : Player("DefaultHero", 100, 12) {}
    Player(const std::string& n, int h, int a) : Character(n, h, a), inventory() {}
    Player(const Player& other) : Character(other), inventory(other.inventory) {}
    ~Player() { std::cout << name << " has been destroyed.\n"; }

    void addItem(const std::string& item) { inventory.push_back(item); }
    void showInventory() const {
        std::cout << name << "'s Inventory: ";
        for (auto& item : inventory) std::cout << item << " ";
        std::cout << "\n";
    }

    void attack(Character& target, int multiplier = 1) override { // polymorfisme + standaardwaarde
        std::cout << name << " bravely attacks!\n";
        Character::attack(target, multiplier);
    }

    // Nuttige lidfunctie: heal met standaardwaarde
    void heal(int amount = 20) { // standaardwaarde 20 HP
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP! (Current HP: " << health << ")\n";
    }
};

// ----------------------------
// Monster class
// ----------------------------
class Monster : public Character {
public:
    Monster() : Monster("DefaultGoblin", 100, 10) {}
    Monster(const std::string& n, int h, int a) : Character(n, h, a) {}
    Monster(const Monster& other) : Character(other) {}
    ~Monster() { std::cout << name << " has been destroyed.\n"; }

    void attack(Character& target, int multiplier = 1) override {
        std::cout << name << " fiercely attacks!\n";
        Character::attack(target, multiplier);
    }
};

// ----------------------------
// Game class (object composition)
// ----------------------------
class Game {
private:
    Character* player;
    Character* monster;

public:
    Game() {
        player = new Player("Hero", 100, 12);
        monster = new Monster("Goblin", 100, 10);
    }

    ~Game() {
        delete player;
        delete monster;
    }

    void start() {
        std::cout << "Battle begins between " << player->getName()
        << " (HP: " << player->getHealth() << "/" << player->getMaxHealth() << ") and "
        << monster->getName() << " (HP: " << monster->getHealth() << "/" << monster->getMaxHealth() << ")!\n\n";

        std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<int> coin(0,1);
        bool playerTurn = coin(rng) == 0;

        while (player->isAlive() && monster->isAlive()) {
            if (playerTurn) {
                player->attack(*monster); // polymorfie
                if (monster->isAlive()) monster->attack(*player);
            } else {
                monster->attack(*player);
                if (player->isAlive()) player->attack(*monster);
            }

            // Voorbeeld: Player geneest soms automatisch (optioneel)
            if (playerTurn && player->getHealth() < 50) {
                dynamic_cast<Player*>(player)->heal(); // standaard heal van 20 HP
            }

            playerTurn = !playerTurn;
        }

        std::cout << "\nBattle Result: ";
        if (player->isAlive())
            std::cout << player->getName() << " wins!\n";
        else
            std::cout << monster->getName() << " wins!\n";
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
