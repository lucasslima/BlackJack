//============================================================================
// Name        : BlackJack.cpp
// Author      : Ryan Walker
// Email       : r_walker@zoho.com
//============================================================================
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "BlackJack.h"
using namespace std;

// function templates for console input
int inputWager(int);
char inputAction();

// function templates for game play
void deal(BlackJackDealer&, BlackJackPlayer&);
void hit(BlackJackPlayer&, BlackJackDealer&);
int handValue(BlackJackPlayer&);
void scoreRound(BlackJackPlayer&, BlackJackDealer&);
void autoplay(BlackJackDealer&);

// Define face values of cards based on standard black jack rules using
// a static map. Note, we don't code a value for "A" because
// it can be either 2 or 11 in Blackjack.
std::map<std::string,int> create_map()
{
	std::map<std::string, int> cardvalues;
	cardvalues["2"] = 2;
	cardvalues["3"] = 3;
	cardvalues["4"] = 4;
	cardvalues["5"] = 5;
	cardvalues["6"] = 6;
	cardvalues["7"] = 7;
	cardvalues["8"] = 8;
	cardvalues["9"] = 9;
	cardvalues["10"] = 10;
	cardvalues["J"] = 10;
	cardvalues["Q"] = 10;
	cardvalues["K"] = 10;
  return cardvalues;
}
std::map<std::string,int> cardvalues = create_map();

// main
int main() {
	// Initialize game
	BlackJackDealer dealer = BlackJackDealer();
	BlackJackPlayer player = BlackJackPlayer();

	// Player begins with 100 chips
	player.setChips(100);

	// control flow variables
	char continue_play = 'Y';
	int round = 1;

	// Game play
	cout << "Welcome to Black Jack!" << endl;
	while(player.getChipCount()>0 and continue_play=='Y'){
		cout << "Round #" << round << ": "<< endl;
		cout << "Place your wager (Blackjack pays 2:1 and all other wins pay 1:1). " << endl;

		// Get and process player's wager from console
		int num = inputWager(player.getChipCount());
		player.setWager(num);
		cout << "########################################" << endl;
		// Deal game
		deal(dealer, player);

		// Get player's requested action from console
		char action;
		int turn =1;
		while(true){
			cout << "Turn #" << turn << endl;
			cout << "Dealer's hand: "<< dealer.printPublicHand() << endl;
			cout << "Player's hand: "<< player.printPrivateHand() <<"\t\tPlayer score: "
					<< handValue(player) << endl <<endl;
			action = inputAction();
			if (action == 'H')
				hit(player, dealer);
			if (action == 'S' or handValue(player)>21)
				break;
			turn++;
		}

		// Dealer's turn
		if(handValue(player)<=21)
			autoplay(dealer);

		// Score round
		scoreRound(player, dealer);

		// Round complete.
		if (player.getChipCount()>0){
			cout << "You now have " << player.getChipCount() << " chips. ";
			cout << "If you'd like to play again, type 'Y' or any other character to quit." << endl;
			cin >> continue_play;
			continue_play = char(toupper(continue_play));
			round +=1;
		}
		else {
			cout << "You don't have any chips left to wager. " << endl;
			continue_play = 'N';
		}
	}
	cout << "Game over.  Goodbye." << endl;
	return 0;
}

/*
 * Input processing functions
 */

int inputWager(int nchips){
	int num;
	while(true){
		string input;
		cout << "Enter a bet between 1 and " << nchips << ": " << endl;

		// get input from console and validate
		cin>> input;
		stringstream s(input);
		   if (s >> num and num>=1 and num<=nchips)
		     break;
		   else
			   cout << input << endl;
			   cout << "Invalid input. Please try again" << endl;
	}
	return num;
}

// collect requested action from console
char inputAction(){
	char action;
	while(true){
		cout << "Enter an action (H = 'Hit' , S = 'Stop'): ";

		// get input from console and validate
		cin >> action;
		action = char(toupper(action));
		if (action=='H' or action=='S')
			break;
		else
			cout << "Invalid input. Please try again." << endl;
	}
	return action;
}


/*
 * Game play functions
 */

void deal(BlackJackDealer& dealer, BlackJackPlayer& player){
	// clean up from old games
	dealer.Shuffle();
	dealer.Hand.clear();
	player.Hand.clear();

	// Deal cards
	player.Hand.push_back(dealer.giveCard());
	dealer.Hand.push_back(dealer.giveCard());
	player.Hand.push_back(dealer.giveCard());
	dealer.Hand.push_back(dealer.giveCard());

}

// dealer gives hit to player
void hit(BlackJackPlayer& player, BlackJackDealer& dealer){
	player.Hand.push_back(dealer.giveCard());
}

// calculate the current value of a players hand
int handValue(BlackJackPlayer& player){
	int min_score = 0;
	int max_score = 0;

	std::string suit;
	std::string face;

	for(int i=0; i<int(player.Hand.size()); i++){
		face = (player.Hand.at(i)).face.getFace();
		suit = (player.Hand.at(i)).suit.getSuit();

		if (face == "A"){
			min_score +=2;
			max_score +=11;
		}
		else{
			min_score += cardvalues[face];
			max_score += cardvalues[face];
		}
	}

	if(max_score<=21)
		return max_score;
	else
		return min_score;
}

// decide who wins and award chips.
void scoreRound(BlackJackPlayer& player, BlackJackDealer& dealer){
	int pscore = handValue(player);
	int dscore = handValue(dealer);
	int pcards = int(player.Hand.size());
	int dcards = int(dealer.Hand.size());
	int wager = player.getWager();

	cout << "########################################" << endl;
	cout << "Dealer final score: " << handValue(dealer)
			<<"\t\t Hand: "<< dealer.printPrivateHand() << endl;
	cout << "Player final score: " << handValue(player)
			<<"\t\t Hand: "<< player.printPrivateHand() << endl;

	if (pscore==21){
		if (pcards==2){
			if(dscore==21 and dcards==2){
				cout << "Dealer and player both have blackjack! Tie." << endl;
			}
			else {
				cout << "Player has blackjack! Player wins "<< 2*wager << " dollars!" << endl;
				player.gainChips(2*wager);
			}
		}
		else if (dscore==21 and dcards==2){
				cout << "Dealer has blackjack.  Player loses" << endl;
				player.loseChips(wager);
			}
		else {
			cout << "Player wins "<< wager << " dollars!"<< endl;
			player.gainChips(wager);
		}
	}
	else if (pscore>21){
		std::cout << "Player busts with score " << pscore <<". Lose "<< wager <<" dollars." <<endl;
		player.loseChips(wager);
	}
	else if (dscore>21){
		std::cout << "Dealer busts and player wins with score " << pscore << ". Win: "<< wager
				<< " dollars." << endl;
		player.gainChips(wager);
	}
	else if (pscore<dscore){
		std::cout << "Player loses with score " << pscore <<". Lose "<< wager <<" dollars." <<endl;
		player.loseChips(wager);
	}
	else if (pscore>dscore){
		std::cout << "Player wins with score " << pscore <<". Win "<< wager <<" dollars." <<endl;
		player.gainChips(wager);
	}
	else if (pscore == dscore)
		std::cout << "Player and dealer tie with " << pscore << "." <<endl;
	cout << "########################################" << endl;
}

// simple automatic play for dealer--dealer plays until hand
// value is great than or equal to 17.
void autoplay(BlackJackDealer& dealer){
	while(true){
		if (handValue(dealer)<=17){
			hit(dealer, dealer);
		}
		else
			break;
	}
}



