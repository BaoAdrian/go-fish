//
//  main.c
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

// Support for prior C99 Machines
//#define FILENAME_SIZE 30
//#define LINE_SIZE 15
//#define SUIT_LENGTH 10
//#define NUM_OF_SWAPS 200
//#define GUESS_SIZE 5
//#define FORCE_SWAP 3
//#define CARD_LIMIT 7
//#define PLAYER_ONE 1
//#define PLAYER_TWO 2

const int FILENAME_SIZE = 30;
const int LINE_SIZE = 15;
const int SUIT_LENGTH = 10;
const int NUM_OF_SWAPS = 200;
const int GUESS_SIZE = 5;
const int FORCE_SWAP = 3;
const int CARD_LIMIT = 7; // Limits the number of cards that can be displayed in one row
const int PLAYER_ONE = 1;
const int PLAYER_TWO = 2;
 
/* Card declaration */
typedef struct card_s {
    char suit[SUIT_LENGTH];
    int value;
    struct card_s *prev;
    struct card_s *next;
} card;

/* Function Prototypes */
void print_go_fish_title(void);
void print_list(card *card);
void print_formatted_list(card *card); 
void print_go_fish(void);
void print_hand(card *hl);
void print_leftside_card(card *card);
void print_rightside_card(card *card);
int get_deck_selection(void);
void generate_random_deck(card **deck_hl, card **deck_hr);
void read_in_deck(card **deck_hl, card **hr);
void add_to_end(card *p, card **hl, card **hr, card *temp_card);
card* pull_card_data(char line[]);
void shuffle_deck(card *hl);
int find_length(card *hl);
int rand_gen(int count);
void swap(card *pt, int i, int j);
card* remove_member(card *p, card **hl, card **hr);
void create_player_hands(card **deck_hl, card **deck_hr, card **p1_hl, card **p1_hr, card **p2_hl, card **p2_hr);
int guess_a_card(int players_turn, int **p1_score_ptr, card **player1_hl, card **player1_hr, card **player2_hl, card **player2_hr, card **deck_hl, card **deck_hr);
int validate_guess(char *guess);
int validate_possession(int guess_rank, card *guesser_hl);
int convert_guess(char guess[]);
char convert_rank(int rank);
int check_if_playable(card *p1_hl, card *p2_hl, card *deck_hl);
int check_for_winner(int* p1_score, int *p2_score);
int check_for_book(card *hl);
int process_guess(int guesser, int guess_rank, int **player_score,  card **p1_hl, card **p1_hr, card **p2_hl, card **p2_hr, card **deck_hl, card **deck_hr);
void transfer_cards(int num_of_cards, int guess_rank, card **guesser_hl, card **guesser_hr, card **opp_hl, card **opp_hr);
void go_fish(card **guesser_hl, card **guesser_hr, card **deck_hl, card **deck_hr);
void remove_book(int rank, card **player_hl, card **player_hr);
void declare_winner(int p1_score, int p2_score);


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
    print_go_fish_title();
    
    // Seed the RNG
    srand((int)time(NULL));
    
    /* Variable Declarations */
    int deck_init; // Selection of which deck they'd like to start with, file or random shuffled deck
    int players_turn = 1; // Binary 1 or 2 that alternates at each players turn
    int flag = 0;
    
    // Declare head and tail pointer to keep track of each end of the list
    card *deck_hl = NULL;
    card *deck_hr = NULL;
    
    
    // Players hands
    card *player1_hl = NULL;
    card *player1_hr = NULL;
    card *player2_hl = NULL;
    card *player2_hr = NULL;
    int player1_score = 0;
    int player2_score = 0;
    int *p1_score_ptr = &player1_score;
    int *p2_score_ptr = &player2_score;
    
    
    // Get user selection: use shuffled deck(0) or use preformatted file input (1)
    deck_init = get_deck_selection();
    
    // Generate deck based on selectiong
    if (deck_init == 0) {
        
        generate_random_deck(&deck_hl, &deck_hr);
        shuffle_deck(deck_hl);
        printf("*********************************\n");
        printf("* GENERATED DECK:               *\n");
        printf("*********************************\n");
        print_hand(deck_hl);
        
    } else if (deck_init == 1) {
        
        read_in_deck(&deck_hl, &deck_hr);
        printf("*********************************\n");
        printf("* DECK FROM FILE:               *\n");
        printf("*********************************\n");
        print_hand(deck_hl);
        
    }
    
    // Generate player hands and display the table before gameplay starts
    create_player_hands(&deck_hl, &deck_hr, &player1_hl, &player1_hr, &player2_hl, &player2_hr);
    
    // printf("*********************************\n");
    // printf("* PLAYER 1 HAND:                *\n");
    // printf("*********************************\n");
    // print_hand(player1_hl);
    
    // printf("*********************************\n");
    // printf("* PLAYER 2 HAND:                *\n");
    // printf("*********************************\n");
    // print_hand(player2_hl);

    
    
    // Start the Gameplay
    printf("\n\n*********************************\n");
    printf("* HANDS DEALT! LET'S BEGIN!     *\n");
    printf("*********************************\n\n");
    
    // First check highly unlikely case where a player is dealt a book at start of game.
    int book_value = check_for_book(player1_hl);
    if (book_value != 0) {
        // Book Detected, remove and increment score
        remove_book(book_value, &player1_hl, &player1_hr);
        player1_score++;
    } else {
        // No Book detected, Proceed
    }
    book_value = check_for_book(player2_hl);
    if (book_value != 0) {
        // Book Detected, remove and increment score
        remove_book(book_value, &player2_hl, &player2_hr);
        player2_score++;
    } else {
        //No Book detected, Proceed
    }
    
    
    
    // Now loop until a winner is declared
    while(check_if_playable(player1_hl, player2_hl, deck_hl) != 1) {
        
        // Check the hands to ensure the game is still playable or break out if game is over
        if (find_length(player1_hl) == 0) {
            // Hand is empty, check to see if there are any more cards to draw from the deck
            if (find_length(deck_hl) == 0) {
                // Pool is empty, therefore game is over
                break;
            } else {
                printf("PLAYER 1 RAN OUT OF CARDS! DRAW A CARD\n");
                go_fish(&player1_hl, &player1_hr, &deck_hl, &deck_hr);
                flag = FORCE_SWAP;
            }
        }
        
        // Check the hands to ensure the game is still playable or break out if game is over
        if (find_length(player2_hl) == 0) {
            // Hand is empty, check to see if there are any more cards to draw from the deck
            if (find_length(deck_hl) == 0) {
                // Pool is empty, therefore game is over
                break;
            } else {
                printf("PLAYER 2 RAN OUT OF CARDS! DRAW A CARD\n");
                go_fish(&player2_hl, &player2_hr, &deck_hl, &deck_hr);
                flag = FORCE_SWAP;
            }
        }

        if (flag == FORCE_SWAP) {
            printf("SWITCHING TURNS!\n");
            if (players_turn == PLAYER_ONE) {
                players_turn = PLAYER_TWO;
            } else {
                players_turn = PLAYER_ONE;
            }
            flag = 0; // Reset flag to playable state
        } else {
            if (players_turn == PLAYER_ONE) {
                // Execute entire processing of a guess within this function call
                printf("\n*********************************\n");
                printf("* PLAYER 1 HAND:                *\n");
                printf("*********************************\n");
                print_hand(player1_hl);
                players_turn = guess_a_card(players_turn, &p1_score_ptr, &player1_hl, &player1_hr, &player2_hl, &player2_hr, &deck_hl, &deck_hr);
            } else if (players_turn == PLAYER_TWO) {
                // Execute entire processing of a guess within this function call
                printf("\n*********************************\n");
                printf("* PLAYER 2 HAND:                *\n");
                printf("*********************************\n");
                print_hand(player2_hl);
                players_turn = guess_a_card(players_turn, &p2_score_ptr, &player2_hl, &player2_hr, &player1_hl, &player1_hr, &deck_hl, &deck_hr);
            }
        }
        
        if (check_for_winner(p1_score_ptr, p2_score_ptr) != 0) {
            // A winner has been found, declare the winner
            break;
        }
        

    } // end gameplaye whileloop
    
    declare_winner(*p1_score_ptr, *p2_score_ptr);
    printf("\n\nTHANKS FOR PLAYING!\n\n");
 
    // free memory
    free(deck_hl);
    free(deck_hr);
    free(player1_hl);
    free(player1_hr);
    free(player2_hl);
    free(player2_hr);
    
    
    return 0;
}


