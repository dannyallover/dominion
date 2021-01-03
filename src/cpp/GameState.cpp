/* DOMINION
 * David Mally, Richard Roberts
 * GameState.cpp
 * Defines functions for managing game state;
 * phases (action/treasure/buy/cleanup), scoring,
 * showing the splash screen, setting text color, etc.
 */

#include <iostream>
#include <vector>

#include "Card.h"
#include "ActionCard.h"
#include "RandUtils.h"
#include "GameState.h"
#include "CardLookup.h"
#include "Pile.h"
#include "Player.h"
#include "Defs.h"

void game_state::SetColor(const char color[]) {
    std::cout << color;
}

void game_state::SetColorByType(CardType type) {
    switch(type) {
        case ACTION:
            game_state::SetColor(GRAY);
            break;
        case ATTACK:
            game_state::SetColor(RED);
            break;
        case REACTION:
            game_state::SetColor(LIGHT_BLUE);
            break;
        case TREASURE_C:
            game_state::SetColor(YELLOW);
            break;
        case VICTORY:
            game_state::SetColor(LIGHT_GREEN);
            break;
        case BASE:
            game_state::SetColor(WHITE);
            break;
    }
}

void game_state::ResetColor(void) {
    std::cout << WHITE << RESET_BG << std::endl;
}

int game_state::SplashScreen(void) {
    game_state::SetColor(BLUE);
    game_state::SetColor(GREEN_BG);
    std::cout << LOGO << std::endl;
    game_state::ResetColor();
    int cmd = DEF_CHOICE;
    char pause;
    while(cmd < 0) {
        std::cout << "Enter a choice below: " << std::endl;
        std::cout << "(0) Start new game" << std::endl
                  << "(1) List rules" << std::endl
                  << "(2) Play demo game" << std::endl
                  << "(3) Exit" << std::endl;
        std::cin >> cmd;
        lookup::CheckInvalidChoice(SPLASH_CHOICES, &cmd);
        if(cmd < 0) {
            std::cout << "Invalid choice. Please try again." << std::endl;
        } else {
            switch(cmd) {
                case NORMAL_GAME:
                    return NORMAL_GAME;
                    break;
                case LIST_RULES:
                    game_state::ListRules();
                    std::cin >> pause;
                    lookup::ClearCinError();
                    break;
                case DEMO_GAME:
                    return DEMO_GAME;
                    break;
                case EXIT_GAME:
                    std::cout << "Exiting..." << std::endl;
                    exit(0);
                    break;
            }
            cmd = DEF_CHOICE;
        }
    }
    return 0;
}

// Pick 10 random kingdom cards
std::vector<Card> game_state::RandomizeKingdom(std::vector<Card> cardSet) {
    std::vector<Card> randCards;

    std::vector<int> randVals = rand_utils::GenPseudoRandList(cardSet.size(), KINGDOM_SIZE);

    for(size_t i = 0; i < randVals.size(); i++) {
        randCards.push_back(cardSet.at(randVals.at(i)));
    }
    return randCards;
}

std::vector<Pile> game_state::GenerateKingdom(std::vector<Card> cardSet) {
    std::vector<Card> randCards = RandomizeKingdom(cardSet);
    std::vector<Pile> kingdomPiles;

    // Generate action card piles
    for(size_t i = 0; i < randCards.size(); i++) {
        Pile tmpPile(KINGDOM);
        if(randCards.at(i).GetType() == VICTORY) {
            tmpPile = Pile(KINGDOM, randCards.at(i), VICTORY_PILE_SIZE);
        } else {
            tmpPile = Pile(KINGDOM, randCards.at(i), PILE_SIZE);
        }
        kingdomPiles.push_back(tmpPile);
    }

    // Generate victory/curse card piles
    Pile victoryPile = Pile(KINGDOM, lookup::estate, VICTORY_PILE_SIZE,
                           "estate");
    kingdomPiles.push_back(victoryPile);

    victoryPile = Pile(KINGDOM, lookup::duchy, VICTORY_PILE_SIZE,
                           "duchy");
    kingdomPiles.push_back(victoryPile);

    victoryPile = Pile(KINGDOM, lookup::province, VICTORY_PILE_SIZE,
                      "province");
    kingdomPiles.push_back(victoryPile);

    victoryPile = Pile(KINGDOM, lookup::curse, PILE_SIZE, "curse");
    kingdomPiles.push_back(victoryPile);

    // Generate treasure card piles
    Pile treasurePile = Pile(TREASURE, lookup::copper, COPPER_PILE_SIZE,
                            "copper");
    kingdomPiles.push_back(treasurePile);

    treasurePile = Pile(TREASURE, lookup::silver, SILVER_PILE_SIZE,
                       "silver");
    kingdomPiles.push_back(treasurePile);

    treasurePile = Pile(TREASURE, lookup::gold, GOLD_PILE_SIZE,
                       "gold");
    kingdomPiles.push_back(treasurePile);

    return kingdomPiles;
}

