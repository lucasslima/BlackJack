//============================================================================
// Name        : BlackJack.cpp
// Author      : Lucas Sousa Lima
// Email       : r_walker@zoho.com
//============================================================================
#include <ctime>
#include <iostream>
#include <boost/asio.hpp>
#include <unordered_map>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include "BlackJack.h"
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <thread>
#include "players.h"

using namespace std;

using boost::asio::ip::tcp;


// function templates for game play
void deal(BlackJackDealer&, BlackJackPlayer&);
void hit(BlackJackPlayer&, BlackJackDealer&);
int handValue(BlackJackPlayer&);
ResponseCode scoreRound(BlackJackPlayer&, BlackJackDealer&);
void autoplay(BlackJackDealer&);

// Define face values of cards based on standard black jack rules using
// a static map. Note, we don't code a value for "A" because
// it can be either 2 or 11 in Blackjack.
std::unordered_map<std::string,int> create_map()
{
	std::unordered_map<std::string, int> cardvalues;
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
std::unordered_map<std::string,int> cardvalues = create_map();

int main()
{
    try
	{
        BlackJackDealer dealer = BlackJackDealer();
        BlackJackPlayer player = BlackJackPlayer();

        // Player begins with 100 chips
        player.setChips(500);

        // control flow variables
//        char continuePlaying = 'Y';
        bool continuePlaying = true;
		// Any program that uses asio need to have at least one io_service object
		boost::asio::io_service io_service;

		// acceptor object needs to be created to listen for new connections
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 7000));
        boost::system::error_code ignored_error;
        // creates a socket
        tcp::socket socket(io_service);

        // wait and listen
		cout << "Waiting for connection. " << endl;
        acceptor.accept(socket);
        boost::asio::write(socket, boost::asio::buffer(std::to_string(player.getChipCount() ) + DELIMITER ), ignored_error);
        while (continuePlaying)
        {
			int wager = lexical_cast<int>(readFromSocketDelim(socket));
			cout << "Wager received: " << wager << endl;
            player.setWager(wager);
			deal(dealer, player);
            int turn = 1;
			while(true){
                stringstream hands;
                string action;
                hands << player.printPrivateHand(); hands << "-"; hands << dealer.printPublicHand(); hands << "-"; hands << handValue(player); hands << DELIMITER;
                cout << "Hands message: " << hands.str() << endl;
                boost::asio::write(socket, boost::asio::buffer(hands.str() ), ignored_error);
				action = readFromSocketDelim(socket);
                cout << "Action received: " << action << endl;
                if (action ==  "H")
                    hit(player, dealer);
                if (action == "S" or handValue(player)>21)
                    break;
                turn++;
                boost::asio::write(socket, boost::asio::buffer(std::to_string(ResponseCode::NOT_FINISHED) + DELIMITER ), ignored_error);
            }
			// Dealer's turn
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            if(handValue(player)<=21)
                autoplay(dealer);

            // Score round
            ResponseCode result = scoreRound(player, dealer);
            boost::asio::write(socket, boost::asio::buffer(std::to_string(result) + DELIMITER ), ignored_error);

            stringstream finalresult;
            finalresult << player.printPrivateHand(); finalresult << "-"; finalresult << dealer.printPrivateHand();
            finalresult  << "-"; finalresult << handValue(player); finalresult  << "-"; finalresult << handValue(dealer);
			finalresult << "-" << player.getChipCount(); finalresult << DELIMITER;
			cout << "Final result string: " << finalresult.str() << endl;
            boost::asio::write(socket, boost::asio::buffer(finalresult.str() + DELIMITER ), ignored_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

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

	for(int i=0; i<player.Hand.size(); i++){
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
ResponseCode scoreRound(BlackJackPlayer& player, BlackJackDealer& dealer){
	int pscore = handValue(player);
	int dscore = handValue(dealer);
	int pcards = int(player.Hand.size());
	int dcards = int(dealer.Hand.size());
	int wager = player.getWager();
	ResponseCode finalResult;

	cout << "########################################" << endl;
	cout << "Dealer final score: " << handValue(dealer)
			<<"\t\t Hand: "<< dealer.printPrivateHand() << endl;
	cout << "Player final score: " << handValue(player)
			<<"\t\t Hand: "<< player.printPrivateHand() << endl;

	if (pscore==21){
		if (pcards==2){
			if(dscore==21 and dcards==2){
				cout << "Dealer and player both have blackjack! Tie." << endl;
				finalResult =  BLACKJACK_TIE;
			}
			else {
				cout << "Player has blackjack! Player wins "<< 2*wager << " dollars!" << endl;
				player.gainChips(2*wager);
				finalResult = PLAYER_NATURAL_BLACKJACK;
			}
		}
		else if (dscore==21 and dcards==2){
			cout << "Dealer has blackjack.  Player loses" << endl;
			player.loseChips(wager);
			finalResult = DEALER_BLACKJACK;
		}
		else {
			cout << "Player wins "<< wager << " dollars!"<< endl;
			player.gainChips(wager);
            finalResult = PLAYER_BLACKJACK;
        }
	}
	else if (pscore>21){
		std::cout << "Player busts with score " << pscore <<". Lose "<< wager <<" dollars." <<endl;
		player.loseChips(wager);
        finalResult = PLAYER_BURSTS;
	}
	else if (dscore>21){
		std::cout << "Dealer busts and player wins with score " << pscore << ". Win: "<< wager
				<< " dollars." << endl;
		player.gainChips(wager);
		finalResult = DEALER_BURSTS;
	}
	else if (pscore<dscore){
		std::cout << "Player loses with score " << pscore <<". Lose "<< wager <<" dollars." <<endl;
		player.loseChips(wager);
		finalResult = DEALER_WINS;
	}
	else if (pscore>dscore){
		std::cout << "Player wins with score " << pscore <<". Win "<< wager <<" dollars." <<endl;
		player.gainChips(wager);
		finalResult = PLAYER_WINS;
	}
	else if (pscore == dscore){
		std::cout << "Player and dealer tie with " << pscore << "." <<endl;
        finalResult = TIE;
	}
	cout << "########################################" << endl;
	return finalResult;
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