/************************************************************************
 * print_title(): Title to add to the UI Element of the program         *
 ************************************************************************/
void print_go_fish_title() {
    printf("\n><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>");
    printf("\n><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n");
    printf("><((('>    ____     _____      _____ _____  _____                 ><((('>\n");
    printf("><((('>  /         |     |    |        |   |       |    |   | |   ><((('>\n");
    printf("><((('> |    ____  |     |    |___     |   |_____  |____|   | |   ><((('>\n");
    printf("><((('> |      |\\  |     |    |        |        |  |    |   | |   ><((('>\n");
    printf("><((('>  \\____/    |_____|    |      __|__ _____|  |    |   o o   ><((('>\n");
    printf("><((('>                                                           ><((('>\n");
    printf("><((('>  ><((('> ~~~ ><((('> ~~~ ><((('> ~~~ ><((('> ~~~ ><((('>  ><((('>\n");
    printf("><((('>                                                           ><((('>\n");
    printf("><((('>  RULES THAT THIS PROGRAM ABIDES BY:                       ><((('>\n");
    printf("><((('>  1. At the start of the game, the user may select to      ><((('>\n");
    printf("><((('>     have a deck generated or read from a file.            ><((('>\n");
    printf("><((('>  2. Each player (game supports 2 players) will be dealt   ><((('>\n");
    printf("><((('>     7 cards from the generetaed deck and displayed.       ><((('>\n");
    printf("><((('>  3. Once the hands of each player are displayed, gameplay ><((('>\n");
    printf("><((('>     will begin with Player 1 starting the game.           ><((('>\n");
    printf("><((('>  4. If a book (4 cards of the same rank) is detected in   ><((('>\n");
    printf("><((('>     a players hand at any point in the game, that book    ><((('>\n");
    printf("><((('>     will be removed from the players hand and their score ><((('>\n");
    printf("><((('>     will be incremented.                                  ><((('>\n");
    printf("><((('>  5. When a player guesses a rank against opponents hand   ><((('>\n");
    printf("><((('>     and the opponent holds ANY number of cards containing ><((('>\n");
    printf("><((('>     that rank, they must surrender the cards to guessing  ><((('>\n");
    printf("><((('>     player. Guessing player maintains the turn.           ><((('>\n");
    printf("><((('>  6. When a player guesses a rank against opponents hand   ><((('>\n");
    printf("><((('>     and the opponent does NOT hold any cards of that rank ><((('>\n");
    printf("><((('>     GO FISH occurs and the guessing player draws a card   ><((('>\n");
    printf("><((('>     from the pool. Turn is then switched to opponent.     ><((('>\n");
    printf("><((('>  7. If at any point in the game, one of the players hand  ><((('>\n");
    printf("><((('>     is empty, i.e. if the guessing player has taken all   ><((('>\n");
    printf("><((('>     of their cards, then the player must draw one card    ><((('>\n");
    printf("><((('>     from the pool and the turns are switched.             ><((('>\n");
    printf("><((('>  8. A winner is declared when either all books have been  ><((('>\n");
    printf("><((('>     collected or one of the players collects a total of   ><((('>\n");
    printf("><((('>     7 complete books. At that point the scores wil be     ><((('>\n");
    printf("><((('>     displayed and a final message will declare a winner.  ><((('>\n");
    printf("><((('>  9. In the rare event that the pool is empty and one of   ><((('>\n");
    printf("><((('>     players also has an empty hand, the game is then      ><((('>\n");
    printf("><((('>     deemed unplayable and a winner is declared based on   ><((('>\n");
    printf("><((('>     the score that it is currently at.                    ><((('>\n");
    printf("><((('>                                                           ><((('>\n");
    printf("><((('>                                                           ><((('>\n");
    printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n");
    printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n\n");
}


