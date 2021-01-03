/* DOMINION
 * David Mally, Richard Roberts
 * mainTest.cpp
 * Contains all unit tests.
 */
#include "ActionCard.h"
#include "Card.h"
#include "CardLookup.h"
#include "Defs.h"
#include "GameState.h"
#include "Pile.h"
#include "Player.h"
#include "RandUtils.h"
#include "TreasureCard.h"
#include "VictoryCard.h"

#include "gtest/gtest.h"

#include <iostream>

using namespace std;
using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

namespace {

vector<Card> randCards;

TEST(PileTest, Constructor1) {
    Pile p1 = Pile(KINGDOM);
    EXPECT_EQ(0, p1.getCards().size());
    EXPECT_EQ(KINGDOM, p1.getOwner());
}

TEST(PileTest, Constructor2) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    EXPECT_EQ(10, p2.getCards().size());
    EXPECT_EQ(KINGDOM, p2.getOwner());
}

TEST(PileTest, EmptyDeck) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    p2.emptyDeck();
    EXPECT_EQ(0, p2.getCards().size());
}

TEST(PileTest, getTopCard) {
    Pile p2 = Pile(KINGDOM, lookup::village, 10);
    Card *card = p2.getTopCard();
    EXPECT_EQ(10, p2.getCards().size());
    EXPECT_EQ("village", card->GetName());
}

TEST(PileTest, topDeck) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    p2.topDeck(new ActionCard(lookup::market));
    EXPECT_EQ(11, p2.getCards().size());
    EXPECT_EQ("market", p2.getTopCard()->GetName());
}

TEST(PileTest, lookThrough) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    p2.topDeck(new ActionCard(lookup::market));
    p2.printPileAsHand();
    int idx = p2.lookThrough(new ActionCard(lookup::village));
    EXPECT_EQ(0, idx);
    idx = p2.lookThrough(new VictoryCard(lookup::gardens));
    EXPECT_EQ(-1, idx);
}

// This test can nondeterministically fail due to RNJesus
TEST(PileTest, trueShuffle) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 1);
    p2.topDeck(new ActionCard(lookup::market));
    p2.topDeck(new VictoryCard(lookup::gardens));
    p2.topDeck(new ActionCard(lookup::workshop));
    p2.topDeck(new ActionCard(lookup::witch));
    p2.topDeck(new ActionCard(lookup::smithy));
    p2.topDeck(new ActionCard(lookup::councilroom));
    p2.topDeck(new ActionCard(lookup::adventurer));
    p2.topDeck(new ActionCard(lookup::woodcutter));
    p2.topDeck(new ActionCard(lookup::throneroom));
    p2.trueShuffle();
    EXPECT_NE(p2.getTopCard()->GetName(), "market");
}

TEST(PileTest, takeAllFrom) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    Pile p3 = Pile(KINGDOM, (Card)lookup::market, 10);
    p2.takeAllFrom(&p3);
    EXPECT_EQ(p2.getCards().size(), 20);
    EXPECT_EQ(p3.getCards().size(), 0);
    EXPECT_EQ(p2.getTopCard()->GetName(), "market");
}

TEST(PileTest, moveCard) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    Pile p3 = Pile(KINGDOM);
    p2.move(new Card(lookup::village), &p3);
    EXPECT_EQ(p2.getCards().size(), 9);
    EXPECT_EQ(p3.getCards().size(), 1);
}

TEST(PileTest, moveIndex) {
    Pile p2 = Pile(KINGDOM, (Card)lookup::village, 10);
    Pile p3 = Pile(KINGDOM);
    p2.move(5, &p3);
    EXPECT_EQ(p2.getCards().size(), 9);
    EXPECT_EQ(p3.getCards().size(), 1);
}

