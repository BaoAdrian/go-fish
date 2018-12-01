//
//  main.c
//  ECE175_FinalProject
//
//  Created by Adrian Bao and Liam Gill on 11/21/2018
//
//  GoFish Game Implemented using Doubly-Linked Lists and Dynamic Memory Management
//

#include <stdio.h>
#include <string.h> // string functions
#include <stdlib.h>
#include <math.h> // Random number generator for shuffling
#include <time.h> // Used to seed the random number generator

const int LINE_SIZE = 15;
const int SUIT_LENGTH = 10;
const int NUM_OF_SWAPS = 200;
const int GUESS_SIZE = 5;

/* Card declaration */
typedef struct card_s {
    char suit[SUIT_LENGTH];
    int value;
    struct card_s *prev;
    struct card_s *next;
} card;

/* Function Prototypes */
void print_title(void);
int get_deck_selection(void);
void generate_random_deck(card **deck_hl, card **deck_hr);
void read_in_deck(card **deck_hl, card **hr);
void add_to_end(card *p, card **hl, card **hr, card *temp_card); // Add card to end of list
card* pull_card_data(char line[]); // Parse line from file into Card Struct
void shuffle_deck(card *hl);
int find_length(card *hl);
int rand_gen(int count);
void swap(card *pt, int i, int j); // Function used to swap cards at index i and j
void print_list(card *card);
void print_formatted_list(card *card); // Prints unicode characters instead of words

card* remove_member(card *p, card **hl, card **hr);
void create_player_hands(card **deck_hl, card **deck_hr, card **p1_hl, card **p1_hr, card **p2_hl, card **p2_hr);
int check_for_book(card *hl);
int convert_guess(char guess[]);
void transfer_cards(int num_of_cards, int guess_rank, card **guesser_hl, card **guesser_hr, card **opp_hl, card **opp_hr);
void go_fish(card **guesser_hl, card **guesser_hr, card **deck_hl, card **deck_hr);
int check_for_winner(card *p1_hl, card *p2_hl);

void remove_book(int rank, card **player_hl, card **player_hr);
int process_guess(int guess_rank, card **p1_hl, card **p1_hr, card **p2_hl, card **p2_hr, card **deck_hl, card **deck_hr);


/*
 * NOTES REGARDING RANKS AND THEIR INTEGER EQUIVALENT
 * - Values 2-10 remain unchanged. A, J, Q, K had to be adjusted as follows:
 * - A -> 1
 * - J -> 11
 * - Q -> 12
 * - K -> 13
 * Used the values above will allow for ease of processing guess depending on what the user enters.
 */