/************************************************************************
 * print_go_fish(): Function that prints large title to signify go fish *
 ************************************************************************/
void print_go_fish() {
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // Clear Screen
    printf("\n><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n");
    printf("><((('>    ____     _____      _____ _____  _____                 ><((('>\n");
    printf("><((('>  /         |     |    |        |   |       |    |   | |   ><((('>\n");
    printf("><((('> |    ____  |     |    |___     |   |_____  |____|   | |   ><((('>\n");
    printf("><((('> |      |\\  |     |    |        |        |  |    |   | |   ><((('>\n");
    printf("><((('>  \\____/    |_____|    |      __|__ _____|  |    |   o o   ><((('>\n");
    printf("><((('>                                                           ><((('>\n");
    printf("><((('>     Incorrect Guess! Draw a card! Switching Turns!        ><((('>\n");
    printf("><((('>                                                           ><((('>\n");
    printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n\n");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // Clear screen
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
        printf("[%d %s] -> ", curr->value, curr->suit);
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
            printf("[%d \u2665] -> ", curr->value);
        } else if (strcmp(curr->suit, "diamonds") == 0){
            printf("[%d \u2666] -> ", curr->value);
        } else if (strcmp(curr->suit, "spades") == 0){
            printf("[%d \u2660] -> ", curr->value);
        } else if (strcmp(curr->suit, "clubs") == 0){
            printf("[%d \u2663] -> ", curr->value);
        }
        curr = curr->next;
    }
    printf("NULL\n");
    
    free(curr);
}


