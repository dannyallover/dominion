/* DOMINION
 * David Mally, Richard Roberts
 * main.cpp
 * Contains main function that runs the main game loop.
 */
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <ncurses.h>

#include "Card.h"
#include "ActionCard.h"
#include "Defs.h"
#include "ActionCard.h"
#include "Pile.h"
#include "RandUtils.h"
#include "CardLookup.h"
#include "Player.h"
#include "GameState.h"

#define P1_NUM 1
#define P2_NUM 2

int main(void) {
    Pile trash(TRASH);
    // Initialize kingdom piles (buyable cards)
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    // Initialize players
    Player p1 = Player(P1_NUM, "p1");
    Player p2 = Player(P2_NUM, "p2");
    Player *currPlayer = &p1;

    // Initialize game state shared object
    struct stateBlock state;
    state.p1        = &p1;
    state.p2        = &p2;
    state.trash     = &trash;
    state.kingdom   = &kingdomCards;

    system(CLEAR);
    int gameMode = game_state::SplashScreen();
    if(gameMode == DEMO_GAME) {
        game_state::SetDemoCards(&state);
    }

    // Game event loop
    while(!game_state::GameOver(kingdomCards)) {
        bool p1Turn = (currPlayer == &p1);
        int pNum = p1Turn ? P1_NUM : P2_NUM;
        game_state::SetColor(LIGHT_BLUE);
        std::cout << "Player " << pNum << ", begin your turn."
                  << " (Type \"OK\" to continue.)" << std::endl;
        game_state::ResetColor();
        char pause;
        std::cin >> pause;
        lookup::ClearCinError();
        game_state::ResetColor();
        game_state::ActionPhase(&state, p1Turn);
        game_state::TreasurePhase(&state, p1Turn);
        game_state::BuyPhase(&state, p1Turn);
        game_state::CleanupPhase(&state, p1Turn);
        // Swap current player for next turn
        if(p1Turn) { currPlayer = &p2; }
        else { currPlayer = &p1; }
    }
    // Consolidate all cards into decks
    p1.DeckPtr()->TakeAllFrom(p1.HandPtr());
    p1.DeckPtr()->TakeAllFrom(p1.DiscardPtr());
    p2.DeckPtr()->TakeAllFrom(p2.HandPtr());
    p2.DeckPtr()->TakeAllFrom(p2.DiscardPtr());
    // Score decks
    int scoreP1 = game_state::ScoreDeck(p1.GetDeck());
    int scoreP2 = game_state::ScoreDeck(p2.GetDeck());
    if(scoreP1 > scoreP2) {
        std::cout << "Player 1 wins!" << std::endl;
    } else {
        std::cout << "Player 2 wins!" << std::endl;
    }
    std::cout << "Scores:\n\tPlayer 1: " << std::to_string(scoreP1)
              << "\n\tPlayer 2: " << std::to_string(scoreP2)
              << std::endl;
    return 0;
}
