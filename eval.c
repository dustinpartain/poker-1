#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int card_ptr_comp(const void * vp1, const void * vp2) {
  const card_t * const * cp1 = vp1;
  const card_t * const * cp2 = vp2;
  if ((**cp1).value > (**cp2).value) {
    return -1;
  }
  else if ((**cp1).value < (**cp2).value) {
    return 1;
  }
  else {
    if ((**cp1).suit > (**cp2).suit) {
      return -1;
    }
    else if ((**cp1).suit < (**cp2).suit) {
      return 1;
    }
    else {
      printf("Cannot have 2 cards with same value and same suit in the same hand.\n");
      exit(EXIT_FAILURE);
    }
  }
}

suit_t flush_suit(deck_t * hand) {
  deck_t my_hand = *hand;
  size_t hand_size = (*hand).n_cards;
  if (hand_size < 5) {
    return NUM_SUITS;
  }
  else {
    int clubs = 0;
    int hearts = 0;
    int diamonds = 0;
    int spades = 0;
    for (size_t i = 0; i < hand_size; i++) {
      card_t current_card = *(my_hand.cards)[i];
      if (current_card.suit == 0) {
	    spades++;
      }
      else if (current_card.suit == 1) {
    	hearts++;
      }
      else if (current_card.suit == 2) {
    	diamonds++;
      }
      else if (current_card.suit == 3) {
    	clubs++;
      }
    }

    if (clubs >= 5) {
      return CLUBS;
    }
    else if (diamonds >= 5) {
       return DIAMONDS;
    }
    else if (hearts >= 5) {
      return HEARTS;
    }
    else if (spades >= 5) {
       return SPADES;
    }
  }
  return NUM_SUITS;
}

unsigned get_largest_element(unsigned * arr, size_t n) {
  unsigned largest = 0;
  for (size_t i = 0; i < n; i++) {
    if (arr[i] > largest) {
      largest = arr[i];
    }
  }
  return largest;
}

size_t get_match_index(unsigned * match_counts, size_t n, unsigned n_of_akind) {
  size_t match_index = 0;
  for (size_t i = 0; i < n; i++) {
    if (match_counts[i] == n_of_akind) {
      match_index = i;
      break;
    }
  }
  return match_index;
}

size_t secondary_pair_idx(card_t current_card, 
                        card_t match_idx_card, 
                        size_t match_idx, 
                        size_t pair_idx, 
                        size_t current_idx) {
  if (current_card.value != match_idx_card.value) {
    if (pair_idx == -1 || current_idx < pair_idx) {
      pair_idx = current_idx;
    }
  }
  return pair_idx;
}

ssize_t find_secondary_pair(deck_t * hand,
			                unsigned * match_counts,
			                size_t match_idx) {
  deck_t my_hand = *hand;
  size_t hand_count = (*hand).n_cards;
  card_t match_idx_card = *(my_hand.cards)[match_idx];
  ssize_t pair_idx = -1;
  for (size_t i = 0; i < hand_count; i++) {
    if (match_counts[i] > 1) {
      card_t current_card = *(my_hand.cards)[i];
      pair_idx = secondary_pair_idx(current_card, match_idx_card, match_idx, pair_idx, i);
    }
  }
  return pair_idx;
}

int check_flush_straight(deck_t * hand, size_t index, suit_t fs, size_t hand_count) {
  deck_t my_hand = *hand;
  int flush_straight = 0;
  for (size_t i = index; i < hand_count; i++) {
    card_t current_card = *(my_hand.cards)[i];
    if (current_card.suit != fs) {
      flush_straight = 0;
      return flush_straight;
    }
    else if(current_card.suit == fs && i == (hand_count - 1)) {
      flush_straight++;
      return flush_straight;
    }
    else if(current_card.suit == fs && i != (hand_count - 1)) {
        card_t next_card = *(my_hand.cards)[i+1];
        if (next_card.value == (current_card.value - 1) && next_card.suit == fs) {
            flush_straight++;
        }
        else if (next_card.value == (current_card.value - 1) && next_card.suit != fs && (i + 2) != hand_count) {
            card_t third_card1 = *(my_hand.cards)[i+2];
            if (third_card1.value == (current_card.value - 1) && third_card1.suit == fs) {
                flush_straight++;
                i++;
            }
            else {
                flush_straight++;
                return flush_straight;
            }
        }
        else if (next_card.value == current_card.value && (i + 2) != hand_count) {
            card_t third_card2 = *(my_hand.cards)[i+2];
            if (third_card2.value == (current_card.value - 1) && third_card2.suit == fs) {
                flush_straight++;
                i++;
            }
            else {
                flush_straight++;
                return flush_straight;
            }
        }
        else {
            flush_straight++;
            return flush_straight;
        }
    }
    else {
        return flush_straight;
    }
  }
  return flush_straight;
}

