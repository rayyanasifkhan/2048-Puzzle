// For more information about SFML graphics, see: https://www.sfml-dev.org/tutorials
// Be sure to close the old window each time you rebuild and rerun, to ensure you
// are seeing the latest output. 
// 
// Author: Rayyan Khan
// Class:  UIC CS-141, Spring 2019
// System: C++ on cloud-based Codio.com 
//    
#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // Since we are using multiple libraries, now use std::
                             // in front of every cin, cout, endl, and string 
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen() 
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)
#include <iomanip>

const int WindowXSize = 400;
const int WindowYSize = 500;

const int MaxBoardSize = 12;          // Max number of squares per side
const int MaxTileStartValue = 1024;   // Max tile value to start out on a 4x4 board


//---------------------------------------------------------------------------------------
class Square {
	public:
		// Default Constructor 
		Square()
		{
			size = 0;
			xPosition = 0;
			yPosition = 0;
			color = sf::Color::Black;
			isVisible = false;
			isCaptured = false;
			text = "";		
		}
	
		// Fully-qualified constructor, used to set all fields
		Square( int theSize, int theXPosition, int theYPosition, 
			     const sf::Color &theColor, bool theVisibility, std::string theText)
		{
			// Set the class fields
			size = theSize;
			xPosition = theXPosition;
			yPosition = theYPosition;
			color = theColor;
			isVisible = theVisibility;
			isCaptured = false;   // By default squares have not been captured
			text = theText;
			// Use the values to set the display characteristics of theSquare
			theSquare.setSize( sf::Vector2f(theSize, theSize));
			theSquare.setPosition( theXPosition, theYPosition);   // Set the position of the square
			theSquare.setFillColor( theColor);
		}
			
		// Get (accessor) functions
		sf::RectangleShape getTheSquare() { return theSquare; }
		int getSize() { return size; }
		int getXPosition() { return xPosition; }
		int getYPosition() { return yPosition; }
		sf::Color& getColor() { return color; }
		bool getIsVisible() { return isVisible; }
		bool getIsCaptured() { return isCaptured; }
		std::string getText() { return text; }
	
		// Set (mutator) functions
		void setSize( int theSize) { 
			size = theSize; 
			theSquare.setSize( sf::Vector2f(theSize, theSize));
		}
		void setXPosition( int theXPosition) { 
			xPosition = theXPosition; 
			theSquare.setPosition( theXPosition, yPosition);   // Set the position of the square
		}
		void setYPosition( int theYPosition) { 
			yPosition = theYPosition; 
			theSquare.setPosition( xPosition, theYPosition);   // Set the position of the square
		}
		void setColor( sf::Color & theColor) { 
			color = theColor; 
			theSquare.setFillColor( theColor);    // Also update the color on the square itself
		}
		void setColor( int R, int G, int B) {
			sf::Color theNewColor( R, G, B);
			color = theNewColor;
			theSquare.setFillColor( theNewColor);
		}
		void setVisibility( bool theVisibility) { isVisible = theVisibility; }
		void setIsCaptured( bool isCaptured) { this->isCaptured = isCaptured; }
		void setText( std::string theText) { text = theText; }

		// Utility functions
		void displayText( sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);
	
	private:
		int size;
		int xPosition;
		int yPosition;
		sf::Color color;
		bool isVisible;
		bool isCaptured;   // Indicates whether or not it is part of the captured area
		std::string text;
		sf::RectangleShape theSquare;

}; //end class Square