TEST(GameState, randomizeKingdom) {
    vector<Card> kingdomCards;
    kingdomCards.push_back(lookup::cellar);
    kingdomCards.push_back(lookup::chapel);
    kingdomCards.push_back(lookup::moat);
    kingdomCards.push_back(lookup::chancellor);
    kingdomCards.push_back(lookup::village);
    kingdomCards.push_back(lookup::woodcutter);
    kingdomCards.push_back(lookup::workshop);
    kingdomCards.push_back(lookup::bureaucrat);
    kingdomCards.push_back(lookup::feast);
    kingdomCards.push_back(lookup::gardens);
    kingdomCards.push_back(lookup::militia);
    kingdomCards.push_back(lookup::moneylender);
    kingdomCards.push_back(lookup::remodel);
    kingdomCards.push_back(lookup::smithy);
    kingdomCards.push_back(lookup::spy);
    kingdomCards.push_back(lookup::thief);
    kingdomCards.push_back(lookup::throneroom);
    kingdomCards.push_back(lookup::councilroom);
    kingdomCards.push_back(lookup::festival);
    kingdomCards.push_back(lookup::laboratory);
    kingdomCards.push_back(lookup::library);
    kingdomCards.push_back(lookup::market);
    kingdomCards.push_back(lookup::mine);
    kingdomCards.push_back(lookup::witch);
    kingdomCards.push_back(lookup::adventurer);

    randCards = game_state::RandomizeKingdom(kingdomCards);
    EXPECT_EQ(randCards.size(), 10);
    /*for(size_t i = 0; i < randCards.size(); i++) {
        cout << randCards.at(i).getName() << endl;
    }*/
}

TEST(GameState, generateKingdom) {
    vector<Pile> kingdomCards = game_state::GenerateKingdom(randCards);
    EXPECT_EQ(kingdomCards.size(), 10+4+3); // 10 kingdom, 4 victory, 3 treasure, 1 trash
}

TEST(GameState, scoreDeck) {
    Pile deck = Pile(PLAYER1);
    deck.topDeck(new VictoryCard(lookup::estate));
    deck.topDeck(new VictoryCard(lookup::estate));
    deck.topDeck(new VictoryCard(lookup::estate));
    deck.topDeck(new TreasureCard(lookup::copper));
    deck.topDeck(new TreasureCard(lookup::copper));
    deck.topDeck(new TreasureCard(lookup::copper));
    deck.topDeck(new TreasureCard(lookup::copper));
    deck.topDeck(new TreasureCard(lookup::copper));
    deck.topDeck(new TreasureCard(lookup::copper));
    deck.topDeck(new TreasureCard(lookup::copper));

    EXPECT_EQ(deck.getCards().size(), 10);
    EXPECT_EQ(game_state::ScoreDeck(deck), 3);
    deck.topDeck(new VictoryCard(lookup::duchy));
    deck.topDeck(new VictoryCard(lookup::province));

    EXPECT_EQ(deck.getCards().size(), 12);
    EXPECT_EQ(game_state::ScoreDeck(deck), 12);
}

TEST(GameState, scoreGardens) {
    int score10 = game_state::ScoreGardens(100);
    int score1  = game_state::ScoreGardens(14);
    int score0  = game_state::ScoreGardens(3);
    int score2  = game_state::ScoreGardens(22);

    EXPECT_EQ(score10, 10);
    EXPECT_EQ(score1, 1);
    EXPECT_EQ(score0, 0);
    EXPECT_EQ(score2, 2);
}


TEST(GameState, actionPhase) {
    Player p1 = Player(1);
    Player p2 = Player(2);
    Pile trash = Pile(TRASH);
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    struct stateBlock state;
    state.p1 = &p1;
    state.p2 = &p2;
    state.trash = &trash;
    state.kingdom = &kingdomCards;

    // Assume phase is skipped
    game_state::ActionPhase(&state, true);

    EXPECT_EQ(p1.GetHand().size(), 5);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
    EXPECT_EQ(p2.GetHand().size(), 5);
    EXPECT_EQ(p2.GetDeck().size(), 5);
    EXPECT_EQ(p2.GetDiscard().size(), 0);
}