int check_straight(deck_t * hand, size_t index, size_t hand_count) {
  deck_t my_hand = *hand;
  int straight = 0;
  for (size_t i = index; i < hand_count; i++) {
    card_t current_card = *(my_hand.cards)[i];
    if (i == (hand_count - 1)) {
        straight++;
        return straight;
    }
    else {
        card_t next_card = *(my_hand.cards)[i+1];
        if (next_card.value == (current_card.value - 1)) {
            straight++;
        }
        else if (next_card.value == current_card.value && (i + 2) != hand_count) {
            card_t third_card = *(my_hand.cards)[i+2];
            if (third_card.value == (current_card.value - 1)) {
                straight++;
                i++;
            }
            else {
                straight++;
                return straight;
            }
        }
        else {
            straight++;
            return straight;
        }
    }
  }
  return straight;
}

int is_ace_low_straight(deck_t * hand, size_t hand_count, suit_t fs) {
  suit_t fl_st = fs;
  deck_t my_hand = *hand;
  int is_ace_low = 1;
  int is_straight = 0;
  card_t last_card = *(my_hand.cards)[hand_count-1];
  if (last_card.value == 2 && fl_st != 4 && last_card.suit == fl_st) {
    int signed_hand_count = hand_count;
    if ((signed_hand_count - 4) >= 0) {
      card_t check_five = *(my_hand.cards)[hand_count-4];
      if (check_five.value == 5 && check_five.suit == fl_st) {
	    // Must also check if Ace suit is correct
        for (size_t j = 0; j <= (hand_count - 4); j++) {
          card_t check_ace = *(my_hand.cards)[j];
          if (check_ace.value == 14 && check_ace.suit == fl_st) {
            size_t start_idx = hand_count - 4;
            is_straight = check_flush_straight(hand, start_idx, fl_st, hand_count);
          }
        }
      }
    }
  }
  else if (last_card.value == 2 && fl_st == 4) {
    int signed_hand_count = hand_count;
    if ((signed_hand_count - 4) >= 0) {
      card_t check_five = *(my_hand.cards)[hand_count-4];
      if (check_five.value == 5) {
        size_t start_idx = hand_count - 4;
        is_straight = check_straight(hand, start_idx, hand_count);
      }
    }
  }

  if (is_straight >= 4) {
    is_ace_low = 0;
    return is_ace_low;
  }
  else {
    return is_ace_low;
  }
}

int is_straight_at_hlpr(deck_t * hand, size_t index, suit_t fs) {
  deck_t my_hand = *hand;
  size_t hand_count = (*hand).n_cards;
  int straight_ctr = 0;
  int is_ace_low = 1;
  card_t index_card = *(my_hand.cards)[index];
  if (index_card.value == 14) {
    is_ace_low = is_ace_low_straight(hand, hand_count, fs);
    if (is_ace_low == 0) {
      straight_ctr = -1;
      return straight_ctr;
    }
  }

  if (fs != 4) {
    straight_ctr = check_flush_straight(hand, index, fs, hand_count);
  }
  else {
    straight_ctr = check_straight(hand, index, hand_count);
  }
  return straight_ctr;
}

int is_straight_at(deck_t * hand, size_t index, suit_t fs) {
  int straight_type = 0;
  int is_straight = is_straight_at_hlpr(hand, index, fs);
  if (is_straight < 5 && is_straight != -1) {
    straight_type = 0;
  }
  else if (is_straight == -1) {
    straight_type = -1;
  }
  else {
    straight_type = 1;
  }
  return straight_type;
}

hand_eval_t build_hand_from_match(deck_t * hand,
				  unsigned n,
				  hand_ranking_t what,
				  size_t idx) {
  deck_t my_hand = *hand;
  hand_eval_t ans;
  ans.ranking = what;
  
  // Gets the first set of cards used for the hand
  for (size_t i = 0; i < n; i++) {
    ans.cards[i] = my_hand.cards[idx+i];
  }

  if (idx != 0) {
    size_t ans_new_idx = n;
    for (size_t k = 0; k < (5 - n); k++) {
      ans.cards[ans_new_idx] = my_hand.cards[k];
      ans_new_idx++;
    }
  }
  else {
    size_t ans_new_idx = n;
    for (size_t l = n; l < 5; l++) {
      ans.cards[ans_new_idx] = my_hand.cards[l];
      ans_new_idx++;
    }
  }
  return ans;
}