void game_state::ActionPhase(struct stateBlock *state, bool p1) {
    Player *currPlayer = p1 ? state->p1 : state->p2;
    Player *otherPlayer = p1 ? state->p2 : state->p1;
    int cmd;
    system(CLEAR); // Clear the console
    PromptActionPhase();
    Pile hand = currPlayer->GetHand();
    while(currPlayer->GetActions() > 0 && cmd > DEF_CHOICE) {
        do {
            std::cout << "You have " << currPlayer->GetActions()
                  << " action(s) remaining." << std::endl;
            hand.PrintPileAsHand();
            std::cin >> cmd;
            lookup::CheckInvalidChoice(hand.Size(), &cmd);
        } while (cmd == BAD_CHOICE);
        if(cmd > DEF_CHOICE) {
            Card *cardPlayed = currPlayer->HandPtr()->DrawAt(cmd);
            CardType type = cardPlayed->GetType();
            if(type == ACTION || type == ATTACK || type == REACTION) {
                currPlayer->AddActions(-1);
                game_state::HandleCardAdditions(currPlayer,
                                                cardPlayed);
                bool trashedSelf = false;
                if(type == ATTACK) {
                    // Check for moat before applying attack effects
                    if(!CheckMoat(otherPlayer->GetHand())) {
                        if(cardPlayed->GetEffect() != NULL) {
                            trashedSelf = cardPlayed->PlayEffect(state, p1);
                        }
                    } else {
                        SetColor(PURPLE);
                        std::cout << "Foe's moat nullfies your attack"
                                  << " effect!" << std::endl;
                        SetColor(WHITE);
                    }
                } else {
                    // Handle the card's effect
                    if(cardPlayed->GetEffect() != NULL) {
                        trashedSelf = cardPlayed->PlayEffect(state, p1);
                    }
                }
                // Trash the card if it trashes itself,
                // else discard the card
                game_state::ActionPhaseCleanup(currPlayer, state->trash,
                                               cardPlayed, trashedSelf);
                hand = currPlayer->GetHand();
            }
        }
    }
    std::cout << PROMPT << std::endl;
    char pause;
    std::cin >> pause;
    lookup::ClearCinError();
}

void game_state::TreasurePhase(struct stateBlock *state, bool p1) {
    Player *currPlayer = p1 ? state->p1 : state->p2;
    Pile hand = currPlayer->GetHand();
    int cmd;
    system(CLEAR); // Clear the console
    game_state::SetColor(YELLOW);
    std::cout << "Treasure phase: choose card(s) to play "
              << "(or -1 to skip phase):" << std::endl;
    game_state::ResetColor();
    do {
        do {
            std::cout << "You have " << currPlayer->GetCoins()
                    << " coins(s)." << std::endl;
            std::cout << "Hand:" << std::endl;
            for(size_t i = 0; i < hand.Size(); i++) {
                game_state::SetColorByType(hand.At(i)->GetType());
                std::cout << i << ": " << hand.At(i)->ToString() << std::endl;
            }
            game_state::ResetColor();
            std::cin >> cmd;
            lookup::CheckInvalidChoice(hand.Size(), &cmd);
        } while (cmd == BAD_CHOICE);

        if(cmd != DEF_CHOICE &&
          currPlayer->GetHand().At(cmd)->GetType() == TREASURE_C) {
            // If a card is a treasure card, add its +coins
            Card *cardPlayed = currPlayer->GetHand().At(cmd);
            currPlayer->AddCoins(cardPlayed->GetCoins());
            // and then discard it.
            currPlayer->DiscardCard(cmd);
            hand = currPlayer->GetHand();
        }
    } while(cmd != DEF_CHOICE);
}

void game_state::BuyPhase(struct stateBlock *state, bool p1) {
    Player *currPlayer = p1 ? state->p1 : state->p2;
    int idx;
    system(CLEAR); // Clear the console
    game_state::SetColor(BROWN);
    std::cout << "Buy phase: choose card(s) to buy "
              << "(or -1 to skip phase):" << std::endl;
    game_state::ResetColor();
    do {
        do{
            std::cout << "Buyable cards:" << std::endl;
            for(size_t i = 0; i < state->kingdom->size(); i++) {
                game_state::SetColorByType(state->kingdom->at(i).GetTopCard()->GetType());
                std::cout << "Card " << i << ": "
                          << state->kingdom->at(i).GetTopCard()->ToString()
                          << "   ";
                if((i+1) % 5 == 0) {
                    std::cout << std::endl;
                }
            }
            game_state::ResetColor();
            std::cout << "\nYou have " << currPlayer->GetCoins()
                      << " coins(s) and " << currPlayer->GetBuys()
                      << " buy(s) remaining." << std::endl;
            std:: cout << "Type a card's number to buy it." << std::endl;
            std::cin >> idx;
            lookup::CheckInvalidChoice(state->kingdom->size(), &idx);
        } while(idx == BAD_CHOICE);
        // Buy a card if it costs <= currPlayer's coins.
        if(idx == DEF_CHOICE) {}
        else if (state->kingdom->at(idx).GetTopCard()->GetCost() <=
                currPlayer->GetCoins()) {
            currPlayer->AddCoins(-1*state->kingdom->at(idx).GetTopCard()->
                                   GetCost());
            state->kingdom->at(idx).Move(0, currPlayer->DiscardPtr());
            currPlayer->AddBuys(-1);
        } else {
            std::cout << "You don't have enough coins to buy that!"
                      << std::endl;
        }
    } while(idx != DEF_CHOICE && currPlayer->GetBuys() > 0);
}