//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text 
// associated with this Square.
// 
// Assuming we display output in sf::RenderWindow window(...), then call this function using: 
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
void Square::displayText( 
		sf::RenderWindow *pWindow,   // The window into which we draw everything
		sf::Font theFont,            // Font to be used in displaying text   
		sf::Color theColor,          // Color of the font
		int textSize)                // Size of the text to be displayed
{	
	// Create a sf::Text object to draw the text, using a sf::Text constructor
	sf::Text theText( text,        // text is a class data member
					  theFont,     // font from a font file, passed as a parameter
					  textSize);   // this is the size of text to be displayed

	// Text color is the designated one, unless the background is Yellow, in which case the text
	// color gets changed to blue so we can see it, since we can't see white-on-yellow very well
	if( this->getColor() == sf::Color::Yellow) {
		theColor = sf::Color::Blue;
	}
	theText.setColor( theColor);

	// Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
	// For horizontal center, find the center of the square and subtract half the width of the text 
	int theXPosition = xPosition + (size / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
	// For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
	int theYPosition = yPosition + (size - theText.getCharacterSize()) / 2;
	// Use an additional offset to get it centered
	int offset = 5;
	theText.setPosition( theXPosition + offset, theYPosition - offset);

	// Finally draw the Text object in the RenderWindow
	pWindow->draw( theText);
}

//---------------------------------------------------------------------------------------
// Initialize the font
void initializeFont( sf::Font &theFont)
{
	// Create the global font object from the font file
	if (!theFont.loadFromFile("arial.ttf"))
	{
		std::cout << "Unable to load font. " << std::endl;
		exit( -1);
	}	
}

//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
    std::cout << "   \n"
			  << "Welcome to 1024. \n"
			  << "  \n"
			  << "For each move enter a direction as a letter key, as follows: \n"
			  << "    W    \n"
			  << "  A S D  \n"
			  << "where A=left,W=up, D=right and S=down. \n"
			  << "  \n"
			  << "After a move, when two identical valued tiles come together they    \n"
			  << "join to become a new single tile with the value of the sum of the   \n"
			  << "two originals. This value gets added to the score.  On each move    \n"
			  << "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
			  << "square.  User input of x exits the game.                            \n"
			  << "  \n";
}//end displayInstructions()

//--------------------------------------------------------------------
// Place a randomly selected value of 2 or 4 into a random open square 
// on the board.
void placeRandomPiece( int board[], int squaresPerSide)
{
    // Randomly choose a piece to be placed (2 or 4)
    int pieceToPlace = 2;
    if( rand()%2 == 1) {
        pieceToPlace = 4;
    }
    
    // Find an unoccupied square that currently has a 0
    int index;
    do {
        index = rand() % (squaresPerSide*squaresPerSide);
    }while( board[ index] != 0);
    
    // board at position index is blank, so place piece there
    board[ index] = pieceToPlace;
}//end placeRandomPiece()

//--------------------------------------------------------------------
//Display Board
void displayAsciiBoard( int board[], int squaresPerSide, int score)
{
    std::cout << "\n"
         << "        Score: " << score << "\n";
    for( int row=0; row<squaresPerSide; row++) {
        std::cout << "   ";
        for( int col=0; col<squaresPerSide; col++ ) {
            int current = row*squaresPerSide + col;  // 1-d index corresponding to row & col
            std::cout << std::setw( 6);    // set output field size to 6 (Requires #include <iomanip> )
            // display '.' if board value is 0
            if( board[ current] == 0) {
               std::cout << '.';
            }
            else {
               std::cout << board[ current];
            }
        }
        std::cout << "\n\n";
    }  
}//end displayAsciiBoard()

//---------------------------------------------------------------------------------------
//Copy one integer array into another
void copyBoard(int boardToCopy[], int board[]) {
    for (int i = 0; i < MaxBoardSize * MaxBoardSize; i++) {
        boardToCopy[i] = board[i];
    }
}//end copyBoard()

//---------------------------------------------------------------------------------------
//Make a left move
void makeMovesLeft(int board[], int squaresPerSide, int &score) {
    
    int sumIndex = -1;
    int index = 0;
    int indexCopy;
   
    for(int i = 0; i < squaresPerSide * squaresPerSide; i++) {
        indexCopy = index;
        
        while (board[indexCopy] != 0 && (indexCopy % squaresPerSide != 0) && board[indexCopy - 1] == 0 && indexCopy > 0 ) { 
            
            board[indexCopy - 1] = board[indexCopy];
            board[indexCopy] = 0;
            indexCopy--;           
        }
        
		if (board[indexCopy] == board[indexCopy - 1] && indexCopy > 0 && sumIndex != indexCopy - 1 && board[indexCopy] != 0 && indexCopy % squaresPerSide  != 0) {
            board[indexCopy - 1] = board[indexCopy] * 2;
            score += board[indexCopy - 1];
            sumIndex = indexCopy - 1;
            board[indexCopy] = 0;  
        }
    
        index++;
              
    }
}//end makeMovesLeft() 

