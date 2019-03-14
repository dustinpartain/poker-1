#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "future.h"
#include "deck.h"
#include "cards.h"

// Changed indents of args
void add_new_decks(size_t new_deck_sz, 
                  future_cards_t * fc, 
                  size_t index, 
                  card_t * ptr, 
                  size_t start_index) {
  fc->decks = realloc(fc->decks, new_deck_sz * sizeof(*fc->decks));
  for (size_t i = start_index; i < (index + 1); i++) {
    fc->decks[i].cards = malloc(sizeof(*fc->decks[i].cards));
    if (i == index) {
      fc->decks[i].cards[0] = ptr;
      fc->decks[i].n_cards = 1;
    }
    else {
      fc->decks[i].n_cards = 0;
    }
    fc->n_decks++;
  }
}

void add_future_card(future_cards_t * fc, size_t index, card_t * ptr) {
  // Future card deck fc already has at least 1 deck
  if (fc->n_decks > 0) {
    // Index of future card requires adding of 1 or more decks
    if (index >= fc->n_decks) {
      size_t new_deck_sz = fc->n_decks + ((index + 1) - fc->n_decks);
      size_t start_index = fc->n_decks;
      add_new_decks(new_deck_sz, fc, index, ptr, start_index);
    }
    // Index of future card points to a deck already in fc
    else {
      size_t new_card_sz = fc->decks[index].n_cards + 1;
      size_t new_card_ind = fc->decks[index].n_cards;
      fc->decks[index].cards = realloc(fc->decks[index].cards, new_card_sz * sizeof(*fc->decks[index].cards));
      fc->decks[index].cards[new_card_ind] = ptr;
      fc->decks[index].n_cards++;
    }
  }
  // Future card deck fc does not have any decks yet
  else {
    size_t new_deck_sz = index + 1;
    if (fc->n_decks != 0) {
      fc->n_decks = 0;
      size_t start_index = 0;
      add_new_decks(new_deck_sz, fc, index, ptr, start_index);      
    }
    else {
      size_t start_index = 0;
      add_new_decks(new_deck_sz, fc, index, ptr, start_index);
    }
  }
}

// Takes a shuffled deck and draws cards from it, assigning the values
// and suits to the placeholders pointed to by the future cards in fc
void future_cards_from_deck(deck_t * deck, future_cards_t * fc) {
  for (size_t i = 0; i < fc->n_decks; i++) {
    if (i > (*deck).n_cards) {
      fprintf(stderr, "Error. There are more future cards than there are cards in the deck.");
      exit(EXIT_FAILURE);
    }
    else {
      for (size_t j = 0; j < fc->decks[i].n_cards; j++) {
	      card_t temp_card = *(deck->cards)[i];
	      *(fc->decks)[i].cards[j] = temp_card;
      }
    }
  }
}