int main(void) {
    
    // Print header
    print_title();
    
    // Seed the RNG
    srand((int)time(NULL));
    
    /* Variable Declarations */
    int deck_init; // Selection of which deck they'd like to start with, file or random shuffled deck
    int players_turn = 1; // Binary 1 or 2 that alternates at each players turn
    char guess[GUESS_SIZE];
    int guess_rank;
    
    // Declare head and tail pointer to keep track of each end of the list
    card *deck_hl = NULL;
    card *deck_hr = NULL;
    
    
    // Players hands
    card *player1_hl = NULL;
    card *player1_hr = NULL;
    card *player2_hl = NULL;
    card *player2_hr = NULL;
    
    // Get user selection: use shuffled deck(0) or use preformatted file input (1)
    deck_init = get_deck_selection();
    
    
    if (deck_init == 0) {
        
        generate_random_deck(&deck_hl, &deck_hr);
        
        printf("GENERATED DECK: \n");
        print_formatted_list(deck_hl);
        
        shuffle_deck(deck_hl);
        
        printf("\n\nSHUFFLED DECK: \n");
        print_formatted_list(deck_hl);
        
    } else if (deck_init == 1) {
        
        read_in_deck(&deck_hl, &deck_hr);
        
        printf("\nDECK FROM FILE: \n");
        print_formatted_list(deck_hl);
        
    }
    
    printf("\n\nRest of this project is under construction. Come back soon!\n\n");
    
    /************************************************************************
     * Generating player decks portion                                      *
     ************************************************************************/
    
    printf("ORIGINAL POOL\n");
    print_formatted_list(deck_hl);
    
    // create_player_hands(card *deck_hl, card *deck_hr, card *p1_hl, card *p1_hr, card *p2_hl, card *p2_hr, card **p1_hl, card** p1_hr, card **p2_hl, card **p2_hr)
    create_player_hands(&deck_hl, &deck_hr, &player1_hl, &player1_hr, &player2_hl, &player2_hr);
    
    printf("\n\nNEW DECK:\n");
    print_formatted_list(deck_hl);
    
    printf("\n\nPLAYER 1 HAND: \n");
    print_formatted_list(player1_hl);
    
    printf("PLAYER 2 HAND: \n");
    print_formatted_list(player2_hl);
    
    
    
    /************************************************************************
     * Game Functionality Implementation                                    *
     ************************************************************************/
    printf("\n");
    
    // First, we must check to see if the cards that were dealt give a player a book
    // (highly unlikely) but necessary nonetheless.
    int book_value = check_for_book(player1_hl);
    if (book_value != 0) {
        printf("BOOK DETECTED\n");
        // Remove book that contains value book_value
        remove_book(book_value, &player1_hl, &player1_hr);
    } else {
        printf("NO BOOK DETECTED\n");
        // Proceed
    }
    book_value = check_for_book(player2_hl);
    if (book_value != 0) {
        printf("BOOK DETECTED\n");
        // Remove book that contains value book_value
        remove_book(book_value, &player2_hl, &player2_hr);
    } else {
        printf("NO BOOK DETECTED\n");
        // Proceed
    }
    
    
    
    // Player 1 will play first, they will perform a query on the opposing players hand
    // If the opposing player contains the rank they guess, they must surrender all cards
    // of that rank.
    
    /*
     * NOTE: Player1 -> turn = 1
     *       Player2 -> turn = 2
     */
    
    // Play until a winner is detected
    while(check_for_winner(player1_hl, player2_hl) != 1) {

        if (players_turn == 1) {
            // Player 1 goes
            printf("Player 1, Make a Guess (please enter A, 2-10, J, Q, or K): \n");
            printf("Guess: ");
            scanf("%s", guess);

            guess_rank = convert_guess(guess);
            
            if (process_guess(guess_rank, &player1_hl, &player1_hr, &player2_hl, &player2_hr, &deck_hl, &deck_hr)) {
                // Card was found and moved, maintain turn
                players_turn = 1;
            } else {
                players_turn = 2;
            }
            
        } else if (players_turn == 2) {
            // Player 2 goes
            printf("Player 2, Make a Guess (please enter A, 2-10, J, Q, or K): \n");
            printf("Guess: ");
            scanf("%s", guess);
            
            guess_rank = convert_guess(guess);
            
            if (process_guess(guess_rank, &player2_hl, &player2_hr, &player1_hl, &player1_hr, &deck_hl, &deck_hr)) {
                // Card was found and moved, maintain turn
                players_turn = 2;
            } else {
                players_turn = 1;
            }
            
        }
        
        // Print hands after each turn
        printf("\n\nPLAYER 1 HAND: \n");
        print_formatted_list(player1_hl);
        
        printf("PLAYER 2 HAND: \n");
        print_formatted_list(player2_hl);

    }
    
    printf("WE HAVE A WINNER!");
    
    
    

    
    return 0;
}


/************************************************************************
 * print_title(): Title to add to the UI Element of the program         *
 ************************************************************************/
