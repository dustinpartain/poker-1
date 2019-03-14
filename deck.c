#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "deck.h"
#include "cards.h"

void free_deck(deck_t * deck) {
  size_t num_cards = deck->n_cards;
  for (size_t i = 0; i < num_cards; i++) {
    free(deck->cards[i]);
  }
  free(deck->cards);
  free(deck);
}

void print_hand(deck_t * hand) {
  size_t hand_size = (*hand).n_cards;
  deck_t my_hand = *hand;
  for (size_t i = 0; i < hand_size; i++) { 
    card_t current_card = *(my_hand.cards)[i];
    print_card(current_card);
    printf(" ");
  } 
}

int deck_contains(deck_t * d, card_t c) {
  deck_t my_deck = *d;
  size_t deck_size = (*d).n_cards;
  for (size_t i = 0; i < deck_size; i++) {
    card_t current_card = *(my_deck.cards)[i];
    if (current_card.value == c.value && current_card.suit == c.suit) {
      return 1;
    }
  }
  return 0;
}

void shuffle(deck_t * d) {
  size_t deck_size = (*d).n_cards;
  if (deck_size > 1) {
    int j = 0;
    while (j < 2) {
      for (size_t i = 0; i < deck_size; i++) {
        int new_index = 1 + (int)((deck_size - 1) * random() / (RAND_MAX + 0.0));
        card_t * temp = d->cards[i];
        d->cards[i] = d->cards[new_index];
        d->cards[new_index] = temp;
      }
      j++;
    }
  }
}

void assert_full_deck(deck_t * d) {
  size_t deck_size = (*d).n_cards;
  if (deck_size != 52) {
    assert(deck_size == 52);
  }
  else {
    for (unsigned value = 2; value < 15; value++) {
      for (int suit = 0; suit < 4; suit++) {
        card_t check;
        check.value = value;
        check.suit = suit;
        assert(deck_contains(d, check));
      }
    }
  }
}

void add_card_to(deck_t * deck, card_t c) {
  size_t deck_size = (*deck).n_cards;
  card_t * temp_card = &c;
  deck->cards = realloc(deck->cards, (deck_size + 1) * sizeof(*deck->cards));
  deck->cards[deck_size] = malloc(sizeof(*temp_card));
  *(deck->cards)[deck_size] = c;
  deck->n_cards++;
}

card_t * add_empty_card(deck_t * deck) {
  card_t * empty = malloc(sizeof(*empty));
  card_t empty_card;
  empty_card.value = 0;
  empty_card.suit = NUM_SUITS;
  *empty = empty_card;
  add_card_to(deck, empty_card);
  return empty;
}

deck_t * make_deck_exclude(deck_t * excluded_cards) {
  size_t excluded_sz = excluded_cards->n_cards;
  deck_t * new_deck = malloc(sizeof(*new_deck));
  new_deck->n_cards = 0;
  new_deck->cards = malloc(sizeof(*new_deck->cards));
  //Loops through all possible card values, 2-14
  for(int value = 2; value < 15; value++) {
    //Loops through all possible suit values, 0-3
    for(int suit = 0; suit < 4; suit++) {
      //Creates a card from value, suit
      card_t temp_card;
      temp_card.value = value;
      temp_card.suit = suit;
      if (excluded_sz == 0) {
        add_card_to(new_deck, temp_card);
      }
      else {
	    if (!deck_contains(excluded_cards, temp_card)) {
	    add_card_to(new_deck, temp_card);
	    }
      }
    }
  }
  return new_deck;
}

deck_t * build_remaining_deck(deck_t ** hands, size_t n_hands) {
  deck_t * exclude_all;
  deck_t * remain_deck;
  exclude_all = malloc(sizeof(*remain_deck));
  exclude_all->cards = malloc(sizeof(*exclude_all->cards));
  exclude_all->n_cards = 0;
  for (size_t i = 0; i < n_hands; i++) {
    deck_t * d = hands[i];
    size_t d_sz = hands[i]->n_cards;
    for (size_t j = 0; j < d_sz; j++) {
      card_t temp = *(d->cards)[j];
      add_card_to(exclude_all, temp);
    }
  }
  remain_deck = make_deck_exclude(exclude_all);
  free_deck(exclude_all);
  return remain_deck;
}