//--------------------------------------------------------------------
//Make a right move
void makeMovesRight(int board[], int squaresPerSide, int &score) {
    
    int index = (squaresPerSide * squaresPerSide) - 1;
    int indexCopy;
    int sumIndex = -1;
    
    for(int i = 0; i < squaresPerSide * squaresPerSide ; i++) {
        indexCopy = index;
        
        while ((board[indexCopy] != 0) && ((indexCopy + 1) % (squaresPerSide) != 0) && (board[indexCopy + 1] == 0) && indexCopy < squaresPerSide * squaresPerSide - 1  ) {
            
            board[indexCopy + 1] = board[indexCopy];
            board[indexCopy] = 0;
            indexCopy++; 
        }
        
        if (board[indexCopy] == board[indexCopy + 1] && indexCopy < squaresPerSide * squaresPerSide - 1 && sumIndex != indexCopy + 1 && board[indexCopy] != 0 && (indexCopy + 1) % squaresPerSide  != 0) {
            board[indexCopy + 1] = board[indexCopy + 1] * 2;
            score += board[indexCopy + 1];
            sumIndex = indexCopy + 1;
            board[indexCopy] = 0; 
        }
        
        index--;
        
    }
}//end makeMovesRight()
    
//--------------------------------------------------------------------
//Make a up move    
void makeMovesUp(int board[], int squaresPerSide, int &score) {
    
    int sumIndex = -1;
    
    for(int i = 0; i < squaresPerSide; i++) {
        int index = 0 + i;
        int indexCopy;
        
        while (index < squaresPerSide * squaresPerSide) {
            indexCopy = index;
            
            
            while ((board[indexCopy] != 0) && ((indexCopy) >= (squaresPerSide)) && (board[indexCopy - squaresPerSide] == 0) ) {

                board[indexCopy - squaresPerSide] = board[indexCopy];
                board[indexCopy] = 0;
                indexCopy -= squaresPerSide;
            }

            if (indexCopy >= squaresPerSide && board[indexCopy] == board[indexCopy - squaresPerSide] && sumIndex != indexCopy - squaresPerSide  && board[indexCopy] != 0 && indexCopy >= squaresPerSide) {
                board[indexCopy - squaresPerSide] = board[indexCopy - squaresPerSide] * 2;
                sumIndex = indexCopy - squaresPerSide;
                score += board[indexCopy - squaresPerSide];
                board[indexCopy] = 0; 
            }
            
            index += squaresPerSide;
            
        }   
    }
   
}//end makeMovesUp()

//--------------------------------------------------------------------
//Make a down move
void makeMovesDown(int board[], int squaresPerSide, int &score) {
    
    int sumIndex = -1;
    for(int i = 0; i > -squaresPerSide; i--) {
        int index =  squaresPerSide * squaresPerSide - 1 + i;
        int indexCopy;
        
        while (index >= 0) {
            indexCopy = index;

            while ((board[indexCopy] != 0) && ((indexCopy) < (squaresPerSide * squaresPerSide - squaresPerSide)) && (board[indexCopy + squaresPerSide] == 0) ) {

                board[indexCopy + squaresPerSide] = board[indexCopy];
                board[indexCopy] = 0;
                indexCopy += squaresPerSide;
            }

            if ((indexCopy <= squaresPerSide * squaresPerSide - squaresPerSide) && board[indexCopy] == board[indexCopy + squaresPerSide] && board[indexCopy] != 0 && sumIndex != indexCopy + squaresPerSide && indexCopy < ((squaresPerSide * squaresPerSide) - squaresPerSide)) {
                board[indexCopy + squaresPerSide] = board[indexCopy + squaresPerSide] * 2;
                sumIndex = indexCopy + squaresPerSide;
                score += board[indexCopy + squaresPerSide];
                board[indexCopy] = 0;               
            }
            
            index -= squaresPerSide;

        }   
    }
          
}//end makeMovesDown()

//--------------------------------------------------------------------
void changeGraphicsBoard(int board[], Square squaresArray[], 
                         int squaresPerSide, int move, sf::RenderWindow &window,  
                         sf::Font &font)
{
    int arraySize = squaresPerSide * squaresPerSide;
    int col = 0;
    int row = 0;
   
    for( int i=0; i<arraySize; i++) {

    // Allocate each square individually.
    // Store a string in each square which contains its number, in order of creation
           
        col = i % squaresPerSide;
        row = i / squaresPerSide;
        
        char name[ 81];

        // Squares with a 0 value should not have a number displayed
        if( board[ i] == 0) {
            strcpy( name, "");   // "print" a blank text string
        }
        
        else {
           sprintf( name, "%d", board[ i]);   // "print" the square creation order number into a string to be stored in the square
        }
        
        // Set each array pointer have the address of a new Square, created with a Square constructor
        squaresArray[ i] = Square((90*4)/squaresPerSide, ((90 * col + col * 10) * 4)/squaresPerSide , ((90 * row + row * 10) * 4)/squaresPerSide, sf::Color::White, true, name);

        // Draw the square
        window.draw( squaresArray[ i].getTheSquare());

        // Draw the text associated with the Square, in the window with the indicated color and text size
        if (board[i] < 100){
            squaresArray[i].displayText( &window, font, sf::Color::Black, (50 * 4)/squaresPerSide);
        }
        
        else if (board[i] < 1000) {
            squaresArray[i].displayText( &window, font, sf::Color::Black, ((50 * 4)/squaresPerSide)/1.3);
        }
        
        else if (board[i] < 10000) {
            squaresArray[i].displayText( &window, font, sf::Color::Black, ((50 * 4)/squaresPerSide)/1.9);
        }
    }
}
    
