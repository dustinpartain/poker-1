#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "future.h"
#include "deck.h"
#include "cards.h"

char * string_slice(const char * str, size_t start, size_t end) {
  size_t start_j = (start + 1);
  size_t end_j = start_j + end;
  char * new_str = malloc((end_j - start_j + 1) * sizeof(*new_str));
  size_t k = 0;
  
  for (size_t j = start_j; j < end_j; j++) {
    new_str[k] = str[j];
    k++;
  }
  new_str[k] = '\0';
  return new_str;
}

size_t is_valid_index(const char * temp_index, 
                    const char * str, 
                    size_t current_index, 
                    size_t string_len, 
                    size_t * next_i) {
  size_t valid_index = -1;
  size_t i = current_index + 1;
  size_t counter = 0;

  if (str[i] < 48 || str[i] > 57) {
    return valid_index;
  }

  while ((str[i] != 32) && (str[i] != '\0') && i < (string_len-1)) {
    if (str[i] >= 48 && str[i] <= 57) {
      i++;
      counter++;
    }
  }

  char * new_str = string_slice(str, current_index, counter);
  const char * const_new_str = new_str;
  valid_index = atoi(const_new_str);
  *next_i = counter;
  free(new_str);
  return valid_index;
}

int is_valid_input(char value, char suit) {
  int is_valid = 0;

  if (value == 48 || (value >= 50 && value <= 57) || value == 63 || value == 65 || (value >= 74 && value <= 75) || value == 81) {
    is_valid = 0;
  }
  else {
    is_valid = 1;
    return is_valid;
  }

  if (suit == 's' || suit == 'c' || suit == 'd' || suit == 'h') {
    is_valid = 0;
  }
  else if (value == '?' && (suit >= 48 && suit <= 57)) {
    is_valid = 0;
  }
  else {
    is_valid = 1;
    return is_valid;
  }      
  return is_valid;
}

deck_t * hand_from_string(const char * str, future_cards_t * fc) {
  deck_t * ans = malloc(sizeof(*ans));
  ans->cards = malloc(sizeof(*ans->cards));
  ans->n_cards = 0;
  size_t string_len = strlen(str);

  for (size_t i = 0; i < string_len; i++) {
    char value = str[i];
    char suit = str[i+1];

    if (is_valid_input(value, suit) == 0) {
      if (value == '?') {
        const char * temp_index = &suit;
        size_t next_i = 0;
        size_t * next_i_ptr = &next_i;
        size_t index = is_valid_index(temp_index, str, i, string_len, next_i_ptr);
        ssize_t signed_index = index;
	    if (signed_index >= 0) {
            card_t * test_ptr = add_empty_card(ans);
            card_t * ptr = ans->cards[ans->n_cards-1];
            add_future_card(fc, index, ptr);
            i = i + (*next_i_ptr);
            free(test_ptr);
	    }
	    else {
            fprintf(stderr, "Something went wrong.");
            exit(EXIT_FAILURE);
	    }
      }
      else {
        card_t temp = card_from_letters(value, suit);
        assert_card_valid(temp);
        add_card_to(ans, temp);
        i = i + 2;
      }
    }
    else if (is_valid_input(value, suit) == 1 && value != 32) {
      if(str[i] == '\0' || str[i] == '\n'){
	    continue;
      }
      else if (str[i] != '\0' || str[i] != '\n') {
        printf("%c is not a valid input\n", str[i]);
        exit(EXIT_FAILURE);
      }
    }
  }

  if (ans->n_cards < 5) {
    fprintf(stderr, "Hands must contain at least 5 cards.");
    exit(EXIT_FAILURE);
  }
  return ans;
}

deck_t ** read_input(FILE * f, size_t * n_hands, future_cards_t * fc) {
  deck_t ** hands = malloc(sizeof(*hands));
  size_t hands_num = 0;
  deck_t * temp;
  size_t sz = 0;
  ssize_t len = 0;
  char * input_line = NULL;

  while ((len = getline(&input_line, &sz, f)) >= 0) {
    if (len <= 1) {
      fprintf(stderr, "No valid card.\n");
      exit(EXIT_FAILURE);
    }
    else {
      temp = hand_from_string(input_line, fc);
      hands = realloc(hands, (hands_num + 1) * sizeof(*hands));
      hands[hands_num] = temp;
      hands_num++;
    }
  }

  *n_hands = hands_num;
  free(input_line);
  return hands;
}