/************************************************************************
 * print_hand(): Function that uses a series of while loops to print    *
 *      the passed-in players hand with formatted graphics and unicode  *
 *      symbols to represent the suits.                                 *
 ************************************************************************/
void print_hand(card *hl) {
    
    card *temp = (card*)malloc(sizeof(card));
    // serves as a flag to maintain pointer to the correct set of cards
    card *current_start = (card*)malloc(sizeof(card));
    int length = find_length(hl);
    int running_length = length;
    
    temp = hl;
    current_start = hl;
    
    int i;
    while (running_length > 0) {
        i = 0;
        
        while (i < CARD_LIMIT && i < running_length) {
            printf(" -----  ");
            i++;
        }
        printf("\n");
        
        i = 0;
        while (i < CARD_LIMIT && i < running_length) {
            print_leftside_card(temp);
            temp = temp->next;
            i++;
        }
        printf("\n");
        temp = current_start;
        i = 0;
        while (i < CARD_LIMIT && i < running_length) {
            printf("|     | ");
            i++;
        }
        printf("\n");
        
        i = 0;
        while (i < CARD_LIMIT && i < running_length) {
            print_rightside_card(temp);
            temp = temp->next;
            i++;
        }
        printf("\n");
        
        i = 0;
        while (i < CARD_LIMIT && i < running_length) {
            printf(" -----  ");
            i++;
        }
        printf("\n");
        
        running_length = running_length - CARD_LIMIT;
        current_start = temp;
        
    }
    
    free(temp);
    free(current_start);
    
}


/************************************************************************
 * print_leftside_card(): Function that is specifically tailored to     *
 *      correctly print the top left side of the card with rank & suit  *
 ************************************************************************/
void print_leftside_card(card *card) {
    
    // 10 is the special case since it takes up two spaces
    if (card->value == 10) {
        printf("|10");
        if (strcmp(card->suit, "hearts") == 0) {
            printf("\u2665  | ");
        } else if (strcmp(card->suit, "diamonds") == 0) {
            printf("\u2666  | ");
        } else if (strcmp(card->suit, "spades") == 0) {
            printf("\u2660  | ");
        } else {
            // Clubs
            printf("\u2663  | ");
        }
    } else {
        printf("|%c", convert_rank(card->value));
        if (strcmp(card->suit, "hearts") == 0) {
            printf("\u2665   | ");
        } else if (strcmp(card->suit, "diamonds") == 0) {
            printf("\u2666   | ");
        } else if (strcmp(card->suit, "spades") == 0) {
            printf("\u2660   | ");
        } else {
            // Clubs
            printf("\u2663   | ");
        }
    }
    
    
    
}


/************************************************************************
 * print_rightside_card(): Function that is specifically tailored to    *
 *      correctly print the bottom right of the card with rank & suit   *
 ************************************************************************/