void print_title() {
    
    printf(" -----   -----   -----   -----   -----   -----\n");
    printf("|A\u2665   | |K\u2665   | |Q\u2665   | |J\u2665   | |10\u2665  | |9\u2665   |\n");
    printf("|     | |     | |     | |     | |     | |     |\n");
    printf("|   A\u2665| |   K\u2665| |   Q\u2665| |   J\u2665| |  10\u2665| |   9\u2665|\n");
    printf(" -----   -----   -----   -----   -----   -----\n");
    
    //  Prints START
    printf(" -----    __  _____  ___   ___   _____   -----\n");
    printf("|A\u2663   |  |      |   |   | |   |    |    |A\u2663   |\n");
    printf("|     |   --    |   |---| | --     |    |     |\n");
    printf("|   A\u2663|     |   |   |   | |  \\     |    |   A\u2663|\n");
    printf(" -----    --    |   |   | |   \\    |     -----\n");
    
    printf(" -----   -----   -----   -----   -----   -----\n");
    printf("|9\u2660   | |10\u2660  | |J\u2660   | |Q\u2660   | |K\u2660   | |A\u2660   |\n");
    printf("|     | |     | |     | |     | |     | |     |\n");
    printf("|   9\u2660| |  10\u2660| |   J\u2660| |   Q\u2660| |   K\u2660| |   A\u2660|\n");
    printf(" -----   -----   -----   -----   -----   -----\n\n");

}


/************************************************************************
 * get_deck_selection(): Function that returns the binary choice (0/1)  *
 *      that the user selected as the deck generation method.           *
 *                                                                      *
 * Parameters - None                                                    *
 ************************************************************************/
int get_deck_selection(void) {
    
    int deck_selection;
    
    printf("Would you like a shuffled deck (0) or a deck provided from a file (1)? Please choose 0 or 1: ");
    scanf("%d", &deck_selection);
    
    // Loop until valid delection is made
    while (deck_selection != 0 && deck_selection != 1) {
        printf("\nERROR, that is not a valid selection.\n");
        printf("Would you like a shuffled deck (0) or a deck provided from a file (1)? Please choose 0 or 1: ");
        scanf("%d", &deck_selection);
    }
    
    return deck_selection;
    
}


/************************************************************************
 * generate_random_deck() Function that generates a random deck cycling *
 *      through suits and ranks using nested for loops. This generates  *
 *      an ordered deck so it is then passed to the shuffle method to   *
 *      essentially generate a randomly generated deck.                 *
 ************************************************************************/
void generate_random_deck(card **deck_hl, card **deck_hr) {
    
    char suits[4] = {'h', 'd', 'c', 's'};
    
    // Construct a standard sequential deck then shuffle it 200 times
    for (int i = 1; i <= 13; i++) {
        for (int j = 0; j < 4; j++) {
            card *temp_card = (card*)malloc(sizeof(card));
            temp_card->value = i;
            if (suits[j] == 'h') {
                strcpy(temp_card->suit, "hearts");
            } else if (suits[j] == 'd') {
                strcpy(temp_card->suit, "diamonds");
            } else if (suits[j] == 'c') {
                strcpy(temp_card->suit, "clubs");
            } else if (suits[j] == 's') {
                strcpy(temp_card->suit, "spades");
            }
            add_to_end(*deck_hr, deck_hl, deck_hr, temp_card);
        }
    }
}


/************************************************************************
 * read_in_deck(): Function that will attempt to read a file that is    *
 *      formatted in a specfic way so that the function can parse the   *
 *      line and populate a deck of 52 cards.                           *
 ************************************************************************/
void read_in_deck(card **deck_hl, card **deck_hr) {
    
    
    char line[LINE_SIZE];
    
    // Load deck from preformatted file
    FILE *inp;
    inp = fopen("ordered_deck.txt", "r");
    if (inp == NULL) {
        printf("ERROR: Could not open file. Ending Program\n");
        exit(-1);
    }
    
    // Loop through file reading and parsing contents line by line
    while (fgets(line, LINE_SIZE, inp) != NULL) {
        card *temp_card = (card*)malloc(sizeof(card));
        temp_card = pull_card_data(line); // Parse data from line
        add_to_end(*deck_hr, deck_hl, deck_hr, temp_card);
    }
    
}