int compare_hands(deck_t * hand1, deck_t * hand2) {
  qsort(hand1->cards, hand1->n_cards, sizeof(hand1->cards[0]), card_ptr_comp);
  qsort(hand2->cards, hand2->n_cards, sizeof(hand2->cards[0]), card_ptr_comp);
  hand_eval_t hand1_eval = evaluate_hand(hand1);
  hand_eval_t hand2_eval = evaluate_hand(hand2);

  if (hand1_eval.ranking > hand2_eval.ranking) {
    return -1;
  }
  else if (hand1_eval.ranking < hand2_eval.ranking) {
    return 1;
  }
  else if (hand1_eval.ranking == hand2_eval.ranking) {
    for (size_t i = 0; i < 5; i++) {
      card_t hand1_temp = *(hand1_eval.cards)[i];
      card_t hand2_temp = *(hand2_eval.cards)[i];
      if (hand1_temp.value > hand2_temp.value) {
	      return 1;
      }
      else if (hand2_temp.value > hand1_temp.value) {
	      return -1;
      }
    }

    // If hands tie with 1 pair, go back to the original, sorted hand
    // compare up to the first 4 cards in the two hands (these will be the highest value cards).
    // If the value of the cards matches the value of the card used as the pair
    // return 0 (tie).
    // If the value of one hand is higher than the value of the other
    // the hand with the higher value card wins.
    // If the first four cards tie in value, the hand is a tie.
    if (hand1_eval.ranking == 7 && hand2_eval.ranking == 7) {
      for (size_t j = 0; j < 4; j++) {
        card_t hand1_win = *(hand1_eval.cards)[j];
        card_t hand2_win = *(hand2_eval.cards)[j];
        card_t hand1_high = *hand1->cards[j];
        card_t hand2_high = *hand2->cards[j];
        if (hand1_high.value > hand2_high.value) {
          if (hand1_high.value == hand1_win.value) {
            return 0;
          }
          else {
            return 1;
          }
        }
        else if (hand2_high.value > hand1_high.value) {
          if (hand2_high.value == hand2_win.value) {
            return 0;
          }
          else {
            return -1;
          }
        }
      }
    }
    return 0;
  }
  else {
    printf("Unable to evaluate hands.\n");
    exit(EXIT_FAILURE);
  }
}

unsigned * get_match_counts(deck_t * hand) {
  size_t hand_counts = hand->n_cards;
  unsigned * match_counts = malloc(hand_counts * sizeof(*match_counts));
  if (hand->n_cards == 0) {
    match_counts = NULL;
    return match_counts;
  }

  dictionary_t * dict = malloc(sizeof(*dict));
  dict->keys = malloc(sizeof(*dict->keys));
  k_t first_key;  
  first_key.k = hand->cards[0]->value;
  first_key.v = 1;
  dict->num_keys = 1;
  dict->keys[0] = first_key;

  for (size_t i = 1; i < hand_counts; i++) {
    int found_key = 0;
    k_t temp_key;
    temp_key.k = hand->cards[i]->value;
    temp_key.v = 1;
    for (int j = 0; j < dict->num_keys; j++) {
	    if (temp_key.k == dict->keys[j].k) {
	      dict->keys[j].v++;
	      found_key = 1;
	      break;
	    }
    }
    if (found_key == 0) {
      dict->keys = realloc(dict->keys, (dict->num_keys + 1) * sizeof(*dict->keys));
      dict->keys[dict->num_keys] = temp_key;
      dict->num_keys++;	
    }
  }

  for (size_t k = 0; k < hand_counts; k++) {
    for (int m = 0; m < dict->num_keys; m++) {
      if (hand->cards[k]->value == dict->keys[m].k) {
	      match_counts[k] = dict->keys[m].v;
      }
    }
  }

  free(dict->keys);
  free(dict);
  return match_counts;
}

// Below functions provided by Coursera course

//This function copies a straight starting at index "ind" from deck "from".
//This copies "count" cards (typically 5).
//into the card array "to"
//if "fs" is NUM_SUITS, then suits are ignored.
//if "fs" is any other value, a straight flush (of that suit) is copied.
void copy_straight(card_t ** to, deck_t *from, size_t ind, suit_t fs, size_t count) {
  assert(fs == NUM_SUITS || from->cards[ind]->suit == fs);
  unsigned nextv = from->cards[ind]->value;
  size_t to_ind = 0;
  while (count > 0) {
    assert(ind < from->n_cards);
    assert(nextv >= 2);
    assert(to_ind <5);
    if (from->cards[ind]->value == nextv &&
	    (fs == NUM_SUITS || from->cards[ind]->suit == fs)) {
      to[to_ind] = from->cards[ind];
      to_ind++;
      count--;
      nextv--;
    }
    ind++;
  }
}

