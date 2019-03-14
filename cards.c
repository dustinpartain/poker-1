#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "cards.h"

void assert_card_valid(card_t c) {
  assert(c.value >= 2 && c.value <= VALUE_ACE);
  assert(c.suit >= SPADES && c.suit <= CLUBS);
}

const char * ranking_to_string(hand_ranking_t r) {
  switch(r){
  case STRAIGHT_FLUSH: return "STRAIGHT_FLUSH"; break;
  case FOUR_OF_A_KIND: return "FOUR_OF_A_KIND"; break;
  case FULL_HOUSE: return "FULL_HOUSE"; break;
  case FLUSH: return "FLUSH"; break;
  case STRAIGHT: return "STRAIGHT"; break;
  case THREE_OF_A_KIND: return "THREE_OF_A_KIND"; break;
  case TWO_PAIR: return "TWO_PAIR"; break;
  case PAIR: return "PAIR"; break;
  case NOTHING: return "NOTHING"; break;
  default: return ""; break;
  }
  return "";
}

char value_letter(card_t c) {
  char x = '0';
  if (c.value >= 2 && c.value <= 9) {
    x = c.value + '0';
  }
  else if (c.value >= 10 && c.value <= 14) {
    switch(c.value){
    case 10: x = '0'; break;
    case 11: x = 'J'; break;
    case 12: x = 'Q'; break;
    case 13: x = 'K'; break;
    case 14: x = 'A'; break;
    default: return EXIT_FAILURE; break;
    }
  }
  return x;
}

char suit_letter(card_t c) {
  char x;
  switch(c.suit){
  case 0: x = 's'; break;
  case 1: x = 'h'; break;
  case 2: x = 'd'; break;
  case 3: x = 'c'; break;
  case 4: x = '0'; break;
  default: return EXIT_FAILURE; break;
  }
  return x;
}

void print_card(card_t c) {
  printf("%c%c", value_letter(c), suit_letter(c));
}

int value_from_letter(char v) {
  int value_int;
  if ((v - '0') >= 2 && (v - '0') <= 9) {
    value_int = v - '0';
  }
  else if ((v - '0') == 0) {
    value_int = 10;
  }
  else if (v == 'K') {
    value_int = VALUE_KING;
  }
  else if (v == 'J') {
    value_int = VALUE_JACK;
  }
  else if (v == 'Q') {
    value_int = VALUE_QUEEN;
  }
  else if (v == 'A') {
    value_int = VALUE_ACE;
  }
  else {
    value_int = 1;
  }
  return value_int;
}

suit_t suit_from_letter(char s) {
  suit_t suit;
  if (s == 's') {
    suit = 0;
  }
  else if (s == 'h') {
    suit = 1;
  }
  else if (s == 'd') {
    suit = 2;
  }
  else if (s == 'c') {
    suit = 3;
  }
  else if (s == NUM_SUITS) {
    suit = 4;
  }
  else {
    suit = 5;
  }
  return suit;
}

card_t card_from_letters(char value_let, char suit_let) {
  card_t temp;
  temp.value = value_from_letter(value_let);
  temp.suit = suit_from_letter(suit_let);
  if ((temp.value < 2 || temp.value > 14) || temp.suit > 3) {
    assert_card_valid(temp);
  }
  return temp;
}

int value_from_num(unsigned n) {
  card_t test_card;
  int value = n;
  test_card.value = n;
  if (n >= 52) {
    assert_card_valid(test_card);
  }
  
  if ((n % 13) <= 12) {
    value = (n % 13) + 2;
  }
  else {
    assert_card_valid(test_card);
  }
  return value;
}

suit_t suit_from_num(unsigned n) {
  suit_t suit = 4;
  card_t test_card;
  test_card.value = 2;
  test_card.suit = n;

  if (n < 13) {
    suit = 0;
  }
  else if (n >= 13 && n < 26) {
    suit = 1;
  }
  else if (n >= 26 && n < 39) {
    suit = 2;
  }
  else if (n >= 39 && n < 52) {
    suit = 3;
  }
  else {
    assert_card_valid(test_card);
  }
  return suit;
}

card_t card_from_num(unsigned c) {
  card_t temp;
  temp.value = value_from_num(c);
  temp.suit = suit_from_num(c);
  return temp;
}