/************************************************************************
 * add_to_end(): Function that adds a Card Struct to the end of the     *
 *      Doubly-Linked List and makes the necessary changes to handle    *
 *      all cases of Linked List construction.                          *
 *                                                                      *
 * Parameters: p - Card Object that head-right is pointing to           *
 *             hl/hr - Double pointer that allows function to change    *
 *                  the value of hl/hr without having to return both.   *
 *             temp_card - Card Object that needs to be added to end.   *
 ************************************************************************/
void add_to_end(card *p, card **hl, card **hr, card *temp_card) {
    
    if (*hl == NULL) {
        // List is empty
        *hl = temp_card;
        *hr = temp_card;
        temp_card->next = NULL;
        temp_card->prev = NULL;
    } else if (p->next == NULL) {
        // Add to the end of the list
        p->next = temp_card;
        temp_card->prev = p;
        temp_card->next = NULL;
        *hr = temp_card;
    }
    
}


/************************************************************************
 * pull_card_data(): Function that accepts the current line being read  *
 *      from the file and uses the pre-determined format to parse the   *
 *      data, create a Card Struct and return that Card Struct.         *
 *                                                                      *
 * Parameters: line - current line being read from the file.            *
 * -> format of line: Rank Suit ----> EXAMPLE: 8 spades OR K diamonds   *
 ************************************************************************/
card* pull_card_data(char line[]) {
    
    char *temp;
    int i;
    card *temp_card = (card*)malloc(sizeof(card));
    
    // Remove trailing new line when reading from file
    line[strlen(line) - 1] = '\0';
    
    // Parse data and begin process of creating Linked List
    if (line[0] == 'J') {
        temp_card->value = 11;
    } else if (line[0] == 'Q') {
        temp_card->value = 12;
    } else if (line[0] == 'K') {
        temp_card->value = 13;
    } else {
        temp_card->value = atoi(&line[0]); // Pull integer value
    }
    temp = &line[2]; // Set pointer to begin pulling suit
    i = 0;
    
    // Move pointer one more (covers the 10 case)
    if (*temp == ' ') {
        temp++;
    }
    // Loop until the end of the line, saving them into suit attribute
    while (*temp != '\0') {
        temp_card->suit[i] = *temp;
        temp++;
        i++;
    }
    
    temp_card->suit[i] = '\0'; // terminate the string
    
    return temp_card;
    
}


/************************************************************************
 * shuffle_deck(): Function that accepts the pointer to the head of the *
 *      LinkedList and uses the reference as a starting point for the   *
 *      shuffling method. Performs a specifiec number of swaps on two   *
 *      randomly selected indices of the LinkedList.                    *
 ************************************************************************/
void shuffle_deck(card *hl) {
    
    // Find number of cards present in the pool
    int num_cards = find_length(hl);
    
    int num_swaps = NUM_OF_SWAPS;
    int idx_1, idx_2;
    for (int i = 0; i < num_swaps; i++) {
        idx_1 = rand_gen(num_cards);
        idx_2 = rand_gen(num_cards);
        
        // Need to ensure that we don't swap a card with itself
        while (idx_2 == idx_1) {
            // Grab another random index
            idx_2 = rand_gen(num_cards);
        }
        swap(hl, idx_1, idx_2);
    }
    
}


/************************************************************************
 * find_length(): Function that traverses the Linked List and counts    *
 *      returns the number of Card Structs that exist in the list.      *
 *                                                                      *
 * Parameters: hl - head of the list to be traversed, used as the start *
 *      point for the traversal. Iterates until NEXT ptr is NULL        *
 ************************************************************************/
int find_length(card *hl) {
    
    int length = 0;
    
    card *curr = (card*)malloc(sizeof(card));
    curr = hl;
    
    while (curr != NULL) {
        length++;
        curr = curr->next;
    }
    
    return length;
}


