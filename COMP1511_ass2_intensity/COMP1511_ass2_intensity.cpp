// intensity.c
// Assignment 2, COMP1511 18s1: Intensity
//
// This program by JOHN TIAN z5207980 on 19/04/2018
//
// Version 1.0.1: Minor changes to wording of comments.
// Version 1.0.0: Assignment released.

#include "stdafx.h"
#include "assert.h"
#include "stdlib.h"

// SOME USEFUL #defines - YOU WILL NEED MORE

#define ACTION_PLAYER_NAME    0
#define ACTION_DISCARD        1
#define ACTION_PLAY_CARD      2
#define ACTION_UNIT_TESTS     3

#define N_CARDS              40
#define N_CARDS_INITIAL_HAND 10
#define N_PLAYERS             4
#define N_CARDS_DISCARDED     3

#define CARD_MIN             10
#define CARD_MAX             49


// ADD EXTRA #defines HERE

#define SUITS				 4
#define TRUE				 1
#define FALSE				 0

//Strategies defined here
#define HAS_BUFFALO			 0
#define BRING_DOWN_BUFFALO	 1
#define SAFE				 2


void print_player_name(void);
void choose_discards(void);
void choose_card_to_play(void);
void run_unit_tests(void);


// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE
void initialiseArray(int *arr, int length);
void read_discard_input(int *cards, int *suitCount);
int find_discard_suit(int *suitCount);
void discard_card(int *cards, int *suitCount, int discardCount, int discardSuit);
int find_legal_plays(int *legalCards, int *cardsThisRound, int *cardsThisGame,
	int *handArray, int handSize, int numCardsThisRound, int numCardsThisGame);
int read_game_int_info();
int *read_game_ptr_info(int* playerDiscard, int* opponentDiscard, int handSize,
	int numCardsThisRound, int numCardsThisGame);
void freeMemAlloc(int *legalCards, int *handArray, int *cardsThisRound,
	int *cardsThisGame);
// You should not need to change this main function

int main(void) {

	int which_action = 0;
	scanf_s("%d", &which_action);
	if (which_action == ACTION_PLAYER_NAME) {
		print_player_name();
	}
	else if (which_action == ACTION_DISCARD) {
		choose_discards();
	}
	else if (which_action == ACTION_PLAY_CARD) {
		choose_card_to_play();
	}
	else {
		run_unit_tests();
	}

	return 0;
}

void print_player_name(void) {
	// CHANGE THIS PRINTF TO YOUR DESIRED PLAYER NAME
	printf("Frostbyte\n");

}

void choose_discards() {
	// ADD CODE TO READ THE CARDS OF YOUR HAND INTO AN ARRAY USING SCANF
	// THEN ADD YOUR CODE HERE TO CHOOSE AND PRINT THE CARDS YOU WISH TO DISCARD

	// NOTE: THE PROVIDED CODE DOES NOT MAKE A LEGAL MOVE. YOU MUST CHANGE IT TO
	// DISCARD CARDS FROM YOUR HAND.

	//There is no point using malloc() here as the size of the discard hand is known.

	int cards[N_CARDS_INITIAL_HAND];
	initialiseArray(cards, N_CARDS_INITIAL_HAND);

	int suitCount[SUITS];
	initialiseArray(suitCount, SUITS);

	int discardCount = 1;
	
	read_discard_input(cards, suitCount);
	int discardSuit = find_discard_suit(suitCount);

	discard_card(cards, suitCount, discardCount, discardSuit);
	discardCount += 1;
	discard_card(cards, suitCount, discardCount, discardSuit);
	discardCount += 1;
	discard_card(cards, suitCount, discardCount, discardSuit);

	printf("\n");
}

void choose_card_to_play(void) {
	// ADD CODE TO READ THE CARDS OF YOUR HAND INTO AN ARRAY USING SCANF
	// ADD CODE TO READ THE CARDS PLAYED THIS ROUND INTO AN ARRAY USING SCANF
	// ADD CODE TO READ THE CARDS PREVIOUSLY PLAYED THIS ROUND INTO AN ARRAY USING SCANF
	// THEN ADD YOUR CODE HERE TO CHOOSE AND PRINT THE CARD YOU WISH TO PLAY

	// Strategy for playing Intensity
	// Discard to try and empty a suit, so calves and buffalo cards can be "discarded"
	// Use if statements to determine special scenarios
	// otherwise, use a basic algorithm for these cases
	//	-if there's a high card, try slip under a high card that's been played
	//	-no calves or buffalos in play

	int playerDiscard[3],
		opponentDiscard[3],
		discard,
		*legalCards;

	int handSize,
		numCardsThisRound,
		position,
		numCardsThisGame = read_game_int_info();

	int *handArray, 
		*cardsThisRound, 
		*cardsThisGame = read_game_ptr_info(playerDiscard, 
			opponentDiscard, 
			handSize, 
			numCardsThisRound, 
			numCardsThisGame);

	discard = find_legal_plays(legalCards,
		cardsThisRound,
		cardsThisGame,
		handArray,
		handSize,
		numCardsThisRound,
		numCardsThisGame);
	
	freeMemAlloc(legalCards, handArray, cardsThisRound, cardsThisGame);
}


