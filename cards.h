/*
 * cards.h - class definitions for player card objects.
 *
 *  Created on: Jul 20, 2014
 *  	Author: Ryan Walker
 *      Email: r_walker@zoho.com
 */

#ifndef CARDS_H_
#define CARDS_H_

#include <vector>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <time.h>

// Suits class
class Suit{
	public:
		std::string getSuit();
		void setSuit(std::string);
	private:
		std::string suit;
};

std::string Suit::getSuit(){
	return Suit::suit;
}

void Suit::setSuit(std::string s){
	Suit::suit=s;
}

// Face values class
class Face{
	public:
		std::string getFace();
		void setFace(std::string);
	private:
		std::string face;
};

std::string Face::getFace(){
	return face;
}

void Face::setFace(std::string f){
	Face::face=f;
}

// Card object, holds suit and value of card
class Card{
	public:
		Card(std::string, std::string);
		std::string getStringValue();
		Suit suit;
		Face face;
};

// constructor
Card::Card(std::string s, std::string f){
	Card::suit.setSuit(s);
	Card::face.setFace(f);
}

// string value of card
std::string Card::getStringValue(){
	std::string value = suit.getSuit() + face.getFace();
	return(value);
};

// Pack holds standard 52 card deck.
class Pack{
	public:
		Pack();
		Card Draw();
		void Shuffle();

	private:
		// card data specification
		std::string const face_t[13] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
		std::string const suit_t[4] = {"H", "D", "C", "S"};
		std::vector<Card> cards;
};

Pack::Pack(){
	std::srand(time(NULL));
}


// draw a card from the pack
Card Pack::Draw(){
	Card draw = cards.back();
	cards.pop_back();
	return draw;
}

// Shuffle (using methods from STL)
void Pack::Shuffle(){
	// empty deck
	cards.clear();

	// populate deck
	for (int i=0; i<4; i++){
		for (int j=0; j<13; j++){
			Pack::cards.push_back(Card(suit_t[i], face_t[j]));
		}
	}

	// shuffle
	std::random_shuffle(Pack::cards.begin(), Pack::cards.end());
}

#endif
