/* DOMINION
 * David Mally, Richard Roberts
 * CardLookup.cpp
 * Contains definitions for card effects,
 * utilities for handling i/o errors in card
 * effects, and a function to generate a vector
 * of all action cards.
 */

#include <vector>
#include <iostream>
#include <limits>
#include <unordered_map>
#include "CardLookup.h"

/* Clears cin of error flags and flushes the stdin buffer */
void lookup::ClearCinError(void) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<int>::max(),'\n');
}

/* Checks invalid input when choosing cards by index from a pile or vector
 * of piles. */
void lookup::CheckInvalidChoice(size_t pileSize, int *choicePtr) {
    if(std::cin.fail() || *choicePtr < DEF_CHOICE ||
      *choicePtr > (int)pileSize - 1) {
        lookup::ClearCinError();
        std::cout << "Invalid choice." << std::endl;
        *choicePtr = BAD_CHOICE;
    }
}

bool lookup::CellarEffect(struct stateBlock *state, bool p1) {
    Pile *hand = p1 ? state->p1->HandPtr() : state->p2->HandPtr();
    Pile *discard = p1 ? state->p1->DiscardPtr() : state->p2->DiscardPtr();
    Pile *deck = p1 ? state->p1->DeckPtr() : state->p2->DeckPtr();
    int numDiscarded = 0;
    size_t handSize = hand->Size();
    for(size_t i = 0; i < handSize; i++) {
        hand->PrintPileAsHand();
        int choice = BAD_CHOICE;
        while(choice < DEF_CHOICE) {
            std::cout << "Choose a card to discard (0 - "
                      << hand->GetCards().size() - 1
                      << ") or -1 to discard none: ";
            std::cin >> choice;
            CheckInvalidChoice(hand->GetCards().size(), &choice);
        }
        if(choice >= 0) {
            hand->Move(choice, discard);
            numDiscarded++;
        }
    }
    for(int i = 0; i < numDiscarded; i++) {
        if(deck->Size() > 0) {
            hand->TopDeck(deck->DrawTopCard());
        }
    }
    return false;
}

bool lookup::ChapelEffect(struct stateBlock *state, bool p1) {
    Pile *hand = p1 ? state->p1->HandPtr() : state->p2->HandPtr();
    int choice = BAD_CHOICE;
    for(size_t i = 0; i < LIM_CHAPEL; i++) {
        hand->PrintPileAsHand();
        choice = BAD_CHOICE;
        while(choice == BAD_CHOICE) {
            std::cout << "Choose a card to trash (0 - "
                      << hand->GetCards().size() - 1
                      << ") or -1 to trash none: ";
            std::cin >> choice;
            CheckInvalidChoice(hand->GetCards().size(), &choice);
        }
        if(choice >= 0) {
            hand->Move(choice, state->trash);
        }
        if(choice == DEF_CHOICE) {
            break;
        }
    }
    return false;
}

bool lookup::HarbingerEffect(struct stateBlock *state, bool p1) {
  Pile *discard = p1 ? state->p1->DiscardPtr() : state->p2->DiscardPtr();
  Pile *deck = p1 ? state->p1->DeckPtr() : state->p2->DeckPtr();

  std::string card_name;
  while(card_name != "-1") {
    std::cout << "Choose a card to place on top of your deck:" << std::endl;
    std::cout << "Type -1 if you don't want to select a card." << std::endl;
    discard->PrintPileAsHand();

    std::cin >> card_name;
    int index = 0;
    for(auto card : discard->GetCards()) {
      if(card_name == card->GetName()) {
        discard->Move(index, deck);
        return false;
      }
      index++;
    }
  }
  return false;
}

bool lookup::VassalEffect(struct stateBlock *state, bool p1) {
  Player *currPlayer = p1 ? state->p1 : state->p2;
  Pile *deck = p1 ? state->p1->DeckPtr() : state->p2->DeckPtr();
  auto drawn_card = deck->DrawTopCard();
  if(drawn_card->GetType() == ACTION) {
    std::cout << "The card you drew is: " << drawn_card->GetName() << std::endl;
    std::cout << "Enter 1 if you want to play the card." << std::endl;
    int response;
    std::cin >> response;
    if(response == 1) {
      drawn_card->PlayEffect(state, p1);
    }
  }
  currPlayer->AddToDiscard(drawn_card);
  return false;
}

