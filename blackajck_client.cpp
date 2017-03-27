#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "BlackJack.h"

using boost::lexical_cast;
using boost::asio::ip::tcp;
using namespace std;


char inputAction();
int inputWager(int);
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
        boost::array<char, 128> initialChipsBuffer;
        boost::system::error_code error;

        // The boost::asio::buffer() function automatically determines
        // the size of the array to help prevent buffer overruns.
        //Recieve initial chips from the server
        size_t len = socket.receive(boost::asio::buffer(initialChipsBuffer));
        if (error == boost::asio::error::eof){
            cout << "Connection cleanly closed" << endl;
            exit(1);
        }
        int playerChips = lexical_cast<int>(initialChipsBuffer.data(), len);
        cout << "Recieved chips: " << playerChips << endl;
        int round;
        char continue_play = 'Y';
        // Game play
        cout << "Welcome to Black Jack!" << endl;
        while (playerChips > 0 and continue_play == 'Y') {
            boost::array<char, 128> roundBuffer;
            len = socket.receive(boost::asio::buffer(roundBuffer));
            round = lexical_cast<int>(roundBuffer.data(), len);
            cout << "Round #" << round << ": " << endl;
            cout << "Place your wager (Blackjack pays 2:1 and all other wins pay 1:1). " << endl;

            // Get and process player's wager from console
            int num = inputWager(playerChips);
            cout << "########################################" << endl;

            // Get player's requested action from console
            //Playing loop
            
            ResponseCode roundResult = NOT_FINISHED;
            while (roundResult == NOT_FINISHED){
                int turn;
                char action;
                boost::system::error_code ignored_error;
                boost::array<char, 128> turnBuffer;
                boost::array<char, 128> dealerHandBuffer;
                boost::array<char, 128> playerHandBuffer;
                boost::array<char, 128> handValueBuffer;

                len = socket.read_some(boost::asio::buffer(turnBuffer), error);
                turn = lexical_cast<int>(turnBuffer.data());

                len = socket.read_some(boost::asio::buffer(dealerHandBuffer), error);
                string dealerHand(dealerHand.data(), len);

                len = socket.read_some(boost::asio::buffer(playerHandBuffer), error);
                string playerHand(playerHandBuffer.data(), len);

                len = socket.read_some(boost::asio::buffer(handValueBuffer), error);
                int handValue = lexical_cast<int>(handValueBuffer.data());

                // the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error,
                // which is how we know to exit the loop.
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw boost::system::system_error(error); // Some other error.

                cout << "Turn #" << turn << endl;
                cout << "Dealer's hand: " << dealerHand << endl;
                cout << "Player's hand: " << playerHand << "\t\t Player score: "
                     << handValue << endl << endl;
                action = inputAction();
                string sendAction(&action);
                boost::asio::write(socket, boost::asio::buffer(sendAction), ignored_error);
                len = socket.read_some(boost::asio::buffer(handValueBuffer), error);
                roundResult = (ResponseCode) lexical_cast<int>(handValueBuffer.data());
            }

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
//string handleResult(ResponseCode gameResult){
//
//	cout << "########################################" << endl;
////	cout << "Dealer final score: " << handValue(dealer)
////			<<"\t\t Hand: "<< dealer.printPrivateHand() << endl;
////	cout << "Player final score: " << handValue(player)
////			<<"\t\t Hand: "<< player.printPrivateHand() << endl;
//
//	switch(gameResult) {
//        case PLAYER_WINS:
//            cout << "Player wins " << wager << " dollars!" << endl;
//            std::cout << "Player wins with score " << pscore << ". Win " << wager << " dollars." << endl;
//            break;
//        case PLAYER_BURSTS:
//            std::cout << "Player busts with score " << pscore << ". Lose " << wager << " dollars." << endl;
//            break;
//        case PLAYER_BLACKJACK:
//            cout << "Player has blackjack! Player wins " << 2 * wager << " dollars!" << endl;
//            break;
//        case DEALER_WINS:
//            std::cout << "Player loses with score " << pscore << ". Lose " << wager << " dollars." << endl;
//
//            break;
//        case DEALER_BURSTS:
//            std::cout << "Dealer busts and player wins with score " << pscore << ". Win: " << wager
//                      << " dollars." << endl;
//            break;
//        case DEALER_BLACKJACK:
//            cout << "Dealer has blackjack.  Player loses" << endl;
//            break;
//        case TIE:
//            std::cout << "Player and dealer tie with " << pscore << "." << endl;
//            break;
//        case BLACKJACK_TIE:
//            cout << "Dealer and player both have blackjack! Tie." << endl;
//            break;
//    }
//	return finalResult;
//}