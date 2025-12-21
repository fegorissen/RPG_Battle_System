#include "rpg_classes.h"

#ifndef UNIT_TEST
int main() {
    try {
        rpg::Game game;

        std::string sword = "Sword";
        std::string shield = "Shield";

        if (rpg::Player* p = game.getPlayer()) {
            p->addItem(sword);
            p->addItem(shield);
        }

        game.start();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown error occurred!\n";
    }
    return 0;
}
#endif