void game_state::CleanupPhase(struct stateBlock *state, bool p1) {
    system(CLEAR); // Clear the console
    Player *currPlayer = p1 ? state->p1 : state->p2;
    game_state::SetColor(LIGHT_PURPLE);
    std::cout << "Cleanup phase: all cards in-hand and in-play "
              << "discarded" << std::endl;
    game_state::ResetColor();
    currPlayer->DiscardPtr()->TakeAllFrom(currPlayer->HandPtr());
    std::cout << "Drawing 5 new cards..." << std::endl;
    std::cout << "Type \"OK\" to pass to your opponent." << std::endl;
    char pause;
    std::cin >> pause;
    lookup::ClearCinError();
    // Reset currPlayer's actions/buys/coins and draw 5 cards
    currPlayer->SetNewTurn();
}

int game_state::ScoreDeck(Pile playerPile) {
    int score = 0;
    std::vector<Card *> deck = playerPile.GetCards();
    for(size_t i = 0; i < deck.size(); i++) {
        if(deck.at(i)->GetName() == "gardens") {
            score += ScoreGardens(deck.size());
        } else {
            score += deck.at(i)->GetPoints();
        }
    }
    return score;
}

int game_state::ScoreGardens(int numCards) {
    return numCards / SCORE_GARDEN;
}

bool game_state::GameOver(std::vector<Pile> &kingdomCards) {
    system(CLEAR); // Clear the console
    int numEmpty = 0;
    for(size_t i = 0; i < kingdomCards.size(); i++) {
        if(kingdomCards.at(i).Size() == 0) {
            numEmpty += 1;
        }
        if(kingdomCards.at(i).GetName() == "province" &&
           kingdomCards.at(i).Size() == 0) {
            return true;
        }
    }
    return numEmpty >= MAX_NUM_EMPTY;
}

bool game_state::CheckMoat(Pile otherHand) {
    return otherHand.LookThrough(new Card(lookup::moat)) > -1;
}

void game_state::PromptActionPhase(void) {
    SetColor(GREEN);
    std::cout << "Action phase: choose card(s) to play "
              << "(or -1 to skip phase):" << std::endl;
    SetColor(WHITE);
}

void game_state::HandleCardAdditions(Player *player, Card *cardPlayed) {
    //ActionCard tmp = static_cast<ActionCard>(*cardPlayed);
    // Add the card's +actions
    player->AddActions(cardPlayed->GetActions());
    // Add the card's +buys
    player->AddBuys(cardPlayed->GetBuys());
    // Add the card's +cards
    for(int i = 0; i < cardPlayed->GetCards(); i++) {
        player->DrawCard();
    }
    // Add the card's +coins
    player->AddCoins(cardPlayed->GetCoins());
}

void game_state::ActionPhaseCleanup(Player *player, Pile *trash,
                                    Card *cardPlayed, bool trashedSelf) {
    if(trashedSelf) {
        trash->TopDeck(cardPlayed);
    } else {
        player->AddToDiscard(cardPlayed);
    }
}

void game_state::ListRules(void) {
    char pause;
    std::cout << PLOT_TEXT << std::endl
              << PROMPT << std::endl;
    std::cin >> pause;
    lookup::ClearCinError();
    std::cout << RULES_TEXT_1 << std::endl
              << PROMPT << std::endl;
    std::cin >> pause;
    lookup::ClearCinError();
    std::cout << RULES_TEXT_2 << std::endl
              << PROMPT << std::endl;
}

void game_state::SetDemoCards(struct stateBlock *state) {
    Card *tmpCard = new Card(lookup::village);
    state->p1->AddToHand(tmpCard);
    tmpCard = new Card(lookup::mine);
    state->p1->AddToHand(tmpCard);
    tmpCard = new Card(lookup::militia);
    state->p1->AddToHand(tmpCard);
    tmpCard = new Card(lookup::gold);
    state->p1->AddToHand(tmpCard);

    tmpCard = new Card(lookup::moneylender);
    state->p2->AddToHand(tmpCard);
    tmpCard = new Card(lookup::market);
    state->p2->AddToHand(tmpCard);
    tmpCard = new Card(lookup::moat);
    state->p2->AddToHand(tmpCard);
    tmpCard = new Card(lookup::gold);
    state->p2->AddToHand(tmpCard);
}
