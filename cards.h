/*
 * 	Class definition file for playing cards.
 */

#ifndef CARDS_H_
#define CARDS_H_

#include <vector>
#include <algorithm>
#include <string>
using namespace std;

// Suits class
class Suit{
	public:
		string getSuit();
		void setSuit(string);
	private:
		string suit;
};

string Suit::getSuit(){
	return Suit::suit;
}

void Suit::setSuit(string s){
	Suit::suit=s;
}


// Face values class
class Face{
	public:
		string getFace();
		void setFace(string);
	private:
		string face;
};

string Face::getFace(){
	return face;
}

void Face::setFace(string f){
	Face::face=f;
}

// Card object, holds suit and value of card
class Card{
	public:
		Card(string, string);
		string getStringValue();
		Suit suit;
		Face face;
};

// constructor
Card::Card(string s, string f){
	Card::suit.setSuit(s);
	Card::face.setFace(f);
}

// string value of card
string Card::getStringValue(){
	string value = suit.getSuit() + face.getFace();
	return(value);
};

// Pack holds standard 52 card deck.
class Pack{
	public:
		Card Draw();
		void Shuffle();

	private:
		// card data specification
		string const face_t[13] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
		string const suit_t[4] = {"H", "D", "C", "S"};
		std::vector<Card> cards;
};

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