//--------------------------------------------------------------------
//Check if a move is made or not  
bool moveMade(int board1[], int board2[], int squaresPerSide) {
    for (int i = 0; i < squaresPerSide * squaresPerSide; i++) {
        if(board1[i] != board2[i]) {
            return true;
        }
    }
    
    return false;
}// end moveMade()

//--------------------------------------------------------------------
//Check whether the board is full or not
bool gameIsOver( int board[], int squaresPerSide, int maxTileValue) {

    // See if the maxTileValue is found anywhere on the board.
    // If so, game is over.
    for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
        if( board[ i] == maxTileValue) {
            std:: cout << "Congratulations!  You made it to " << maxTileValue << " !!!" << "\n";
            return true;  // game is over
        }
    }
    
    // See if there are any open squares.  If so return true since we aren't done
    for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
        if( board[ i] == 0) {
            return false;  // there are open squares, so game is not over
        }
    }
    
    // All squares are full.
    // To check if board is done, make a copy of board, then slide left
    // and slide down.  If resulting board is same as original, we are done
    // with game since there are no moves to be made.
    int boardCopy[ squaresPerSide * squaresPerSide];
    for (int i = 0; i < squaresPerSide * squaresPerSide; i++) {
    	boardCopy[i] = board[i];
	}
    
    int tempScore = 0;   // used as a placeHolder only for function calls below
    makeMovesLeft( boardCopy, squaresPerSide, tempScore);
    makeMovesDown( boardCopy, squaresPerSide, tempScore);
    
    // Compare each square of boards.  If any is different, a move is possible
    for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
        if( boardCopy[ i] != board[ i]) {
            return false;  // Game is not over
        }
    }
    
    std::cout << "\n"
              << "No more available moves.  Game is over.\n"
              << "\n";
    return true;  // Game is over since all squares are full and there are no moves
}//end gameIsOver()

//--------------------------------------------------------------------
struct Node {
    int board[MaxBoardSize * MaxBoardSize];
    int move;
    int score;
    Node* pNext;
};