/************************************************************************
 * rand_gen(): Function that generates a random number from 0 to length *
 *      of the LinkedList in order to pull two indices to swap their    *
 *      values (i.e. their value and suit of card)                      *
 ************************************************************************/
int rand_gen(int count) {
    double frac;
    frac = (double)rand()/((double)RAND_MAX+1);
    return floor(count*frac);
}


/************************************************************************
 * swap(): Function that accepts the two target indices and the pointer *
 *      to the head of the LinkedList. Traverses the list pulling the   *
 *      nodes at the given indices (i, j) then performs a swap of the   *
 *      value and suit of the card, essentially swapping cards          *
 ************************************************************************/
void swap(card *pt, int i, int j) {
    
    card *temp = pt;
    card *node_1 = NULL;
    card *node_2 = NULL;
    int flag;
    
    // Pull Node 1
    flag = 0; // start at beginning of list
    while (flag < i) {
        temp = temp->next;
        flag++;
    }
    node_1 = temp;
    
    // Reset and Pull Node 2
    temp = pt;
    flag = 0;
    
    while (flag < j) {
        temp = temp->next;
        flag++;
    }
    node_2 = temp;
    
    // Temporary values
    int temp_value;
    char temp_suit[SUIT_LENGTH];
    
    temp_value = node_1->value;
    strcpy(temp_suit, node_1->suit);
    
    node_1->value = node_2->value;
    strcpy(node_1->suit, node_2->suit);
    
    node_2->value = temp_value;
    strcpy(node_2->suit, temp_suit);
    
}


/************************************************************************
 * print_list(): Standard function that traverses the Linked List and   *
 *      prints the formatted Card Struct at each stop along the way.    *
 *      Generic in the formatted output due to no unicode usage.        *
 *                                                                      *
 * Parameters: p - head of the list to be traversed, used as the start  *
 *      point for the traversal. Iterates until NEXT ptr is NULL        *
 ************************************************************************/
void print_list(card *p) {
    card *curr = p;
    while (curr != NULL) {
        printf("[%d : %s] -> ", curr->value, curr->suit);
        curr = curr->next;
    }
    printf("NULL\n");
}


/************************************************************************
 * print_formatted_list(): Print function that outputs the formatted    *
 *      values of the struct by converting the suit into their unicode  *
 *      equivalent.
 *                                                                      *
 * Parameters: p - head of the list to be traversed, used as the start  *
 *      point for the traversal. Iterates until NEXT ptr is NULL        *
 ************************************************************************/
void print_formatted_list(card *p) {
    card *curr = p;
    while (curr != NULL) {
        if (strcmp(curr->suit, "hearts") == 0) {
            printf("[%d : \u2665] -> ", curr->value);
        } else if (strcmp(curr->suit, "diamonds") == 0){
            printf("[%d : \u2666] -> ", curr->value);
        } else if (strcmp(curr->suit, "spades") == 0){
            printf("[%d : \u2660] -> ", curr->value);
        } else if (strcmp(curr->suit, "clubs") == 0){
            printf("[%d : \u2663] -> ", curr->value);
        }
        curr = curr->next;
    }
    printf("NULL\n");
}




/*********************************************/
/* Rest of the Project to be completed below */
/*********************************************/





/************************************************************************
 * delete_member() Function that accepts the pointer and addresses,     *
 *      similar to the add_to_end function, and removes a card at that  *
 *      pointer p is pointing to. Makes the necessary adjustments to    *
 *      LinkedList pointers and returns the card removed from the list. *
 ************************************************************************/
card* remove_member(card *p, card **hl, card **hr) {
    if (p == *hl)         // if deleting the first element
        *hl = p->next;     // update the left headp
    else
        p->prev->next = p->next;
    if (p == *hr)
        *hr = p->prev;     // if deleting the last element
    else
        p->next->prev = p->prev;
    return p;
}