bool lookup::WorkshopEffect(struct stateBlock *state, bool p1) {
    Pile *discard = p1 ? state->p1->DiscardPtr() : state->p2->DiscardPtr();
    std::string cmd;
    std::cout << "Choose a card with cost <= $4:" << std::endl;
    for(size_t i = 0; i < state->kingdom->size(); i++) {
        if(state->kingdom->at(i).GetTopCard()->GetCost() <= LIM_WORKSHOP) {
            std::cout << state->kingdom->at(i).GetTopCard()->GetName()
                      << std::endl;
        }
    }
    bool validResponse = false;
    while(!validResponse) {
        std::cout << "Name a card: ";
        std::cin >> cmd;
        for(size_t i = 0; i < state->kingdom->size(); i++) {
            if(state->kingdom->at(i).GetTopCard()->GetName() == cmd &&
               state->kingdom->at(i).GetTopCard()->GetCost() <= LIM_WORKSHOP) {
                state->kingdom->at(i).Move(0, discard);
                validResponse = true;
            }
        }
    }
    return false;
}

bool lookup::BureaucratEffect(struct stateBlock *state, bool p1) {
    Pile *deck = p1 ? state->p1->DeckPtr() : state->p2->DeckPtr();
    Pile *otherHand = p1 ? state->p2->HandPtr() : state->p1->HandPtr();
    Pile *otherDeck = p1 ? state->p2->DeckPtr() : state->p1->DeckPtr();

    // Topdeck a silver
    for(size_t i = 0; i < state->kingdom->size(); i++) {
        if(state->kingdom->at(i).GetName() == "silver") {
            state->kingdom->at(i).Move(0, deck);
        }
    }

    // Opponent reveals a victory card and topdecks it
    for(size_t i = 0; i < otherHand->Size(); i++) {
        if(otherHand->At(i)->GetPoints() > 0) {
            otherHand->Move(i, otherDeck);
            return false;
        }
    }
    return false;
}

bool lookup::MilitiaEffect(struct stateBlock *state, bool p1) {
    // Opponent discards down to 3 cards
    Player *otherPlayer = p1 ? state->p2 : state->p1;
    char otherPlayerNum = p1 ? '2' : '1';
    std::cout << "Player " << otherPlayerNum
              << ", discard down to 3 cards."
              << std::endl;
    while(otherPlayer->HandPtr()->Size() > LIM_MILITIA) {
        int idx = DEF_CHOICE;
        std::cout << "Choose card(s) to discard:" << std::endl;
        otherPlayer->HandPtr()->PrintPileAsHand();
        while(idx < 0 || idx > (int)otherPlayer->GetHand().Size() - 1) {
            std::cin >> idx;
            CheckInvalidChoice(otherPlayer->HandPtr()->Size(), &idx);
        }
        otherPlayer->DiscardCard(idx);
    }
    return false;
}
bool lookup::MoneylenderEffect(struct stateBlock *state, bool p1) {
    // Trash a copper and gain + $3
    Player *currPlayer = p1 ? state->p1 : state->p2;
    std::string cmd;
    std::cout << "Trash a copper and gain $3? (y/n)" << std::endl;
    std::cin >> cmd;
    if(cmd == YES) {
        int idx = currPlayer->GetHand().LookThrough(new Card(lookup::copper));
        if(idx > DEF_CHOICE) {
            currPlayer->TrashCard(idx, state->trash);
            currPlayer->AddCoins(COINS_MONEYLENDER);
        }
    }
    return false;
}

bool lookup::PoacherEffect(struct stateBlock *state, bool p1) {
  std::vector<Pile> *kingdom = state->kingdom;

  std::unordered_map<std::string, int> kingdom_count;
  for(auto pile : *kingdom) {
    for(auto card : pile.GetCards()) {
      kingdom_count[card->GetName()]++;
    }
  }

  int num_to_discard = 17 - kingdom_count.size();

  while(num_to_discard > 0) {
    Pile *hand = p1 ? state->p1->HandPtr() : state->p2->HandPtr();
    Pile *discard = p1 ? state->p1->DiscardPtr() : state->p2->DiscardPtr();

    std::cout << "Here is your hand:" << std::endl;
    hand->PrintPileAsHand();
    std::cout << "Choose a card to discard:" << std::endl;

    std::string card_name;
    std::cin >> card_name;
    int index = 0;
    for(auto card : hand->GetCards()) {
      if(card_name == card->GetName()) {
        hand->Move(index, discard);
        num_to_discard--;
        break;
      }
      index++;
    }
  }
  return false;
}