void print_rightside_card(card *card) {
    
    // |  10\u2665|
    
    // 10 is the special case since it takes up two spaces
    if (card->value == 10) {
        printf("|  10");
    } else {
        printf("|   %c", convert_rank(card->value));
    }
    
    if (strcmp(card->suit, "hearts") == 0) {
        printf("\u2665| ");
    } else if (strcmp(card->suit, "diamonds") == 0) {
        printf("\u2666| ");
    } else if (strcmp(card->suit, "spades") == 0) {
        printf("\u2660| ");
    } else {
        // Clubs
        printf("\u2663| ");
    }
    
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
    char filename[FILENAME_SIZE];
    // Load deck from preformatted file
    FILE *inp;
    
    // Request the filename from the user
    printf("Enter the filename you wish to read from: ");
    scanf("%s", filename);
    
    // Attempt to open and read given file
    inp = fopen(filename, "r");
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
    
    free(curr);
    
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
 * guess_a_card(): Function that is responsible the guessing mechanics  *
 *      and will prompt user for guess, validate the response, and      *
 *      call all necessary functions to correctly process that guess    *
 *      and returns the int value of the Player (1 or 2) that will      *
 *      continue with the next turn.                                    *
 ************************************************************************/
int guess_a_card(int players_turn, int **p1_score_ptr, card **player1_hl, card **player1_hr, card **player2_hl, card **player2_hr, card **deck_hl, card **deck_hr) {
    
    char guess[GUESS_SIZE];
    int guess_rank;
    
    printf("Player %d, Make a Guess (please enter A, 2-10, J, Q, or K): \n", players_turn);
    printf("Guess: ");
    scanf("%s", guess);
    
    while (validate_guess(guess) != 1) {
        printf("That is not a valid guess. Try Again\n");
        printf("Player %d, Make a Guess (please enter A, 2-10, J, Q, or K): \n", players_turn);
        printf("Guess: ");
        scanf("%s", guess);
    }
    
    guess_rank = convert_guess(guess);
    
    if (process_guess(players_turn, guess_rank, p1_score_ptr, player1_hl, player1_hr, player2_hl, player2_hr, deck_hl, deck_hr)) {
        // Card was found and moved, maintain turn
        if (players_turn == PLAYER_ONE) {
            return PLAYER_ONE;
        } else {
            return PLAYER_TWO;
        }
    } else {
        // Card was not found, switch turns
        if (players_turn == PLAYER_ONE) {
            return PLAYER_TWO;
        } else {
            return PLAYER_ONE;
        }
    }
    
}


/************************************************************************
 * validate_guess(): Function that will return 0 or 1 depending on if   *
 *      the entered guess is a valid one or not.                        *
 ************************************************************************/
int validate_guess(char *guess) {
    
    if (strcmp(guess, "A") == 0 || strcmp(guess, "2") == 0 || strcmp(guess, "3") == 0 || strcmp(guess, "4") == 0 || strcmp(guess, "5") == 0 || strcmp(guess, "6") == 0 || strcmp(guess, "7") == 0 || strcmp(guess, "8") == 0 || strcmp(guess, "9") == 0 || strcmp(guess, "10") == 0 || strcmp(guess, "J") == 0 || strcmp(guess, "Q") == 0 || strcmp(guess, "K") == 0) {
        // If any of the above is true, then a valid guess has been entered, return 1
        return 1;
    }
    return 0; // Invalid guess given, return 0
    
}

int validate_possession(int guess_rank, card *guesser_hl) {
    
    card *temp = (card*)malloc(sizeof(card));
    
    // Check to see if the user guessed a rank that they posses
    temp = guesser_hl;
    while (temp != NULL) {
        if (temp->value == guess_rank) {
            // Player possess card, proceed to process guess
            return 1;
        }
        temp = temp->next;
    }
    
    return 0;
    
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
 * convert_rank(): Function that converts the integer rank to its char  *
 *      equivalent (based on specifications) in order to print correctly*
 *      when printing the card graphics.                                *
 ************************************************************************/
char convert_rank(int rank) {
    switch(rank) {
        case 1: return 'A';
        case 11: return 'J';
        case 12: return 'Q';
        case 13: return 'K';
        default: return (rank + '0'); // Returns the character value of any integer
                                      // I.e: 8 is returned as '8'...
    }
}


/************************************************************************
 * check_if_playable(): Function that checks to see if the game is still*
 *      playable. I.e. the highly unlikely case where the winning score *
 *      has not been reached, the pool no longer has cards, and one of  *
 *      the players has an empty hand (whom cannot draw any cards from  *
 *      an empty pool. Will return 1 if dead game, 0 otherwise.         *
 ************************************************************************/
int check_if_playable(card *p1_hl, card *p2_hl, card *deck_hl) {
    if (((find_length(p1_hl) == 0) || (find_length(p2_hl) == 0)) && (find_length(deck_hl) == 0)) {
        // One of the hands is empty AND deck is empty, then game is no longer playable. Return 1
        return 1;
    } else {
        // No winner found, return 0
        return 0;
    }
}


/************************************************************************
 * check_for_winner(): Function that will be called at each iteration   *
 *      of a turn to see if either of the players has acheived the      *
 *      winning score of 7. When that occurs, game will declare a       *
 *      winner and display an ending message.                           *
 ************************************************************************/
int check_for_winner(int *p1_score, int *p2_score) {
    
    if (*p1_score >= 7) {
        return PLAYER_ONE;
    } else if (*p2_score >= 7) {
        return PLAYER_TWO;
    }
    return 0;
    
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
int process_guess(int guesser, int guess_rank, int **player_score, card **guesser_hl, card **guesser_hr, card **opp_hl, card **opp_hr, card **deck_hl, card **deck_hr) {
   
    // Iterate through the list and see if rank exists in hand, if so, how many.
    int num_of_cards = 0;
    int book_value;
    
    card *temp = (card*)malloc(sizeof(card));
    char guess[GUESS_SIZE];
    
    while (validate_possession(guess_rank, *guesser_hl) != 1) {
        printf("Oops! You do not possess that card! Try again!\n");
        printf("Player %d, Make a Guess (please enter A, 2-10, J, Q, or K): \n", guesser);
        printf("Guess: ");
        scanf("%s", guess);
        guess_rank = convert_guess(guess);
    }
    
    temp = *opp_hl;
    while (temp != NULL) {
        if (temp->value == guess_rank) {
            num_of_cards++;
        }
        temp = temp->next;
    }
    
    // Now we check to see if any cards exist
    if (num_of_cards > 0) {
        // There is a card that needs to be transfered from opponenets deck to guessers deck
        
        if (guesser == PLAYER_ONE) {
            // transfer from Player 2 to Player 1
            if (guess_rank == 10) {
                printf("\n*************************************************************\n");
                printf("*\n");
                printf("* CARD FOUND! Transferring all 10's from Player 2 to Player 1\n");
                printf("*\n");
                printf("*************************************************************\n");
            } else {
                printf("\n*************************************************************\n");
                printf("*\n");
                printf("* CARD FOUND! Transferring all %c's from Player 2 to Player 1\n", convert_rank(guess_rank));
                printf("*\n");
                printf("*************************************************************\n");
            }
        } else {
            if (guess_rank == 10) {
                printf("\n*************************************************************\n");
                printf("*\n");
                printf("* CARD FOUND! Transferring all 10's from Player 1 to Player 2\n");
                printf("*\n");
                printf("*************************************************************\n");
            } else {
                printf("\n*************************************************************\n");
                printf("*\n");
                printf("* CARD FOUND! Transferring all %c's from Player 1 to Player 2\n", convert_rank(guess_rank));
                printf("*\n");
                printf("*************************************************************\n");
            }
        }
        
        transfer_cards(num_of_cards, guess_rank, guesser_hl, guesser_hr, opp_hl, opp_hr);
        book_value = check_for_book(*guesser_hl);
        if (book_value != 0) {
            remove_book(book_value, guesser_hl, guesser_hr);
            (**player_score)++;
            printf("\n*************************************************************\n");
            printf("*\n");
            printf("* NICE JOB COMPLETING A BOOK! PLAYER %d's NEW SCORE IS: %d  \n", guesser, **player_score);
            printf("*\n");
            printf("*************************************************************\n");
        }
        return 1;
    } else {
        // Card was not found, therefore, GOFISH occurs
        print_go_fish();
        go_fish(guesser_hl, guesser_hr, deck_hl, deck_hr);
        book_value = check_for_book(*guesser_hl);
        if (book_value != 0) {
            remove_book(book_value, guesser_hl, guesser_hr);
            (**player_score)++;
            printf("\n*************************************************************\n");
            printf("*\n");
            printf("* NICE JOB COMPLETING A BOOK! PLAYER %d's NEW SCORE IS: %d  \n", guesser, **player_score);
            printf("*\n");
            printf("*************************************************************\n");
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


/************************************************************************
 * declare_winner(): Function that will analyze the players scores and  *
 *      declare a winner if one exists or a tie of one occurrs.         *
 ************************************************************************/
void declare_winner(int p1_score, int p2_score) {
    
    printf("\n\nGAME OVER! LETS TALLY UP THE SCORES\n\n");
    printf("Player 1: %d\n", p1_score);
    printf("Player 2: %d\n", p2_score);
    
    if (p1_score > p2_score) {
        // Player 1 wins
        printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n");
        printf("><((('>  CONGRATULATIONS TO PLAYER 1, YOU ARE THE WINNER!!!!      ><((('>\n");
        printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n\n");
    } else if (p1_score < p2_score) {
        // Player 2 wins
        printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n");
        printf("><((('>  CONGRATULATIONS TO PLAYER 2, YOU ARE THE WINNER!!!!      ><((('>\n");
        printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n\n");
    } else {
        // Tie Occurred
        printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n");
        printf("><((('>  A TIE HAS OCCURRED! BETTER LUCK NEXT TIME!               ><((('>\n");
        printf("><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('> ~~~ ><(((('>\n\n");
    }
    
}
