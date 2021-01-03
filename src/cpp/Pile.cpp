/* DOMINION
 * David Mally, Richard Roberts
 * Pile.cpp
 * Defines Pile class, which is used to hold vectors of cards as well as
 * some additional state.
 */
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "Pile.h"
#include "Card.h"
#include "RandUtils.h"
#include "CardLookup.h"
#include "GameState.h"

Pile::Pile(Owner owner, Card cardType, int size, std::string name) {
    m_owner = owner;
    m_cards = std::vector<Card *>(size, new Card(cardType));
    m_name = name;
}

size_t Pile::Size(void) {
    return m_cards.size();
}

Owner Pile::GetOwner(void) {
    return m_owner;
}

std::string Pile::GetName(void) {
    return m_name;
}

std::vector<Card *> Pile::GetCards(void) {
    return m_cards;
}

Card *Pile::At(int idx) {
    return m_cards.at(idx);
}

Card *Pile::DrawAt(int idx) {
    Card *tmpCard = m_cards.at(idx);
    m_cards.erase(m_cards.begin() + idx);
    return tmpCard;
}

void Pile::EmptyDeck(void) {
    m_cards.clear();
}

Card *Pile::GetTopCard(void) {
    return m_cards.at(m_cards.size() - 1);
}

Card *Pile::DrawTopCard(void) {
    if(m_cards.size() > DEF_SIZE) {
        Card *tmpCard = m_cards.at(m_cards.size() - 1);
        m_cards.erase(m_cards.begin() + m_cards.size() - 1);
        return tmpCard;
    } else {
        return NULL;
    }
}

void Pile::TopDeck(Card *card) {
    m_cards.push_back(card);
}

int Pile::LookThrough(Card *card) {
    for(size_t i = 0; i < m_cards.size(); i++) {
        if(m_cards.at(i)->GetName() == card->GetName()) {
            return i;
        }
    }
    return CARD_NOT_FOUND;
}

void Pile::TrueShuffle(void) {
    if(m_cards.size() == DEF_SIZE) { return; }
    std::vector<int> shuffledIdxs = rand_utils::GenPseudoRandList(m_cards.size(), m_cards.size());
    for(size_t i = 0; i < m_cards.size(); i++) {
        Card *tmpCard = m_cards.at(i);
        m_cards.at(i) = m_cards.at(shuffledIdxs.at(i));
        m_cards.at(shuffledIdxs.at(i)) = tmpCard;
    }
}

void Pile::TakeAllFrom(Pile *other) {
    // Note that other shouldn't be a reference here because
    // we're actually modifying it
    std::vector<Card *> otherCards = other->GetCards();
    for(size_t i = 0; i < otherCards.size(); i++) {
        m_cards.push_back(otherCards.at(i));
    }
    other->EmptyDeck();
}

void Pile::Move(Card *card, Pile *other) {
    for(size_t i = 0; i < m_cards.size(); i++) {
        if(m_cards.at(i)->GetName() == card->GetName()) {
            Card *tmp = m_cards.at(i);
            m_cards.erase(m_cards.begin() + i);
            other->TopDeck(tmp);
            return;
        }
    }
}

void Pile::Move(int idx, Pile *other) {
    Card *tmp = m_cards.at(idx);
    m_cards.erase(m_cards.begin() + idx);
    other->TopDeck(tmp);
}

void Pile::PrintPileAsKingdom(void) {
    for(size_t i = 0; i < m_cards.size(); i++) {
        game_state::SetColorByType(m_cards.at(i)->GetType());
        std::cout << "Card " << i  << ": "
                  << m_cards.at(i)->ToString() << std::endl;
    }
    game_state::ResetColor();
}

void Pile::PrintPileAsHand(void) {
    std::cout << "Hand:" << std::endl;
    for(size_t i = 0; i < m_cards.size(); i++) {
        game_state::SetColorByType(m_cards.at(i)->GetType());
        std::cout << "Card " << i  << ": "
                  << m_cards.at(i)->GetName() << std::endl;
    }
    game_state::ResetColor();
}
