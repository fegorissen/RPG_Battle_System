#include <iostream>
#include <string>
#include <vector>

// ----------------------------
// Abstracte basisklasse Character
// ----------------------------
// Vraag 3: useful and correct class (explain why)
// Vraag 4: useful and correct abstraction (explain why)
// Vraag 5: useful and correct encapsulation (explain why)
// Vraag 6: useful and correct inheritance (explain waarom) -> voor Player en Monster
// Vraag 7: useful and correct polymorphism (via virtual attack())
// Vraag 8: useful and correct object composition (indirect, via Game later)
class Character {
protected:
    std::string name;    // Vraag 5: encapsulation
    int health;          // Vraag 5: encapsulation
    int attackPower;     // Vraag 5: encapsulation

public:
    // Parameterized constructor
    Character(const std::string& n, int h, int a) : name(n), health(h), attackPower(a) {} // Vraag 3

    // Copy constructor
    Character(const Character& other) : name(other.name), health(other.health), attackPower(other.attackPower) {} // Vraag 3

    // Virtual destructor (Correct OOP practice)
    virtual ~Character() {} // Vraag 7

    // Getters & setters (Encapsulation)
    std::string getName() const { return name; } // Vraag 5
    int getHealth() const { return health; }    // Vraag 5
    void setHealth(int h) { health = h; }       // Vraag 5

    // Virtual function (Polymorphism)
    virtual void attack(Character& target) {    // Vraag 7
        target.health -= attackPower;
        std::cout << name << " attacks " << target.getName()
                  << " for " << attackPower << " damage!\n";
    }

    bool isAlive() const { return health > 0; } // Vraag 5
};

// ----------------------------
// Player class (erft van Character)
// ----------------------------
// Vraag 3: useful and correct class
// Vraag 6: inheritance
// Vraag 7: polymorphism (override attack)
// Vraag 8: object composition (via inventory)
class Player : public Character {
private:
    std::vector<std::string> inventory; // Vraag 8: object composition

public:
    Player(const std::string& n, int h, int a) : Character(n, h, a) {} // Vraag 3

    void addItem(const std::string& item) { inventory.push_back(item); } // Vraag 8

    void showInventory() const { // Vraag 8
        std::cout << name << "'s Inventory: ";
        for (auto& item : inventory) std::cout << item << " ";
        std::cout << "\n";
    }

    // Override attack (Polymorphism)
    void attack(Character& target) override { // Vraag 7
        std::cout << name << " bravely attacks!\n";
        Character::attack(target); // base class function called
    }
};

// ----------------------------
// Monster class (erft van Character)
// ----------------------------
// Vraag 3: useful and correct class
// Vraag 6: inheritance
// Vraag 7: polymorphism (override attack)
class Monster : public Character {
public:
    Monster(const std::string& n, int h, int a) : Character(n, h, a) {} // Vraag 3

    void attack(Character& target) override { // Vraag 7
        std::cout << name << " fiercely attacks!\n";
        Character::attack(target);
    }
};

// ----------------------------
// Game class (object composition)
// ----------------------------
// Vraag 3: useful and correct class
// Vraag 8: object composition
class Game {
private:
    Player player;   // Vraag 8: object composition
    Monster monster; // Vraag 8: object composition

public:
    Game() : player("Hero", 100, 20), monster("Goblin", 50, 10) {} // Vraag 3

    void start() {
        std::cout << "Battle begins between "
                  << player.getName() << " and " << monster.getName() << "!\n";

        // Simple battle loop
        while (player.isAlive() && monster.isAlive()) {
            player.attack(monster);  // Vraag 7: polymorphism
            if (monster.isAlive())
                monster.attack(player); // Vraag 7: polymorphism
        }

        if (player.isAlive())
            std::cout << player.getName() << " wins!\n";
        else
            std::cout << monster.getName() << " wins!\n";
    }
};

// ----------------------------
// main functie
// ----------------------------
// Clean main, geen OOP fouten
int main() {
    Game game;      // Vraag 8: object composition
    game.start();   // Clean main
    return 0;
}