bool lookup::RemodelEffect(struct stateBlock *state, bool p1) {
    // Trash a card and gain a card costing up to $2 more than it
    Player *currPlayer = p1 ? state->p1 : state->p2;
    Pile *trash = state->trash;
    std::vector<Pile> *kingdom = state->kingdom;
    int idx = DEF_CHOICE;
    currPlayer->GetHand().PrintPileAsHand();
    while(idx < 0) {
        std::cin >> idx;
        CheckInvalidChoice(currPlayer->GetHand().Size(), &idx);
    }
    int cardCost = currPlayer->GetHand().At(idx)->GetCost();
    currPlayer->HandPtr()->Move(idx, trash);
    for(size_t i = 0; i < kingdom->size(); i++) {
        std::cout << i << ": " << kingdom->at(i).GetTopCard()->ToString()
                  << std::endl;
    }
    idx = DEF_CHOICE;
    while(idx < 0) {
        std::cout << "Choose a card from the kingdom costing up to $"
                  << cardCost + LIM_REMODEL << ":" << std::endl;
        std::cin >> idx;
        CheckInvalidChoice(kingdom->size(), &idx);
        if(kingdom->at(idx).GetTopCard()->GetCost() >
           cardCost + LIM_REMODEL) {
            idx = DEF_CHOICE;
        }
    }
    currPlayer->AddToDiscard(kingdom->at(idx).DrawTopCard());
    return false;
}

bool lookup::ThroneroomEffect(struct stateBlock *state, bool p1) {
    // Plays any action card from your hand twice
    Player *currPlayer = p1 ? state->p1 : state->p2;
    int idx = DEF_CHOICE;
    currPlayer->GetHand().PrintPileAsHand();
    while(idx < 0) {
        std::cin >> idx;
        CheckInvalidChoice(currPlayer->HandPtr()->Size(), &idx);
        if(currPlayer->HandPtr()->At(idx)->GetType() != ACTION &&
           currPlayer->HandPtr()->At(idx)->GetType() != ATTACK &&
           currPlayer->HandPtr()->At(idx)->GetType() != REACTION) {
            idx = DEF_CHOICE;
            std:: cout << "Selection was not an action card." << std::endl;
        }
    }
    bool trashed = false;
    // Apply all +action, +buy, +coin, +card and effects twice
    for(int i = 0; i < LIM_THRONEROOM; i++) {
        if(currPlayer->HandPtr()->At(idx)->GetEffect() != NULL) {
            trashed = currPlayer->HandPtr()->At(idx)->PlayEffect(state, p1);
        }
        currPlayer->AddActions(currPlayer->HandPtr()->At(idx)->GetActions());
        currPlayer->AddBuys(currPlayer->HandPtr()->At(idx)->GetBuys());
        int plusCards = currPlayer->HandPtr()->At(idx)->GetCards();
        for(int i = 0; i < plusCards; i++) {
            currPlayer->DrawCard();
        }
        currPlayer->AddCoins(currPlayer->HandPtr()->At(idx)->GetCoins());
    }
    if(trashed) {
        currPlayer->TrashCard(idx, state->trash);
    } else {
        currPlayer->DiscardCard(idx);
    }
    return false;
}

bool lookup::BanditEffect(struct stateBlock *state, bool p1) {
  Player *currPlayer = p1 ? state->p1 : state->p2;
  Pile *discard = p1 ? state->p1->DiscardPtr() : state->p2->DiscardPtr();
  std::vector<Pile> *kingdom = state->kingdom;

  for(size_t i = 0; i < kingdom->size(); i++) {
      if(kingdom->at(i).GetTopCard()->GetName() == "gold") {
        kingdom->at(i).Move(0, discard);
      }
  }

  Player *otherPlayer = p1 ? state->p2 : state->p1;
  Pile *trash = state->trash;
  std::string cmd;
  Card *otherCard1 = otherPlayer->DeckPtr()->DrawTopCard();
  Card *otherCard2 = otherPlayer->DeckPtr()->DrawTopCard();
  std::cout << "Opponent's top 2 cards: " << std::endl
            << otherCard1->GetName() << std::endl
            << otherCard2->GetName() << std::endl;

  if(otherCard1->GetType() != TREASURE_C && otherCard2->GetType() != TREASURE_C) {
    std::cout << "Select the treasure to trash: " << std::endl;
    std::string card_name;
    std::cin >> card_name;
    if(otherCard1->GetName() == card_name) {
      trash->TopDeck(otherCard1);
      otherPlayer->AddToDiscard(otherCard2);
      return false;
    }
    if(otherCard2->GetName() == card_name) {
      trash->TopDeck(otherCard2);
      otherPlayer->AddToDiscard(otherCard1);
      return false;
    }
  } else if(otherCard1->GetType() == TREASURE_C && otherCard1->GetName() != "copper") {
    trash->TopDeck(otherCard1);
    otherPlayer->AddToDiscard(otherCard2);
  } else if(otherCard2->GetType() == TREASURE_C && otherCard1->GetName() != "copper") {
    trash->TopDeck(otherCard2);
    otherPlayer->AddToDiscard(otherCard1);
  } else {
    otherPlayer->AddToDiscard(otherCard1);
    otherPlayer->AddToDiscard(otherCard2);
  }
  return false;
}