/************************************************************************
 * create_player_hand(): Function that generates the hands for players  *
 *      with the use of double pointers allowing us to make adjustments *
 *      within the various function calls and update the players hands  *
 *      as well as the pool of cards all within this function.          *
 * Logic - Uses a for loop that iterates 7 times, dealing cards to each *
 *      player, one at a time, removing from the pool LinkedList and    *
 *      adding to the player hands LinkedList.                          *
 ************************************************************************/
void create_player_hands(card **deck_hl, card **deck_hr, card **player1_hl, card **player1_hr, card **player2_hl, card **player2_hr) {
    
    for (int i = 0; i < 7; i++) {
        // Remove 1 card from top of pool (head-left of the deck)
        card *p1_card = (card*)malloc(sizeof(card));
        p1_card = remove_member(*deck_hl, deck_hl, deck_hr);
        
        // Add to player 1's hand
        add_to_end(*player1_hr, player1_hl, player1_hr, p1_card);
        
        // Remove 1 card from top of pool (head-left of the deck)
        card *p2_card = (card*)malloc(sizeof(card));
        p2_card = remove_member(*deck_hl, deck_hl, deck_hr);
        
        // Add to player 2's hand
        add_to_end(*player2_hr, player2_hl, player2_hr, p2_card);
        
    }
    
}


/************************************************************************
 * convert_guess(): Function that will accept a user query in the form  *
 *      of a string and convert it to its integer equivalent.           *
 ************************************************************************/
int convert_guess(char guess[]) {
    
    // Will convert the A, J, Q, K inputs to their respective rank values
    // If value 2-10 entered, will return the integer equivalent
    if (strcmp(guess, "A") == 0) {
        return 1;
    } else if (strcmp(guess, "J") == 0) {
        return 11;
    } else if (strcmp(guess, "Q") == 0) {
        return 12;
    } else if (strcmp(guess, "K") == 0) {
        return 13;
    } else {
        return atoi(guess);
    }
    
}





/************************************************************************
 * check_for_winner(): Function that checks to see if either of the     *
 *      players hands has a length of zero, if so, that indicates that  *
 *      a hand is empty and a winner has been found.                    *
 *      Returns 1 if winner is detected, 0 otherwise.                   *
 ************************************************************************/
int check_for_winner(card *p1_hl, card *p2_hl) {
    if ((find_length(p1_hl) == 0) || (find_length(p2_hl) == 0)) {
        // One of the hands is empty, therefore a player has won, return 1
        return 1;
    } else {
        // No winner found, return 0
        return 0;
    }
}


/************************************************************************
 * check_for_book(): Function that will traverse the LinkedList of the  *
 *      list passed in and search for a rank that occurs 4 times in the *
 *      hand, i.e. a book. If one is found, it will return the value    *
 *      of the book that was found, otherwise, it will return 0.        *
 ************************************************************************/
int check_for_book(card *hl) {
    // Create array of size List Length
    int length = find_length(hl);
    int count = 0;
    int occurrence_array[length];
    
    // populate array with values
    card *temp = (card*)malloc(sizeof(card));
    temp = hl;
    
    for (int i = 0; i < length; i++) {
        occurrence_array[i] = temp->value;
        temp = temp->next;
    }
    
    // Reset temp and iterate through the list
    for (int i = 0; i < length; i++) {
        count = 0;
        temp = hl;
        while (temp != NULL) {
            if (temp->value == occurrence_array[i]) {
                count++;
            }
            if (count == 4) {
                // Return actual value of the book
                return temp->value;
            }
            temp = temp->next;
        }
    }
    // If reached this point, no book was found, return 0
    return 0;
}


/************************************************************************
 * process_guess(): Function that accepts a guess_rank and traverses    *
 *      the opponents hand searching for any cards that share that same *
 *      rank. If so, keeps a count so that all cards of that rank can   *
 *      be transferred from opponent's hand to the guesser's hand.      *
 ************************************************************************/
