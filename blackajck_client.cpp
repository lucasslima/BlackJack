#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "BlackJack.h"

using boost::lexical_cast;
using boost::asio::ip::tcp;
using namespace std;
using namespace boost;


char inputAction();
int inputWager(int);
void handleResult(ResponseCode gameResult, int pscore, int wager);

int main(int argc, char* argv[]) {
    try {
        // the user should specify the server - the 2nd argument
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], "7000");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // The connection is open.
        //Now we need to talk to the server
        // We use a boost::array to hold the received data.
        boost::system::error_code error;
        boost::system::error_code ignored_error;

        // The boost::asio::buffer() function automatically determines
        // the size of the array to help prevent buffer overruns.
        //Recieve initial chips from the server
        size_t len;
        if (error == boost::asio::error::eof){
            cout << "Connection cleanly closed" << endl;
            exit(1);
        }
        int playerChips = lexical_cast<int>(readFromSocketDelim(socket));
        cout << "Recieved chips: " << playerChips << endl;
        char continue_play = 'Y';
        // Game play
        cout << "Welcome to Black Jack!" << endl;
        int round = 0;
        while (playerChips > 0 and continue_play == 'Y') {
            round++;
            cout << "Round #" << round << ": " << endl;
            cout << "Place your wager (Blackjack pays 2:1 and all other wins pay 1:1). " << endl;

            // Get and process player's wager from console
            int num = inputWager(playerChips);
            cout << "########################################" << endl;

            boost::asio::write(socket, boost::asio::buffer(std::to_string(num) + DELIMITER ), ignored_error);
            // Get player's requested action from console

            //Playing loop
            
            ResponseCode roundResult = NOT_FINISHED;
            int turn = 0;
            while (roundResult == NOT_FINISHED){
                turn++;
                char action;

                string hands = readFromSocketDelim(socket);
                std::vector<std::string> gameInfo;
                boost::split(gameInfo,hands, boost::is_any_of("-"));
                string playerHand = gameInfo[0];
                string dealerHand = gameInfo[1];
                int handValue = lexical_cast<int>(gameInfo[2]) ;
                cout << "Turn #" << turn << endl;
                cout << "Dealer's hand: " << dealerHand << endl;
                cout << "Player's hand: " << playerHand << "\t\t Player score: "
                     << handValue << endl << endl;
                action = inputAction();
                string sendAction(1,action);
                boost::asio::write(socket, boost::asio::buffer(sendAction+ DELIMITER) , ignored_error);
                roundResult = (ResponseCode) lexical_cast<int>(readFromSocketDelim(socket));
            }
            std::vector<std::string> finalresult;
            string result = readFromSocketDelim(socket);
            boost::split(finalresult,result, boost::is_any_of("-"));
            string playerHand = finalresult[0];
            string dealerHand = finalresult[1];
            int playerScore = lexical_cast<int>(finalresult[2]);
            int dealerScore = lexical_cast<int>(finalresult[3]);
            playerChips = lexical_cast<int>(finalresult[4]);
            cout << "########################################" << endl;
            cout << "Dealer final score: " << dealerScore << '\t' << "Hand: " << dealerHand<< endl;
            cout << "Player final score: " << playerScore << '\t' << "Hand: " << dealerHand<< endl;
            handleResult(roundResult, playerScore, num ) ;
            cout << "########################################" << endl;
        }
    }
    // handle any exceptions that may have been thrown.
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
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
void handleResult(ResponseCode gameResult, int pscore, int wager){

	switch(gameResult) {
        case PLAYER_WINS:
            cout << "Player wins " << wager << " dollars!" << endl;
            std::cout << "Player wins with score " << pscore << ". Win " << wager << " dollars." << endl;
            break;
        case PLAYER_BURSTS:
            std::cout << "Player busts with score " << pscore << ". Lose " << wager << " dollars." << endl;
            break;
        case PLAYER_BLACKJACK:
            cout << "Player has blackjack! Player wins " << 2 * wager << " dollars!" << endl;
            break;
        case DEALER_WINS:
            std::cout << "Player loses with score " << pscore << ". Lose " << wager << " dollars." << endl;

            break;
        case DEALER_BURSTS:
            std::cout << "Dealer busts and player wins with score " << pscore << ". Win: " << wager
                      << " dollars." << endl;
            break;
        case DEALER_BLACKJACK:
            cout << "Dealer has blackjack.  Player loses" << endl;
            break;
        case TIE:
            std::cout << "Player and dealer tie with " << pscore << "." << endl;
            break;
        case BLACKJACK_TIE:
            cout << "Dealer and player both have blackjack! Tie." << endl;
            break;
        default:
            break;
    }
}