TEST(GameState, treasurePhase) {
    Player p1 = Player(1);
    Player p2 = Player(2);
    Pile trash = Pile(TRASH);
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    struct stateBlock state;
    state.p1 = &p1;
    state.p2 = &p2;
    state.trash = &trash;
    state.kingdom = &kingdomCards;

    // Assume all treasures played
    game_state::TreasurePhase(&state, true);

    EXPECT_LE(p1.GetHand().size(), 4);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_GE(p1.GetDiscard().size(), 1);
    EXPECT_EQ(p2.GetHand().size(), 5);
    EXPECT_EQ(p2.GetDeck().size(), 5);
    EXPECT_EQ(p2.GetDiscard().size(), 0);

    EXPECT_GE(p1.GetCoins(), 1);
}


TEST(GameState, buyPhase) {
    Player p1 = Player(1);
    Player p2 = Player(2);
    Pile trash = Pile(TRASH);
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    struct stateBlock state;
    state.p1 = &p1;
    state.p2 = &p2;
    state.trash = &trash;
    state.kingdom = &kingdomCards;

    // Assume nothing is bought
    game_state::BuyPhase(&state, true);

    EXPECT_EQ(p1.GetHand().size(), 5);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
    EXPECT_EQ(p2.GetHand().size(), 5);
    EXPECT_EQ(p2.GetDeck().size(), 5);
    EXPECT_EQ(p2.GetDiscard().size(), 0);
}

TEST(RandUtils, genPseudoRandList) {
    vector<int> pseudoRands = rand_utils::GenPseudoRandList(10, 10, rand());
    EXPECT_EQ(pseudoRands.size(), 10);
    for(size_t i = 0; i < pseudoRands.size(); i++) {
        for(size_t j = 0; j < pseudoRands.size(); j++) {
            if(i != j) {
                EXPECT_NE(pseudoRands.at(i), pseudoRands.at(j));
            }
        }
    }
}

TEST(CardLookup, cellarEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    state.p1 = &p1;

    lookup::CellarEffect(&state, true);

    EXPECT_EQ(p1.GetHand().size(), 5);
    EXPECT_EQ(p1.GetDiscard().size(), 5);
    EXPECT_EQ(p1.GetDeck().size(), 0);
}

TEST(CardLookup, chapelEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Pile trash = Pile(TRASH);
    state.p1 = &p1;
    state.trash = &trash;

    lookup::ChapelEffect(&state, true);

    EXPECT_EQ(p1.GetHand().size(), 1);
    EXPECT_EQ(trash.size(), 4);
}

TEST(CardLookup, chancellorEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    state.p1 = &p1;
    // deck->discard
    lookup::ChancellorEffect(&state, true);

    EXPECT_EQ(p1.GetDeck().size(), 0);
    EXPECT_EQ(p1.GetDiscard().size(), 5);

    p1 = Player(1);
    state.p1 = &p1;
    // don't deck->discard
    lookup::ChancellorEffect(&state, true);

    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
}

