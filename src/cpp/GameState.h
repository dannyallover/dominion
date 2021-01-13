/* DOMINION
 * David Mally, Richard Roberts
 * GameState.h
 * Defines functions for managing game state;
 * phases (action/treasure/buy/cleanup), scoring,
 * showing the splash screen, setting text color, etc.
 */

#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <vector>

#include "Card.h"
#include "ActionCard.h"
#include "Pile.h"
#include "TreasureCard.h"
#include "VictoryCard.h"
#include "Player.h"

#define KINGDOM_SIZE 10
#define PILE_SIZE 10
#define VICTORY_PILE_SIZE 8
#define COPPER_PILE_SIZE 100
#define SILVER_PILE_SIZE 40
#define GOLD_PILE_SIZE 30
#define TRASH_PILE_SIZE 0

#define SCORE_GARDEN 10

#define MAX_NUM_EMPTY 3

#define NORMAL_GAME 0
#define LIST_RULES  1
#define DEMO_GAME   2
#define EXIT_GAME   3
#define SPLASH_CHOICES 4


#define CLEAR "clear"

namespace game_state {
    void SetColor(const char color[]);
    void SetColorByType(CardType type);
    void ResetColor(void);
    int SplashScreen(void);
    std::vector<Card> RandomizeKingdom(std::vector<Card> cardSet);
    std::vector<Pile> GenerateKingdom(std::vector<Card> cardSet);
    bool ActionPhase(struct stateBlock *state, bool p1);
    void TreasurePhase(struct stateBlock *state, bool p1, bool merchant);
    void BuyPhase(struct stateBlock *state, bool p1);
    void CleanupPhase(struct stateBlock *state, bool p1);
    int ScoreDeck(Pile playerPile);
    int ScoreGardens(int numCards);
    bool GameOver(std::vector<Pile> &kingdomCards);
    bool CheckMoat(Pile otherHand);
    void PromptActionPhase(void);
    void ActionPhaseCleanup(Player *player, Pile *trash,
                            Card *cardPlayed, bool trashedSelf);
    void HandleCardAdditions(Player *player, Card *cardPlayed);
    void ListRules(void);
    void SetDemoCards(struct stateBlock *state);
}

#endif
