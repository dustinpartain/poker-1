#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "cards.h"
#include "deck.h"
#include "eval.h"
#include "future.h"
#include "input.h"

// Testing functions, to be removed after testing completed
void read_input_testing(deck_t ** hands, size_t * n_hands, future_cards_t * fc) {
  printf("TESTING READ_INPUT\n");
  printf("----------------------------------\n");

  for (size_t k = 0; k < *n_hands; k++) {
    printf("Hand %zu: ", (k+1));
    print_hand(hands[k]);
    printf("\n");
  }

  printf("FC has %zu deck(s)\n", fc->n_decks);
  for (size_t m = 0; m < fc->n_decks; m++) {
    printf("FC deck %zu: ", m+1);
    for (size_t n = 0; n < fc->decks[m].n_cards; n++) {
      print_card(*fc->decks[m].cards[n]);
      printf(" ");
    }
    printf("\n");
  }
  printf("----------------------------------\n\n");
}

void deck_testing(deck_t * d) {
  printf("TESTING DECK CREATION\n");
  printf("----------------------------------\n");
  printf("Test Deck: \n");
  print_hand(d);
  printf("\n\n");
  printf("----------------------------------\n\n");
}

void deck_exclude(deck_t * d, deck_t ** hands, size_t * n_hands) {
  printf("TESTING DECK EXCLUSION OF ALREADY KNOWN CARDS\n");
  printf("----------------------------------\n\n");
  for (size_t i = 0; i < *n_hands; i++) {
    for (size_t j = 0; j < hands[i]->n_cards; j++) {
      card_t c = *hands[i]->cards[j];
      int does_contain = deck_contains(d, c);
      if (does_contain == 0) {
        printf("Deck does not contain card ");
        print_card(c);
        printf("\n");
      }
      else {
        printf("Deck does contain card ");
        print_card(c);
        printf("\n");
      }
    }
  }
  printf("----------------------------------\n\n");
}

void testing_wins_array(unsigned int * w, size_t * n_hands) {
  printf("TESTING WINS ARRAY INITIALIZATION\n");
  printf("----------------------------------\n\n");
  printf("There are %zu hands\n", *n_hands);
  for (size_t i = 0; i < (*n_hands + 1); i++) {
    printf("Wins index %zu is %u\n", i, w[i]);
  }
  printf("----------------------------------\n\n");
}

void check_shuffle(deck_t * d) {
  printf("After shuffle: ");
  print_hand(d);
  printf("\n");
}

void check_hands(deck_t ** hands, size_t * n_hands) {
  for (size_t i = 0; i < *n_hands; i++) {
    printf("Hand %zu: ", i);
    print_hand(hands[i]);
    printf("\n");
  }
}
// End of testing functions

void free_future_cards(future_cards_t * fc) {
  if (fc->n_decks > 0) {
    for (size_t i = 0; i <fc->n_decks; i++) {
      free(fc->decks[i].cards);
    }
  }
  free(fc->decks);
  free(fc);
}

void free_input_hands(deck_t ** hands, size_t * n_hands) {
  for (size_t i = 0; i < *n_hands; i++) {
    free_deck(hands[i]);
  }
  free(hands);
}

void initialize_wins(unsigned int * w, size_t * n_hands) {
  for (size_t i = 0; i < (*n_hands + 1); i++) {
    w[i] = 0;
  }
}

void print_results(unsigned int * w, size_t * n_hands, unsigned int num_trials) {
  for (size_t i = 0; i < *n_hands; i++) {
    unsigned int win_num = w[i];
    unsigned int total_trials = num_trials;
    float win_num_float = win_num;
    float total_trials_float = total_trials;
    double percent_won = 100 * (win_num_float / total_trials_float);
    printf("Hand %zu won %u / %u times (%.2f%%)\n", i, win_num, total_trials, percent_won);
  }
  unsigned int num_ties = w[*n_hands];
  printf("And there were %u ties\n", num_ties);
}

int main(int argc, char ** argv) {

  unsigned int num_trials = 0;
  if (argc == 1) {
    fprintf(stderr, "Too few arguments provided. Usage: program name, input file, number of Monte Carlo trials (optional).\n");
    return EXIT_FAILURE;
  }
  else if (argc == 2) {
    num_trials = 10000;
  }
  else if (argc == 3) {
    num_trials = atoi(argv[2]);
  }
  else if (argc > 4) {
    fprintf(stderr, "Too many arguments provided. Usage: program name, input file, number of Monte Carlo trials (optional).\n");
    return EXIT_FAILURE;
  }

  FILE * f = fopen(argv[1], "r");
  if (f == NULL) {
    perror("Could not open file.");
    return EXIT_FAILURE;
  }
  
  size_t hands_sz = 0;
  size_t * n_hands = &hands_sz;
  future_cards_t * fc = malloc(sizeof(*fc));
  fc->decks = malloc(sizeof(*fc->decks));
  fc->n_decks = 0;
  deck_t ** hands = read_input(f, n_hands, fc);

  if (fclose(f) != 0) {
    perror("Failed to close the input file.");
    return EXIT_FAILURE;
  }

  deck_t * deck = build_remaining_deck(hands, *n_hands);
  unsigned int * wins = malloc((*n_hands + 2) * sizeof(*wins));
  initialize_wins(wins, n_hands);
  unsigned int num_trials_tracker = num_trials;

  while (num_trials_tracker > 0) {
    if (*n_hands < 2) {
      fprintf(stderr, "At least two hands are required to run the Monte Carlo trial.\n");
      return EXIT_FAILURE;
    }
    shuffle(deck);
    future_cards_from_deck(deck, fc);

    // If first hand wins, result of compare_hands will be 1
    // If second hand wins, result will be -1
    // If hands tie, result will be 0
    int result = -10;
    if (*n_hands == 2) {
      result = compare_hands(hands[0], hands[1]);
      if (result == -1) {
	    wins[1] = wins[1] + 1;
      }
      else if (result == 1) {
	    wins[0] = wins[0] + 1;
      }
      else if (result == 0) {
	    wins[2] = wins[2] + 1;
      }
      else {
	    printf("Something went wrong, no hand won, no tie.\n");
      }
    }
    else {
      size_t k = *n_hands;
      size_t m = 1;
      size_t winner = 0;
      size_t tie = 0;
      while (k > 0 && m < *n_hands) {
        result = compare_hands(hands[winner], hands[m]);
        if (result == -1) {
	        tie = 0;
            winner = m;
	    }
	    else if (result == 1) {
            size_t hand0_idx = winner;
            winner = hand0_idx;
	    }
	    else if (result == 0) {
            size_t hand0_idx = winner;
            winner = hand0_idx;
  	        tie = 1;
	    }
	    else {
	        printf("Something went wrong, no hand won, no tie.\n");
	    }
	    m++;
	    k--;
      }
      if (tie == 0) {
      	wins[winner] = wins[winner] + 1;
      }
      else {
      	wins[*n_hands] = wins[*n_hands] + 1;
      }
    }
    num_trials_tracker--;
  }
  print_results(wins, n_hands, num_trials);
  free(wins);
  free_deck(deck);
  free_input_hands(hands, n_hands);
  free_future_cards(fc);

  return EXIT_SUCCESS;
}