int process_guess(int guess_rank, card **guesser_hl, card **guesser_hr, card **opp_hl, card **opp_hr, card **deck_hl, card **deck_hr) {
   
    // Iterate through the list and see if rank exists in hand, if so, how many.
    int num_of_cards = 0;
    int book_value;
    
    card *temp = (card*)malloc(sizeof(card));
    
    temp = *opp_hl;
    while (temp != NULL) {
        if (temp->value == guess_rank) {
            num_of_cards++;
        }
        temp = temp->next;
    }
    
    /* Verified up to this point */
    
    // Now we check to see if any cards exist
    if (num_of_cards > 0) {
        // There is a card that needs to be transfered from opponenets deck to guessers deck
        transfer_cards(num_of_cards, guess_rank, guesser_hl, guesser_hr, opp_hl, opp_hr);
        book_value = check_for_book(*guesser_hl);
        if (book_value != 0) {
            remove_book(book_value, guesser_hl, guesser_hr);
        }
        return 1;
    } else {
        // Card was not found, therefore, GOFISH occurs
        printf("NOPE! GO FISH!\n");
        go_fish(guesser_hl, guesser_hr, deck_hl, deck_hr);
        book_value = check_for_book(*guesser_hl);
        if (book_value != 0) {
            remove_book(book_value, guesser_hl, guesser_hr);
        }
        return 0;
    }
    
}


/************************************************************************
 * transfer_cards(): Function that is called when a transfer needs to   *
 *      occur. Will traverse the opponents hand, num_of_cards number of *
 *      times since that is how many cards need to be transferred. The  *
 *      cards will be removed from opponents hand and added to the      *
 *      guessers hand.                                                  *
 ************************************************************************/
void transfer_cards(int num_of_cards, int guess_rank, card **guesser_hl, card **guesser_hr, card **opp_hl, card **opp_hr) {
    
    // Loop through to get the number of cards present in users hand
    for (int i = 0; i < num_of_cards; i++) {
        
        // Traverse list, finding the cards, removing it, and adding to guessers hand
        card *temp = (card*)malloc(sizeof(card));
        temp = *opp_hl;
        
        while (temp != NULL) {
            if (temp->value == guess_rank) {
                // Remove this card and add to guessers hand
                add_to_end(*guesser_hr, guesser_hl, guesser_hr, remove_member(temp, opp_hl, opp_hr));
            }
            temp = temp->next;
        }
        
    }
    
    // Now player two no longer possesses any card of rank, guess_rank.
    // Guessing player now possesses all cards of rank, guess_rank, that opponent once had
    
}


/************************************************************************
 * go_fish(): Function that will be called when a player who guesses    *
 *      for a card that does not exist in the other players hand. When  *
 *      called, will remove the top card from the deck and add to the   *
 *      guessers hand.                                                  *
 ************************************************************************/
void go_fish(card **guesser_hl, card **guesser_hr, card **deck_hl, card **deck_hr) {
    card *drawn_card = (card*)malloc(sizeof(card));
    drawn_card = remove_member(*deck_hl, deck_hl, deck_hr);
    add_to_end(*guesser_hr, guesser_hl, guesser_hr, drawn_card);
}


/************************************************************************
 * remove_book(): Function that accepts a specific rank to remove from  *
 *      the players hand. The rank is the value of the card that        *
 *      completes the book that will be removed.                        *
 ************************************************************************/
void remove_book(int rank, card **player_hl, card **player_hr) {
    
    // Iterate 4 times (4 is the amount of a completed book)
    for (int i = 0; i < 4; i++) {
        
        // Traverse the hand and remove the card with value of rank
        card *temp = (card*)malloc(sizeof(card));
        temp = *player_hl;
        
        while (temp != NULL) {
            if (temp->value == rank) {
                // Remove this card
                free(remove_member(temp, player_hl, player_hr)); // Free memory taken by the card (no longer needed)
                break;
            }
            temp = temp->next;
        }
    }
}