bool lookup::CouncilroomEffect(struct stateBlock *state, bool p1) {
    // Other player draws a card
    Player *otherPlayer = p1 ? state->p2 : state->p1;
    otherPlayer->DrawCard();
    return false;
}

bool lookup::LibraryEffect(struct stateBlock *state, bool p1) {
    // Draw until you have 7 cards in hand. If you draw actions,
    // you may discard them if you wish.
    Player *currPlayer = p1 ? state->p1 : state->p2;
    while(currPlayer->GetHand().Size() < LIM_LIBRARY) {
        Card *tmpCard = currPlayer->DeckPtr()->DrawTopCard();
        if(tmpCard->GetType() == ACTION ||
           tmpCard->GetType() == ATTACK ||
           tmpCard->GetType() == REACTION) {
            std::string cmd;
            std::cout << tmpCard->ToString() << std::endl;
            std::cout << "Do you wish to set this card aside? (y/n)"
                      << std::endl;
            std::cin >> cmd;
            if(cmd == "y" || cmd == "yes") {
                currPlayer->AddToDiscard(tmpCard);
            } else {
                currPlayer->AddToHand(tmpCard);
            }
        } else {
            currPlayer->AddToHand(tmpCard);
        }
    }
    return false;
}

bool lookup::MineEffect(struct stateBlock *state, bool p1) {
    // Trash a treasure card and gain a treasure card costing
    // up to 3 more than it in-hand.
    Player *currPlayer = p1 ? state->p1 : state->p2;
    std::vector<Pile> *kingdom = state->kingdom;
    int idx = DEF_CHOICE;
    while(idx < 0) {;
        currPlayer->GetHand().PrintPileAsHand();
        std::cin >> idx;
        CheckInvalidChoice(currPlayer->HandPtr()->Size(), &idx);
        if(currPlayer->HandPtr()->At(idx)->GetType() == TREASURE_C) {
            std::cout << "Card choices:" << std::endl;
            int cardCost = currPlayer->HandPtr()->At(idx)->GetCost();
            currPlayer->TrashCard(idx, state->trash);
            // Treasure cards are the last 3 cards in the kingdom vector
            // Silver
            std::cout << "0: " << kingdom->at(IDX_SILVER(kingdom->size()))
                                  .At(0)->ToString()
                      << std::endl;
            // Gold
            if(cardCost + LIM_MINE >= COST_GOLD) {
                std::cout << "1: "
                          << kingdom->at(IDX_GOLD(kingdom->size())).At(0)
                             ->ToString()
                          << std::endl;
            }
            idx = DEF_CHOICE;
            while(idx < 0) {
                std::cin >> idx;
                CheckInvalidChoice(currPlayer->HandPtr()->Size(), &idx);
                if(idx >= 0 && idx < LIM_MINE_IDX){
                    // Add card to hand
                    currPlayer->AddToHand(kingdom->at(
                                kingdom->size() - 1 - (1 -idx)).DrawTopCard());
                } else {
                    idx = DEF_CHOICE;
                }
            }
        } else {
            idx = DEF_CHOICE;
        }
    }
    return false;
}

