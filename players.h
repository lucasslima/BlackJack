/*
 * players.h
 *
 *  Created on: Jul 16, 2014
 *      Author: ryan
 */

#ifndef PLAYERS_H_
#define PLAYERS_H_
#include <vector>
#include "cards.h"

class Player{
	public:

		// hand
		std::vector<Card> Hand;

		// chips and wager management
		int getChipCount();
		int getWager();
		void loseChips(int);
		void gainChips(int);
		void setChips(int);
		void setWager(int);

	protected:
		int chips;
		int wager;
};

void Player::setChips(int nchips){
	chips = nchips;
}

// chip and wager methods
int Player::getChipCount(){
	return(chips);
}

void Player::loseChips(int nchips){
	Player::chips-=nchips;
}

void Player::gainChips(int nchips){
	Player::chips+=nchips;
}

int Player::getWager(){
	return Player::wager;
}

void Player::setWager(int w){
	Player::wager = w;
}

// Dealer type
class Dealer {
	public:
		void Shuffle();
		Card giveCard();
		Pack deck;
};

Card Dealer::giveCard(){
	Card draw = Dealer::deck.Draw();
	return draw;
}

void Dealer::Shuffle(){
	deck.Shuffle();
}


#endif
