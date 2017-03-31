/*
 * BlackJack.h - Black jack game implementation.
 *
 *  Created on: Jul 20, 2014
 *  	Author: Ryan Walker
 *      Email: r_walker@zoho.com
 */

#ifndef BLACKJACK_H_
#define BLACKJACK_H_

#include <string>
#include <sstream>
#include "players.h"
#include <boost/asio.hpp>

using namespace std;
using namespace boost;

const string DELIMITER = "\n";

enum ResponseCode{
	NOT_FINISHED=0,
    PLAYER_WINS,
    PLAYER_BURSTS,
	PLAYER_NATURAL_BLACKJACK,
    PLAYER_BLACKJACK,
	DEALER_WINS,
	DEALER_BURSTS,
	DEALER_BLACKJACK,
	TIE,
	BLACKJACK_TIE
};
std::string readFromSocketDelim(asio::ip::tcp::socket& sock){
    asio::streambuf buf;
    asio::read_until(sock, buf, '\n');
    std::string message;

    std::istream input_stream(&buf);
    std::getline(input_stream, message);
    return message;
}
// BlackJack player extends player
class BlackJackPlayer : virtual public Player {
	public:
		BlackJackPlayer() : Player(){}
		string printPublicHand();
		string printPrivateHand();
		int score();
};

// Public hand masks the first card (face down card)
string BlackJackPlayer::printPublicHand(){
	string S = "";
	if(int(Hand.size()>0)){
		S+="[*]";
	for(int i =1; i<int(Hand.size()); i++)
		S += "[" + Hand.at(i).getStringValue() +  "]";
	}
	return S;
}

// Private hand print shows all cards
string BlackJackPlayer::printPrivateHand(){
	string S = "";
	for(int i = 0; i<int(Hand.size()); i++)
			S += "[" + Hand.at(i).getStringValue() +  "]";
	return S;
}

// BlackJack dealer inherits from both BlackJackPlayer and Dealer classes
class BlackJackDealer : public virtual BlackJackPlayer, public virtual Dealer{
	public:
		BlackJackDealer(): BlackJackPlayer(), Dealer(){}
		void deal(BlackJackPlayer player);
};

#endif /* BLACKJACK_H_ */