TEST(CardLookup, workshopEffect) {
    Pile chapels = Pile(KINGDOM, (Card)lookup::chapel, 10);
    Pile moats = Pile(KINGDOM, (Card)lookup::moat, 10);
    Pile chancellors = Pile(KINGDOM, (Card)lookup::chancellor, 10);
    Pile woodcutters = Pile(KINGDOM, (Card)lookup::woodcutter, 10);
    Pile workshops = Pile(KINGDOM, (Card)lookup::workshop, 10);
    Pile thiefs = Pile(KINGDOM, (Card)lookup::thief , 10);
    Pile councilrooms = Pile(KINGDOM, (Card)lookup::councilroom, 10);
    Pile markets = Pile(KINGDOM, (Card)lookup::market, 10);
    Pile mines = Pile(KINGDOM, (Card)lookup::mine, 10);
    Pile adventurers = Pile(KINGDOM, (Card)lookup::adventurer, 10);

    std::vector<Pile> kingdomPiles;
    kingdomPiles.push_back(chapels);
    kingdomPiles.push_back(moats);
    kingdomPiles.push_back(chancellors);
    kingdomPiles.push_back(woodcutters);
    kingdomPiles.push_back(workshops);
    kingdomPiles.push_back(thiefs);
    kingdomPiles.push_back(councilrooms);
    kingdomPiles.push_back(markets);
    kingdomPiles.push_back(mines);
    kingdomPiles.push_back(adventurers);

    Player p1 = Player(1);
    struct stateBlock state;
    state.p1 = &p1;
    state.kingdom = &kingdomPiles;

    lookup::WorkshopEffect(&state, true);

    EXPECT_EQ(kingdomPiles.size(), 10);
    EXPECT_EQ(kingdomPiles.at(1).size(), 9);
    EXPECT_EQ(p1.GetDiscard().size(), 1);

    lookup::WorkshopEffect(&state, true);
    EXPECT_EQ(kingdomPiles.size(), 10);
    EXPECT_EQ(kingdomPiles.at(1).size(), 8);
    EXPECT_EQ(p1.GetDiscard().size(), 2);

    lookup::WorkshopEffect(&state, true);
    EXPECT_EQ(kingdomPiles.size(), 10);
    EXPECT_EQ(kingdomPiles.at(5).size(), 9);
    EXPECT_EQ(p1.GetDiscard().size(), 3);
}

TEST(CardLookup, bureaucratEffect) {
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    Player p1 = Player(1);
    Player p2 = Player(2);
    struct stateBlock state;
    state.p1 = &p1;
    state.p2 = &p2;
    state.kingdom = &kingdomCards;

    lookup::BureaucratEffect(&state, true);

    EXPECT_EQ(p1.GetDeck().getTopCard()->GetName(), "silver");
    EXPECT_EQ(p1.GetDeck().size(), 6);
    EXPECT_EQ(p2.GetHand().size(), 4);
    EXPECT_EQ(p2.GetDeck().size(), 6);
    EXPECT_EQ(p2.GetDeck().getTopCard()->GetName(), "estate");
}

TEST(CardLookup, feastEffect) {
    Player p1 = Player(1);
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    Pile trash = Pile(TRASH);
    struct stateBlock state;
    state.p1 = &p1;
    state.kingdom = &kingdomCards;
    state.trash = &trash;

    bool trashed = lookup::FeastEffect(&state, true);

    EXPECT_EQ(p1.GetDiscard().size(), 1);
    EXPECT_LE(p1.GetDiscard().getTopCard()->GetCost(), 5);
    EXPECT_EQ(trashed, true);
    EXPECT_EQ(p1.GetHand().size(), 5);
}

TEST(CardLookup, militiaEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Player p2 = Player(2);
    state.p1 = &p1;
    state.p2 = &p2;
    lookup::MilitiaEffect(&state, true);
    EXPECT_EQ(p2.GetHand().size(), 3);
}

TEST(CardLookup, moneylenderEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Pile trash = Pile(TRASH);
    state.p1 = &p1;
    state.trash = &trash;
    lookup::MoneylenderEffect(&state, true);
    EXPECT_EQ(p1.GetCoins(), 3);
    EXPECT_EQ(p1.GetHand().size(), 4);
}

TEST(CardLookup, remodelEffect) {
    Player p1 = Player(1);
    Pile trash = Pile(TRASH);
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    struct stateBlock state;
    state.p1 = &p1;
    state.trash = &trash;
    state.kingdom = &kingdomCards;

    // Assume we trash an estate
    lookup::RemodelEffect(&state, true);

    EXPECT_LE(p1.GetHand().getTopCard()->GetCost(), 4);
    EXPECT_EQ(p1.GetHand().size(), 4);
    EXPECT_EQ(p1.GetDiscard().size(), 1);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(trash.size(), 1);
}

