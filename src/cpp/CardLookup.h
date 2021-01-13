/* DOMINION
 * David Mally, Richard Roberts
 * CardLookup.h
 * Contains declarations for card effects,
 * utilities for handling i/o errors in card
 * effects, and a function to generate a vector
 * of all action cards. Also defines const
 * Cards that are used in numerous places as
 * the basis for all cards.
 */

#ifndef __CARDLOOKUP_H__
#define __CARDLOOKUP_H__

#include <vector>

#include "Card.h"
#include "ActionCard.h"
#include "TreasureCard.h"
#include "VictoryCard.h"
#include "Pile.h"
#include "InfoStrings.h"

// Input defaults
#define DEF_CHOICE -1
#define BAD_CHOICE -2

// Card limits on effects (e.g. max cost)
#define LIM_CHAPEL     4
#define LIM_WORKSHOP   4
#define LIM_FEAST      5
#define LIM_MILITIA    3
#define LIM_REMODEL    2
#define LIM_THRONEROOM 2
#define LIM_LIBRARY    7
#define LIM_MINE       3
#define LIM_MINE_IDX   2
#define LIM_ADVENTURER 2

// Card indices
#define IDX_SILVER(size) size - 2
#define IDX_GOLD(size)   size - 1

// Coins granted by card effects
#define COINS_MONEYLENDER 3

// Card costs
#define COST_GOLD 6

// Command strings
#define YES "y"

namespace lookup {
    /* EFFECT UTILITIES */
    void ClearCinError(void);
    void CheckInvalidChoice(size_t pileSize, int *choicePtr);

    /* ACTION EFFECTS */
    bool CellarEffect(struct stateBlock *state, bool p1);
    bool ChapelEffect(struct stateBlock *state, bool p1);
    bool HarbingerEffect(struct stateBlock *state, bool p1);
    bool VassalEffect(struct stateBlock *state, bool p1);
    bool WorkshopEffect(struct stateBlock *state, bool p1);
    bool BureaucratEffect(struct stateBlock *state, bool p1);
    bool MilitiaEffect(struct stateBlock *state, bool p1);
    bool MoneylenderEffect(struct stateBlock *state, bool p1);
    bool PoacherEffect(struct stateBlock *state, bool p1);
    bool RemodelEffect(struct stateBlock *state, bool p1);
    bool ThroneroomEffect(struct stateBlock *state, bool p1);
    bool BanditEffect(struct stateBlock *state, bool p1);
    bool CouncilroomEffect(struct stateBlock *state, bool p1);
    bool LibraryEffect(struct stateBlock *state, bool p1);
    bool MineEffect(struct stateBlock *state, bool p1);
    bool SentryEffect(struct stateBlock *state, bool p1);
    bool WitchEffect(struct stateBlock *state, bool p1);
    bool ArtisanEffect(struct stateBlock *state, bool p1);

    /* VICTORY CARDS */
    const VictoryCard estate   = VictoryCard(2, 1, "estate", VICTORY);
    const VictoryCard duchy    = VictoryCard(5, 3, "duchy", VICTORY);
    const VictoryCard province = VictoryCard(8, 6, "province", VICTORY);
    const VictoryCard curse    = VictoryCard(0, -1, "curse", VICTORY);

    /* TREASURE CARDS */
    const TreasureCard copper  = TreasureCard(0, 1, "copper", TREASURE_C);
    const TreasureCard silver  = TreasureCard(3, 2, "silver", TREASURE_C);
    const TreasureCard gold    = TreasureCard(6, 3, "gold", TREASURE_C);

    /* KINGDOM CARDS */

    /* COST 2 */
    const ActionCard cellar    = ActionCard(2,1,0,0,0,"cellar",
                                 CellarEffect, ACTION, CELLAR_INFO);
    const ActionCard chapel    = ActionCard(2,0,0,0,0,"chapel",
                                 ChapelEffect, ACTION, CHAPEL_INFO);
    const ActionCard moat      = ActionCard(2,0,0,2,0,"moat",
                                  NULL, REACTION, MOAT_INFO);