//--------------------------------------------------------------------
int main()
{	
    int score = 0;
    int squaresPerSide = 4;
	char aString[ 81];    // used to create a string to display text in a square 
	int board[ MaxBoardSize * MaxBoardSize];        // Board of 4 integer values
    int maxTileValue = MaxTileStartValue;  // 1024 for 4x4 board, 2048 for 5x5, 4096 for 6x6, etc.
    char userInput = ' ';                  // handles user input
    int move = 1;                          // user move counter
    int previousBoard[MaxBoardSize * MaxBoardSize];  // space for copy of board, used to see
	
	// Create and initialize the font, to be used in displaying text.
	sf::Font font;  
	initializeFont( font);
    
	// Create the destination window
	sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 6: 1024");
	 
	// Create the messages label at the bottom of the screen, to be used in displaying debugging information.
	sf::Text messagesLabel( "Welcome to 1024", font, 24);
    
	// Make a text object from the font
	messagesLabel.setColor(sf::Color::White);
    
	// Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
	messagesLabel.setPosition( 0, WindowYSize - messagesLabel.getCharacterSize() - 5);  
	
    
	displayInstructions();
	
    
	// Initialize numeric values for board array of integers to all be 0
	for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
		board[ i] = 0;
	}
    
	// Set two randomly selected board positions to have a value 2 or 4 in it.
	placeRandomPiece( board, squaresPerSide);
	placeRandomPiece( board, squaresPerSide);
	
    
	// Create array of Square objects
	int arraySize = squaresPerSide * squaresPerSide;
    Square squaresArray[ arraySize];
    changeGraphicsBoard(board, squaresArray, squaresPerSide, move, window, font);	    
    
    Node* pTemp = new Node;
    pTemp->move = move;
    pTemp->score = score;
    copyBoard(pTemp->board, board);
    Node* pHead = pTemp;
    
	while (true)
    {      
        sprintf( aString, "Move %d", move);   //Printing out the move number in graphical mode
		messagesLabel.setString( aString);        
		window.draw( messagesLabel); 
        
        window.display(); // Display both the boards
        displayAsciiBoard( board, squaresPerSide, score);
        
        std::cout << "\n\n        List: ";
        for(int i = move; i > 1; i--) {
            std::cout << i << "->";
        }
        std::cout << "1\n\n";
        
        copyBoard(previousBoard, board);
        
        bool placed = false; // True if user places a value
        bool resized = false; // True if user resizes the board
        bool undo = false; // True if user undos a move
        int product = 2 * 2 * 2 * 2 * 2 * 2; // Each maxTileValue is of the form 2^(6 + n), where n is squaresPerSide.
        
		std::cout << move << ". Your move: ";
		std::cin >> userInput;
        
        switch (userInput) {
            case 'x': //Exit
                    std::cout << "Thanks for playing. Exiting program... \n\n";
                    exit( 0);
                    break;
            case 'a': //Move left
                    makeMovesLeft(board, squaresPerSide, score);
                    break;
            case 's': //Move down
                    makeMovesDown(board, squaresPerSide, score);
                    break;
            case 'd': //Move right
                    makeMovesRight(board, squaresPerSide, score);
                    break;
            case 'w': //Move up
                    makeMovesUp(board, squaresPerSide, score);
                    break;
            case 'p':                    
                    int valueToPlace;
                    int locToPlace;
                
                    std::cin >> locToPlace;
                    std::cin >> valueToPlace;
                
                    board[locToPlace] = valueToPlace;
                    placed = true;
                
                    break;
            case 'u':
                    if (move == 1) {
                        std::cout << "\n\n        *** You cannot undo past the beginning of the game.  Please retry. ***  \n\n";
                        break;
                    }
                    
                    std::cout << "\n        * Undoing move *\n\n";
                    move = pTemp->pNext->move;
                    copyBoard(board, pTemp->pNext->board);
                    score = pTemp->pNext->score;
                    pHead = pTemp->pNext;
                    pTemp = pTemp->pNext;
                    undo = true;
                    changeGraphicsBoard(board, squaresArray, squaresPerSide, move, window, font);

                    break;
            case 'r':                   
                    std::cout << "Resetting board\n\nEnter the size board you want, between 4 and 12: ";
                
                    std::cin >> squaresPerSide;
                
                    score = 0;
                    move = 1;
                
                    for (int i = 0; i < MaxBoardSize * MaxBoardSize; i++) {
                        board[i] = 0;
                    }
                    
                    for (int i = 0; i < squaresPerSide; i++) {
                        product *= 2;
                    }
                
                    maxTileValue = product;
                
                    std::cout << "Game ends when you reach "<< maxTileValue <<" .\n\n";
                
                    placeRandomPiece(board, squaresPerSide); //Placing two random tiles after resizing.
                    placeRandomPiece(board, squaresPerSide);
                
                    resized = true;
                	// Clear the graphics window, erasing what is displayed
                    window.clear();
                    
                    continue;
            default:
                    std::cout << "Invalid input, please retry.\n\n";
                    continue;
                    
        }//end switch( userInput) 
        
        
        if (moveMade(board, previousBoard, squaresPerSide) && !placed && !resized) {           
            if (!undo) {
                placeRandomPiece(board, squaresPerSide);
                move++;
            }
            changeGraphicsBoard(board, squaresArray, squaresPerSide, move, window, font);	    
        }
        
        
        if( gameIsOver( board, squaresPerSide, maxTileValue)) {
            // Display the final board
            displayAsciiBoard( board, squaresPerSide, score);
            window.display();
           break;
        }
         
        if(!undo && moveMade(board, previousBoard, squaresPerSide)) {
            pTemp = new Node;
            pTemp->move = move;
            pTemp->score = score;
            copyBoard(pTemp->board, board);
            pTemp->pNext = pHead;
            pHead = pTemp;
        }
               
		//system("clear");   // Clear the screen 

		// Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
		std::this_thread::sleep_for(std::chrono::milliseconds( 10));
        
        window.clear();
        changeGraphicsBoard(board, squaresArray, squaresPerSide, move, window, font);

		
	}//end while( window.isOpen())
	
	return 0;
}//end main()