TEST(CardLookup, spyEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Player p2 = Player(2);

    state.p1 = &p1;
    state.p2 = &p2;

    // Don't discard either card
    lookup::SpyEffect(&state, true);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
    EXPECT_EQ(p1.GetHand().size(), 5);
    EXPECT_EQ(p2.GetDeck().size(), 5);
    EXPECT_EQ(p2.GetDiscard().size(), 0);
    EXPECT_EQ(p2.GetHand().size(), 5);

    // Discard both cards
    lookup::SpyEffect(&state, true);
    EXPECT_EQ(p1.GetDeck().size(), 4);
    EXPECT_EQ(p1.GetDiscard().size(), 1);
    EXPECT_EQ(p1.GetHand().size(), 5);
    EXPECT_EQ(p2.GetDeck().size(), 4);
    EXPECT_EQ(p2.GetDiscard().size(), 1);
    EXPECT_EQ(p2.GetHand().size(), 5);
}

TEST(CardLookup, thiefEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Player p2 = Player(2);
    Pile trash = Pile(TRASH);

    p2.DrawCard();
    p2.DrawCard();
    p2.DrawCard();
    p2.DrawCard();
    p2.DrawCard();
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));
    p2.AddToDeck(new Card(lookup::copper));

    state.p1 = &p1;
    state.p2 = &p2;
    state.trash = &trash;

    EXPECT_EQ(p2.GetDiscard().size(), 0);
    // Trash first card, discard 2nd, take trashed
    lookup::ThiefEffect(&state, true);

    EXPECT_EQ(trash.size(), 0);
    EXPECT_EQ(p2.GetDiscard().size(), 1);
    EXPECT_EQ(p2.GetDeck().size(), 8);
    EXPECT_EQ(p1.GetDiscard().size(), 1);

    // Trash 2nd card, discard 1st, take trashed
    lookup::ThiefEffect(&state, true);

    EXPECT_EQ(trash.size(), 0);
    EXPECT_EQ(p2.GetDiscard().size(), 2);
    EXPECT_EQ(p2.GetDeck().size(), 6);
    EXPECT_EQ(p1.GetDiscard().size(), 2);

    // Trash 1st card, discard 2nd, take nothing
    lookup::ThiefEffect(&state, true);
    EXPECT_EQ(trash.size(), 1);
    EXPECT_EQ(p2.GetDiscard().size(), 3);
    EXPECT_EQ(p2.GetDeck().size(), 4);
    EXPECT_EQ(p1.GetDiscard().size(), 2);
}

TEST(CardLookup, throneroomEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Player p2 = Player(2);

    Card *tmpCard = new ActionCard(lookup::councilroom);
    tmpCard->SetEffect(lookup::CouncilroomEffect);
    p1.AddToHand(tmpCard);
    EXPECT_EQ(p1.GetHand().size(), 6);


    state.p1 = &p1;
    state.p2 = &p2;
    lookup::ThroneroomEffect(&state, true);

    EXPECT_EQ(p1.GetHand().size(), 10);
    EXPECT_EQ(p2.GetHand().size(), 7);
}

TEST(CardLookup, councilroomEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Player p2 = Player(2);

    state.p1 = &p1;
    state.p2 = &p2;

    lookup::CouncilroomEffect(&state, true);

    EXPECT_EQ(p2.GetHand().size(), 6);
    EXPECT_EQ(p2.GetDeck().size(), 4);
    EXPECT_EQ(p2.GetDiscard().size(), 0);
}

TEST(CardLookup, libraryEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    p1.AddToDeck(new Card(lookup::adventurer));
    p1.AddToDeck(new Card(lookup::village));
    p1.AddToDeck(new Card(lookup::gold));
    p1.AddToDeck(new Card(lookup::chapel));
    p1.AddToDeck(new Card(lookup::silver));

    state.p1 = &p1;
    lookup::LibraryEffect(&state, true);

    // Assume we set aside the chapel
    EXPECT_EQ(p1.GetHand().size(), 7);
    EXPECT_EQ(p1.GetHand().at(6)->GetName(), "gold");
    EXPECT_EQ(p1.GetHand().at(5)->GetName(), "silver");

    EXPECT_EQ(p1.GetDiscard().size(), 1);
    EXPECT_EQ(p1.GetDiscard().getTopCard()->GetName(), "chapel");

}