    /* COST 3 */
    const ActionCard harbinger = ActionCard(3,2,0,1,0,"harbinger",
                                  HarbingerEffect, ACTION, HARBINGER_INFO);
    const ActionCard merchant = ActionCard(3,1,0,1,0,"merchant",
                                  NULL, ACTION);
    const ActionCard vassal = ActionCard(3,0,0,0,2,"vassal",
                                  VassalEffect, ACTION, VASSAL_INFO);
    const ActionCard village = ActionCard(3,2,0,1,0,"village",
                                  NULL, ACTION);
    const ActionCard workshop = ActionCard(3,0,0,0,0,"workshop",
                                  WorkshopEffect, ACTION, WORKSHOP_INFO);

    /* COST 4 */
    const ActionCard bureaucrat = ActionCard(4,0,0,0,0,"bureaucrat",
                                  BureaucratEffect, ATTACK, BUREAUCRAT_INFO);
    const VictoryCard gardens = VictoryCard(4,0,"gardens", VICTORY,
                                            GARDENS_INFO);
    const ActionCard militia = ActionCard(4,0,0,0,2,"militia",
                                  MilitiaEffect, ATTACK, MILITIA_INFO);
    const ActionCard moneylender = ActionCard(4,0,0,0,0,"moneylender",
                                  MoneylenderEffect, ACTION, MONEYLENDER_INFO);
    const ActionCard poacher = ActionCard(4,1,0,1,1,"poacher",
                                  PoacherEffect, ACTION, POACHER_INFO);
    const ActionCard remodel = ActionCard(4,0,0,0,0,"remodel",
                                  RemodelEffect, ACTION, REMODEL_INFO);
    const ActionCard smithy = ActionCard(4,0,0,3,0,"smithy",
                                  NULL, ACTION);
    const ActionCard throneroom = ActionCard(4,0,0,0,0,"throneroom",
                                  ThroneroomEffect, ACTION, THRONEROOM_INFO);

    /* COST 5 */
    const ActionCard bandit = ActionCard(5,0,0,0,0,"bandit",
                                  BanditEffect, ACTION, BANDIT_INFO);
    const ActionCard councilroom = ActionCard(5,0,1,4,0,"councilroom",
                                  CouncilroomEffect, ACTION, COUNCILROOM_INFO);
    const ActionCard festival = ActionCard(5,2,1,0,2,"festival",
                                    NULL, ACTION);
    const ActionCard laboratory = ActionCard(5,1,0,2,0,"laboratory",
                                    NULL, ACTION);
    const ActionCard library = ActionCard(5,0,0,0,0,"library",
                                  LibraryEffect, ACTION, LIBRARY_INFO);
    const ActionCard market = ActionCard(5,1,1,1,1,"market", NULL, ACTION);
    const ActionCard mine = ActionCard(5,0,0,0,0,"mine",
                                  MineEffect, ACTION, MINE_INFO);
// ActionCard::ActionCard(int cost, int actions, int buys, int cards,
//                                                          int coins, std::string name,
    const ActionCard sentry = ActionCard(5,1,0,1,0,"sentry",
                                  SentryEffect, ACTION, SENTRY_INFO);
    const ActionCard witch = ActionCard(5,0,0,2,0,"witch",
                                  WitchEffect, ATTACK, WITCH_INFO);

    /* COST 6 */
    const ActionCard artisan = ActionCard(6,0,0,0,0,"artisan",
                                  ArtisanEffect, ACTION, ARTISAN_INFO);

    /* DUMMY */
    const Card dummy = Card(DEF_COST, DEF_NAME);

    /* VECTOR OF ALL CARDS */
    std::vector<Card> GenAllCards(void);

}

#endif