// ADD A COMMENT HERE EXPLAINING YOUR OVERALL TESTING STRATEGY

void run_unit_tests(void) {
	// PUT YOUR UNIT TESTS HERE
}


// ADD YOUR FUNCTIONS HERE

void initialiseArray(int *arr, int length) {
	int i;
	for (i = 0; i < length; i += 1) {
		arr[i] = '\0';
	}
}

void read_discard_input(int* cards, int* suitCount) {
	int i, num1, num2;
	getchar(); //gets the newline
	for (i = 0; i < N_CARDS_INITIAL_HAND; i += 1) {
		num1 = getchar() - '0';
		suitCount[num1 - 1] += 1;
		num1 = num1 * 10;
		num2 = getchar() - '0';
		cards[i] = num1 + num2;
		getchar();
	}
}

int find_discard_suit(int* suitCount) {
	int i;
	for (i = 0; i < SUITS; i += 1) {
		if (suitCount[i]  <= 2) {
			return i + 1;
		}
	}
	return -1; //no easy suit discard avaliable
}

void discard_card(int* cards, 
	int* suitCount, 
	int discardCount, 
	int discardSuit) {
	int i,
		buffaloSuit = 0,
		buffalo = FALSE,
		discard = 0;


	// Count buffalo suit and if buffalo is true for later discard strat.

	for (i = 0; i < N_CARDS_INITIAL_HAND; i += 1) {
		if (cards[i] % 10 == 4) {
			buffaloSuit += 1;
		}
		if (cards[i] == 47) {
			buffalo = TRUE;
		}
	}

	//Implement priority discards first. These cards are probably the worst to have
	//with one of the strategies I'm attempting to add (push the buffalo out of the
	// game without risk of it falling to the player).
	// However, the player will keep buffalo if he can maintain and discard it.

	//May attempt an implementation where the player keeps buffalo and calves so he can
	//push other players out of the game and control when the penalty points go out.

	//This process is also used as a failsafe to store the highest card in a suit.
	//If somehow the discard process fails, discard will store teh highest card

	for (i = 0; i < N_CARDS_INITIAL_HAND; i += 1) {

		if (cards[i] % 10 > i) {
			discard = cards[i];
		}

		if (buffaloSuit < 3 || buffalo == FALSE) {

			if (cards[i] == 47) {
				printf("%d ", 47);
				cards[i] = 0;
				return;
			}
			else if (cards[i] == 48) {
				printf("%d ", 48);
				cards[i] = 0;
				return;
			}
			else if (cards[i] == 49) {
				printf("%d ", 49);
				cards[i] = 0;
				return;
			}
		}
	}

	//Program now chooses a strategy to obtain an optimal starting hand.
	//Determine if there's an overload of a suit in the hand or
	// if the hand is balanced around all suits. The reason is because I am
	// planning to build this algorithm through discarding the buffalo and high-
	// end cards as much as I can. This results in a safe end-game where the player
	// will not be leading the game and unlikely to obtain points.
	//Suits are defined as the ten's digit (ie, 10 and 11 are the same suit,
	// 20 and 30 are different suits).
	//Discard currently stores the highest number of a random suit, so if
	// the algorithm messes up, the failsafe is still a reasonable discard.

	if (discardSuit != -1){
		for (i = 0; i < N_CARDS_INITIAL_HAND && discardCount <= 2; i += 1) {
			if (cards[i] / 10 == discardSuit) {
				printf("%d ", cards[i]);
				cards[i] = 0;
				return;
			}
		}
	}
	
	//Program will now attempt to discard high-cost calves
	//High-cost will be defined as > 36
	//The reason is because at the end of a round, playing calves is mandatory
	//Since calves are worth points, and since it's unlikely for them to be
	// completely given to other players, losing an endgame will likely result
	// in multiples calves being given to the player. Although this risk can be
	// mitigated in play, not having a high-cost calf will make it much easier.

	else if (discardSuit == -1) {
		for (i = 0; i < N_CARDS_INITIAL_HAND; i += 1) {
			if (cards[i] / 10 == 3 && cards[i] % 10 > 6) {
				printf("%d ", cards[i]);
				cards[i] = 0;
				return;
			}
		}
	}

	//By now, if no cards have been discarded, the hand should be relatively decent.
	//The highest cards can now be discarded. Discard should already store the highest card.
	

	for (i = 0; i < N_CARDS_INITIAL_HAND; i += 1) {
		if (cards[i] == discard) {
			printf("%d ", cards[i]);
			cards[i] = 0;
			return;
		}
	}

}

int read_game_int_info() {

	int handSize,
		numCardsThisRound,
		position,
		numCardsThisGame;

	//I honestly can't think of an application where knowing what another player
	//has discarded will come in handy. Even as a human player I can't infer any
	//good info, as their discards are a reflection of the strategy they are 
	//attempting to implement. However, since one doesn't know what cards the 
	//discarding player received, it's impossible to know how their strategy
	//might change.

	//Since the input will be given by a computer and not a user, won't be testing
	//for error in input.
	scanf("%d %d %d", &handSize, &numCardsThisRound, &position);

	//A calculation to find the amount of cards played this game.
	numCardsThisGame = (N_CARDS_INITIAL_HAND - handSize) * N_PLAYERS;

	return handSize, numCardsThisRound, position, numCardsThisGame;
}