// May nondeterministically fail due to RNJesus
TEST(CardLookup, mineEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Pile trash = Pile(TRASH);
    p1.AddToHand(new Card(lookup::copper));
    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    EXPECT_EQ(p1.GetHand().size(), 6);
    state.p1 = &p1;
    state.kingdom = &kingdomCards;
    state.trash = &trash;

    // Trash a copper and take a silver
    lookup::MineEffect(&state, true);
    EXPECT_EQ(p1.GetHand().size(), 6);
    EXPECT_EQ(trash.size(), 1);
    int cardIdx = p1.GetHand().lookThrough(new Card(lookup::silver));
    EXPECT_GT(cardIdx, -1);

    // Trash a silver and take a gold
    lookup::MineEffect(&state, true);
    EXPECT_EQ(p1.GetHand().size(), 6);
    EXPECT_EQ(trash.size(), 2);
    cardIdx = p1.GetHand().lookThrough(new Card(lookup::silver));
    EXPECT_EQ(cardIdx, -1);
    cardIdx = p1.GetHand().lookThrough(new Card(lookup::gold));
    EXPECT_GT(cardIdx, -1);

}

TEST(CardLookup, witchEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    Player p2 = Player(2);

    std::vector<Pile> kingdomCards = game_state::GenerateKingdom(
                                                 lookup::GenAllCards());
    state.p1 = &p1;
    state.p2 = &p2;
    state.kingdom = &kingdomCards;
    lookup::WitchEffect(&state, true);
    EXPECT_EQ(p2.GetDiscard().size(), 1);
    EXPECT_EQ(p2.GetDiscard().getTopCard()->GetName(), "curse");
}

TEST(CardLookup, adventurerEffect) {
    struct stateBlock state;
    Player p1 = Player(1);
    for(int i = 0; i < 5; i++) {
        p1.DiscardCard(0);
    }
    EXPECT_EQ(p1.GetDeck().size(), 5);
    p1.AddToDeck(new Card(lookup::silver));
    p1.AddToDeck(new Card(lookup::estate));
    p1.AddToDeck(new Card(lookup::estate));
    p1.AddToDeck(new Card(lookup::estate));
    p1.AddToDeck(new Card(lookup::gold));

    EXPECT_EQ(p1.GetDeck().size(), 10);
    EXPECT_EQ(p1.GetHand().size(), 0);
    EXPECT_EQ(p1.GetDiscard().size(), 5);

    state.p1 = &p1;

    lookup::AdventurerEffect(&state, true);
    EXPECT_EQ(p1.GetHand().size(), 2);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(p1.GetHand().at(0)->GetName(), "gold");
    EXPECT_EQ(p1.GetHand().at(1)->GetName(), "silver");
}

TEST(Player, drawCard) {
    Player p1 = Player(1, "p1");
    p1.DrawCard();
    EXPECT_EQ(p1.GetHand().size(), 6);
    EXPECT_EQ(p1.GetDeck().size(), 4);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
    for(int i = 0; i < 10; i++) {
        p1.DrawCard();
    }
    EXPECT_EQ(p1.GetHand().size(), 10);
    EXPECT_EQ(p1.GetDeck().size(), 0);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
}

TEST(Player, discardCard) {
    Player p1 = Player(1, "p1");
    p1.DiscardCard(0);
    p1.DiscardCard(0);
    p1.DiscardCard(0);
    EXPECT_EQ(p1.GetHand().size(), 2);
    EXPECT_EQ(p1.GetDeck().size(), 5);
    EXPECT_EQ(p1.GetDiscard().size(), 3);
    for(int i = 0; i < 10; i++) {
        p1.DrawCard();
    }
    EXPECT_EQ(p1.GetHand().size(), 10);
    EXPECT_EQ(p1.GetDeck().size(), 0);
    EXPECT_EQ(p1.GetDiscard().size(), 0);
}


} // namespace


int main(int argc, char **argv) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