bool lookup::SentryEffect(struct stateBlock *state, bool p1) {
  Pile *deck = p1 ? state->p1->DeckPtr() : state->p2->DeckPtr();
  Pile *discard = p1 ? state->p1->DiscardPtr() : state->p2->DiscardPtr();
  Pile *trash = p1 ? state->trash : state->trash;

  int size = deck->Size();
  auto card1 = deck->DrawAt(size - 1);
  auto card2 = deck->DrawAt(size - 1);
  bool resolved1 = false;
  bool resolved2 = false;

  std::cout << "Here are the top two cards:" << std::endl;
  std::cout << card1->GetName() << std::endl;
  std::cout << card2->GetName() << std::endl;

  while(!resolved1 || !resolved2) {
    std::cout << "Type a card name to trash; Type -1 if no trash." << std::endl;
    std::string card_name;
    std::cin >> card_name;
    if(card_name == "-1") {
      break;
    }
    if(card_name == card1->GetName() && !resolved1) {
      trash->TopDeck(card1);
      resolved1 = true;
    }
    if(card_name == card2->GetName() && !resolved2) {
      trash->TopDeck(card2);
      resolved2 = true;
    }
  }

  while(!resolved1 || !resolved2) {
    std::cout << "Type a card name to discard; Type -1 if no discard." << std::endl;
    std::string card_name;
    std::cin >> card_name;
    if(card_name == "-1") {
      break;
    }
    if(card_name == card1->GetName() && !resolved1) {
      discard->TopDeck(card1);
      resolved1 = true;
    }
    if(card_name == card2->GetName() && !resolved2) {
      discard->TopDeck(card2);
      resolved2 = true;
    }
  }

  while(!resolved1 && !resolved2) {
    std::cout << "Type a card name to put back on deck" << std::endl;
    std::string card_name;
    std::cin >> card_name;
    if(card_name == card1->GetName() && !resolved1) {
      deck->TopDeck(card1);
      resolved1 = true;
    }
    if(card_name == card2->GetName() && !resolved2) {
      deck->TopDeck(card2);
      resolved2 = true;
    }
  }

  if(!resolved1 && resolved2) {
    deck->TopDeck(card1);
  }

  if(!resolved2 && resolved1) {
    deck->TopDeck(card2);
  }

  return false;
}

bool lookup::WitchEffect(struct stateBlock *state, bool p1) {
    // Opponent gains 1 curse
    Pile *otherDiscard = p1 ? state->p2->DiscardPtr()
                            : state->p1->DiscardPtr();
    for(size_t i = 0; i < state->kingdom->size(); i++) {
        if(state->kingdom->at(i).GetName() == "curse") {
            state->kingdom->at(i).Move(0, otherDiscard);
        }
    }
    return false;
}

bool lookup::ArtisanEffect(struct stateBlock *state, bool p1) {
  // Trash a card and gain a card costing up to $2 more than it
  Player *currPlayer = p1 ? state->p1 : state->p2;
  Pile *discard = state->p1->DiscardPtr();
  std::vector<Pile> *kingdom = state->kingdom;

  int idx = DEF_CHOICE;
  std::cout << "Put a card from your hand onto your deck." << std::endl;
  currPlayer->GetHand().PrintPileAsHand();
  while(idx < 0) {
      std::cin >> idx;
      CheckInvalidChoice(currPlayer->GetHand().Size(), &idx);
  }
  currPlayer->HandPtr()->Move(idx, discard);


  for(size_t i = 0; i < kingdom->size(); i++) {
      std::cout << i << ": " << kingdom->at(i).GetTopCard()->ToString()
                << std::endl;
  }
  while(idx < 0) {
      std::cout << "Choose a card from the kingdom costing up to $"
                << 5 << ":" << std::endl;
      std::cin >> idx;
      CheckInvalidChoice(kingdom->size(), &idx);
      if(kingdom->at(idx).GetTopCard()->GetCost() > 5) {
          idx = DEF_CHOICE;
      }
  }
  currPlayer->AddToDiscard(kingdom->at(idx).DrawTopCard());
  return false;
}

std::vector<Card> lookup::GenAllCards(void) {
    std::vector<Card> allCards;

    allCards.push_back((lookup::cellar));
    allCards.push_back((lookup::chapel));
    allCards.push_back((lookup::moat));

    allCards.push_back((lookup::harbinger));
    allCards.push_back((lookup::merchant));
    allCards.push_back((lookup::vassal));
    allCards.push_back((lookup::village));
    allCards.push_back((lookup::workshop));

    allCards.push_back((lookup::bureaucrat));
    allCards.push_back((lookup::gardens));
    allCards.push_back((lookup::militia));
    allCards.push_back((lookup::moneylender));
    allCards.push_back((lookup::poacher));
    allCards.push_back((lookup::remodel));
    allCards.push_back((lookup::smithy));
    allCards.push_back((lookup::throneroom));

    allCards.push_back((lookup::bandit));
    allCards.push_back((lookup::councilroom));
    allCards.push_back((lookup::festival));
    allCards.push_back((lookup::laboratory));
    allCards.push_back((lookup::library));
    allCards.push_back((lookup::market));
    allCards.push_back((lookup::mine));
    allCards.push_back((lookup::sentry));
    allCards.push_back((lookup::witch));

    allCards.push_back((lookup::artisan));

    return allCards;
}