int *read_game_ptr_info(int* playerDiscard, 
	int* opponentDiscard, 
	int handSize, 
	int numCardsThisRound, 
	int numCardsThisGame) {

	int i,
		*handArray,
		*cardsThisRound,
		*cardsThisGame;

	//Memory allocation for player's hand, cards on the round and cards
	//played this game.
	*handArray = (int) malloc(handSize * sizeof(int));
	*cardsThisRound = (int) malloc(numCardsThisRound * sizeof(int));
	*cardsThisGame = (int)malloc( numCardsThisGame * sizeof(int));

	//Reading player's hand
	for (i = 0; i < handSize; i += 1) {
		scanf("%d", &handArray[i]);
	}

	//Reading cards played this round
	for (i = 0; i < numCardsThisRound; i += 1) {
		scanf("%d", &cardsThisRound[i]);
	}

	//Reading cards played this game (players * rounds)
	for (i = 0; i < numCardsThisGame; i += 1) {
		scanf("%d", &cardsThisGame[i]);
	}

	//Reading what the player formerly discarded, and what's in the
	//hand of the player on the left.
	for (i = 0; i < N_CARDS_DISCARDED; i += 1) {
		scanf("%d", &playerDiscard[i]);
	}

	//Reading what the player on the right discarded.
	for (i = 0; i < N_CARDS_DISCARDED; i += 1) {
		scanf("%d", &opponentDiscard[i]);
	}

	return handArray, cardsThisRound, cardsThisGame;
}

int find_legal_plays(
	int *legalCards,
	int *cardsThisRound, 
	int *cardsThisGame, 
	int *handArray,
	int handSize, 
	int numCardsThisRound, 
	int numCardsThisGame) {

	int i,
		legalSuit,
		legalCount = 0,
		isCalfLegal = FALSE;

	//Case where the player is starting the round

	if (numCardsThisRound == 0) {

		//Find if it's legal to start play with calf

		//Case where the player has ONLY calves in hand
		if (handArray[0] % 10 == 3 && handArray[handSize - 1] % 10 == 3) {
			isCalfLegal = TRUE;
		}

		//Case where calf played earlier in the game
		for (i = 0; i < numCardsThisGame; i += 1) {
			if (cardsThisGame[i] % 10 == 3) {
				isCalfLegal = TRUE;
			}
		}

		//Filing up the legalCards array

		//Case where calves are legal, as determined before
		if (isCalfLegal == TRUE) {
			*legalCards = (int)malloc(handSize * sizeof(int));
			for (i = 0; i < handSize; i += 1) {
				legalCards[i] = handArray[i];
			}
			return FALSE;
		}

		//Case where calves are illegal
		//2 for loops because the amount of legal cards need
		//to be found first so a size can be malloc'ed
		else if (isCalfLegal == FALSE) {
			for (i = 0; i < handSize; i += 1) {
				if (handArray[i] % 10 != 3) {
					legalCount += 1;
				}
			}

			*legalCards = (int)malloc(legalCount * sizeof(int));
			legalCount = 0;

			for (i = 0; i < handSize; i += 1) {
				if (handArray[i] % 10 != 3) {
					legalCards[legalCount] = handArray[i];
					legalCount += 1;
				}
			}
			return FALSE;
		}
	}

	//Variable can be safely assigned as the function would have returned
	//earlier if numCardsThisRound = 0

	legalSuit = cardsThisRound[0];

	//Case where the player is not starting the round

	if (numCardsThisRound != 0) {
		for (i = 0; i < handSize; i += 1) {
			if (handArray[i] % 10 == legalSuit) {
				legalCount += 1;
			}
		}

		//Check if player has suit in hand

		//No suit in hand, all cards are valid to play
		if (legalCount == 0) {

			*legalCards = (int)malloc(handSize * sizeof(int));

			for (i = 0; i < handSize; i += 1) {
				legalCards[i] = handArray[i];
			}
			return TRUE;
		}

		//Player has suit in hand, only that suit is added to *legalCards
		else {
			*legalCards = (int)malloc(legalCount * sizeof(int));
			legalCount = 0;

			for (i = 0; i < handSize; i += 1) {
				if (handArray[i] % 10 != 3) {
					legalCards[legalCount] = handArray[i];
					legalCount += 1;
				}
			}
			return FALSE;
		}

	}
}

//int chooseStrategy(int *cardsThisRound, 
//	int *cardsThisGame, 
//	int *legalPlays, 
//	int numCardsThisRound) {
//
//}

void freeMemAlloc(int *legalCards, int *handArray, int *cardsThisRound,
	int *cardsThisGame) {
	int free(*legalCards);
	int free(*handArray);
	int free(*cardsThisRound);
	int free(*cardsThisGame);
}