//This looks for a straight (or straight flush if "fs" is not NUM_SUITS)
// in "hand".  It calls the student's is_straight_at for each possible
// index to do the work of detecting the straight.
// If one is found, copy_straight is used to copy the cards into
// "ans".
int find_straight(deck_t * hand, suit_t fs, hand_eval_t * ans) {
  if (hand->n_cards < 5) {
    return 0;
  }
  for (size_t i = 0; i <= hand->n_cards -5; i++) {
    int x = is_straight_at(hand, i, fs);
    if (x != 0) {
      if (x < 0) { //ace low straight
        if (hand->cards[i]->value == VALUE_ACE && hand->cards[i]->suit != fs && fs != 4) {
	        continue;
	      }	
	      assert(hand->cards[i]->value == VALUE_ACE &&
	          (fs == NUM_SUITS || hand->cards[i]->suit == fs));
	      ans->cards[4] = hand->cards[i];
	      size_t cpind = i + 1;
	      while(hand->cards[cpind]->value != 5 ||
	          !(fs==NUM_SUITS || hand->cards[cpind]->suit ==fs)){
	        cpind++;
	        assert(cpind < hand->n_cards);
	      }
	      copy_straight(ans->cards, hand, cpind, fs,4) ;
      }
      else {
	      copy_straight(ans->cards, hand, i, fs,5);
      }
        return 1;
    }
  }
  return 0;
}

//This function puts all the hand evaluation logic together.
//This function is longer than we generally like to make functions,
//and is thus not so great for readability :(
hand_eval_t evaluate_hand(deck_t * hand) {
  suit_t fs = flush_suit(hand);
  hand_eval_t ans;
  
  if (fs != NUM_SUITS) {
    if(find_straight(hand, fs, &ans)) {
      ans.ranking = STRAIGHT_FLUSH;
      return ans;
    }
  }

  unsigned * match_counts = get_match_counts(hand);
  unsigned n_of_a_kind = get_largest_element(match_counts, hand->n_cards);
  assert(n_of_a_kind <= 4);
  size_t match_idx = get_match_index(match_counts, hand->n_cards, n_of_a_kind);
  ssize_t other_pair_idx = find_secondary_pair(hand, match_counts, match_idx);
  free(match_counts);
  
  if (n_of_a_kind == 4) { //4 of a kind
    return build_hand_from_match(hand, 4, FOUR_OF_A_KIND, match_idx);
  }
  else if (n_of_a_kind == 3 && other_pair_idx >= 0) {     //full house
    ans = build_hand_from_match(hand, 3, FULL_HOUSE, match_idx);
    ans.cards[3] = hand->cards[other_pair_idx];
    ans.cards[4] = hand->cards[other_pair_idx+1];
    return ans;
  }
  else if (fs != NUM_SUITS) { //flush
    ans.ranking = FLUSH;
    size_t copy_idx = 0;
    for (size_t i = 0; i < hand->n_cards; i++) {
      if (hand->cards[i]->suit == fs) {
        ans.cards[copy_idx] = hand->cards[i];
        copy_idx++;
        if (copy_idx >=5){
          break;
	      }
      }
    }
    return ans;
  }
  else if (find_straight(hand,NUM_SUITS, &ans)) {     //straight
    ans.ranking = STRAIGHT;
    return ans;
  }
  else if (n_of_a_kind == 3) { //3 of a kind
    return build_hand_from_match(hand, 3, THREE_OF_A_KIND, match_idx);
  }
  else if (other_pair_idx >= 0) {     //two pair
    assert(n_of_a_kind == 2);
    ans = build_hand_from_match(hand, 2, TWO_PAIR, match_idx);
    ans.cards[2] = hand->cards[other_pair_idx];
    ans.cards[3] = hand->cards[other_pair_idx + 1];
    if (match_idx > 0) {
      ans.cards[4] = hand->cards[0];
    }
    else if (other_pair_idx > 2) {  //if match_idx is 0, first pair is in 01
      //if other_pair_idx > 2, then, e.g. A A K Q Q
      ans.cards[4] = hand->cards[2];
    }
    else {       //e.g., A A K K Q
      ans.cards[4] = hand->cards[4]; 
    }
    return ans;
  }
  else if (n_of_a_kind == 2) {
    return build_hand_from_match(hand, 2, PAIR, match_idx);
  }
  return build_hand_from_match(hand, 0, NOTHING, 0);
}
