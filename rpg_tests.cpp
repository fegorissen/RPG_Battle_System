#include "rpg_classes.h"
#include <cassert>
#include <iostream>

using namespace rpg;

int main() {
    Player p("Tester", 50, 10, 1, 0);
    p.heal(20);
    assert(p.getHealth() == 70);

    Monster m("Dummy", 50, 10, 1, 0);
    p.attack(m);
    assert(m.getHealth() <= 50);

    std::cout << "All tests passed!\n";
    return 0;
}
