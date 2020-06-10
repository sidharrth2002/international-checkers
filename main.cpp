/**********|**********|**********|
Program: main.cpp
Name: Sidharrth Nagappan
**********|**********|**********/

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <ctype.h>
#include <fstream>
using namespace std;

//Function to clear screen
//can change command according to OS used
void clearScreen()
{
    system( "clear" );
}

void DisplayBoard();
bool MoveValidity(int turn, int starting_row, int starting_col, int final_row, int final_col, int& most_recent_capture, int& move_form);
void Game(int &turn, int& exit_flag);
void ReturnPositions(string coordinates, int& i, int& j);
bool AnotherMovePossible(int turn, int final_row, int final_col, int most_recent_capture);
void SaveToFile (int turn); 
void GetFromFile(int& turn);
bool WhoWins();

//Declaring the Board, global, to be used by all functions
char Board[10][10] = {
    {' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b'},
    {'b', ' ', 'b', ' ', 'b', ' ', 'b', ' ','b', ' '},
    {' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b'},
    {'b', ' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b', ' '},
    {' ', '#', ' ', '#', ' ', '#', ' ', '#', ' ', '#'},
    {'#', ' ', '#', ' ', '#', ' ', '#', ' ','#', ' '},
    {' ', 'a', ' ', 'a', ' ', 'a', ' ', 'a', ' ', 'a'},
    {'a', ' ', 'a', ' ', 'a', ' ', 'a', ' ','a', ' '},
    {' ', 'a', ' ', 'a', ' ', 'a', ' ', 'a', ' ', 'a'},
    {'a', ' ', 'a', ' ', 'a', ' ', 'a', ' ','a', ' '},
};

//the only inputs the user can enter, any coordinate they enter will be looped through this to check for validity
string valid_inputs[54] = {"b1", "d1", "f1", "h1", "j1",
                            "a2", "c2", "e2", "g2", "i2",
                            "b3", "d3", "f3", "h3", "j3",
                            "a4", "c4", "e4", "g4", "i4",
                            "b5", "d5", "f5", "h5", "j5",
                            "a6", "c6", "e6", "g6", "i6",
                            "b7", "d7", "f7", "h7", "j5",
                            "a8", "c8", "e8", "g8", "i8",
                            "b9", "d9", "f9", "h9", "j7", "j9",
                            "a10", "c10", "e10", "g10", "i10", 
                            "quit", "load", "save"
                            };

//Arrays holding valid moves in 4 possible directions
const int MoveRow[] = {-1, 1, 1, -1};
const int MoveCol[] = {-1, -1, 1, 1};
int a_score = 0;
int b_score = 0;
string winner;

int main() {
  int turn = 1;
  clearScreen();
  cout << "=====================================================================" << endl;
  cout << "Welcome to International Checkers by Sidharrth" << endl;
  cout << "I hope you are familiar with the rules of the game." << endl;
  cout << "=====================================================================" << endl;

  //Opening Menu Options
  string OpeningMenuOption;
  int loop_exit = 0;
  int exit_flag = 0;
  //Prints the Opening Menu
  //As usual, uses while loop for error validation
  while (loop_exit == 0) {
    cout << "1. Press 1 to start a new game." << endl;
    cout << "2. Press 2 to load a previous game." << endl;
    cout << "3. Press 3 to exit the game." << endl;
    cin >> OpeningMenuOption;
    if (OpeningMenuOption == "1" || OpeningMenuOption == "2" || OpeningMenuOption == "3") {
      loop_exit = 1;
    } else {
      clearScreen();
      cout << "Invalid input. Try Again!" << endl;
    }
  }
  clearScreen();
  //If user chooses to start the game, call the game function.
  if (OpeningMenuOption == "1") {
    Game(turn, exit_flag);
    //Once Game returns, it comes back to main, to display final positions and print exit message
    //Display the results
    if (exit_flag == 2) {
      cout << "We have exited the game. We will automatically save your progress!" << endl << endl;
      SaveToFile(turn);
    }
    cout << "Thank you and see you again!" << endl;
  }

  //if user wants to load previous game, call the GetFromFile() function
  //then call the game function when the board is updated  
  if (OpeningMenuOption == "2") {
    clearScreen();
    GetFromFile(turn);
    Game(turn, exit_flag);
    //once returns to main from game function because of entering save,
    //call the savetofile function
    if (exit_flag == 2) {
      cout << "We have exited the game. We will automatically save your progress" << endl;
      SaveToFile(turn);
    }
    cout << "Thank you and see you again!" << endl;
  }
  //exit program if exit selected.
  if (OpeningMenuOption == "3") {
    clearScreen();
    cout << "You have exited the program." << endl;
    exit(0);
  }
}

/*
Function to Print Checkerboard every time it is called
Data In: Array(Global), a_score (Global), b_score (Global)
Data Out: Checker Board
*/
void DisplayBoard() {
  int i;
  //print scores, just for player to see
  cout << "Player a's score: " << a_score << right << setw(30) << "Player b's score: " << b_score << endl;
  //loops by row, prints every column
  for (i = 0; i <= 9; ++i) {
    cout << "   +---+---+---+---+---+---+---+---+---+---+" << endl;
    cout << setw(2) << i + 1 << setw(2) << " | " << Board[i][0] << " | " << Board[i][1] << " | " << Board[i][2] << " | " << Board[i][3] << " | " <<
      Board[i][4] << " | " << Board[i][5] << " | " << Board[i][6] << " | " << Board[i][7] << " | " << Board[i][8] << " | " <<
      Board[i][9] << " | " << endl;
  }
  cout << "   +---+---+---+---+---+---+---+---+---+---+" << endl;
  cout << "     a   b   c   d   e   f   g   h   i   j    " << endl;
  //user input for the coordinates follow (x,y) system
  //I then convert to array index for convenience for use in functions, swapping, etc.
}

/*
Actual Game runs in this function
Data In: turn, exit_flag
*/
void Game(int & turn, int & exit_flag) {
  string GameMenuOption;
  string starting_pos, final_pos;
  string anothermovechoice;
  int starting_row, starting_col, final_row, final_col, exit_value, most_recent_capture, move_form;
  bool move_validity, movepossible, keeprunning, notKinged;
  keeprunning = true;
  notKinged = true;
  char temp;
  //for error checking via loops
  exit_value = 1;
  cout << "Coordinate inputs go by x-axis, y-axis, e.g. a1, b3" << endl;
  cout << "Remember, if you can make a capture from that position, you must make the capture! If not, I'll deem the move invalid!" << endl << endl;
  cout << "1) If you want to load from a saved file anytime, enter load at the start of any turn (I will load your past progress)." << endl;
  cout << "2) If you want to save progress at anytime, enter save at the start of any turn (I will save your progress)." << endl;
  cout << "3) If you want to save and exit at anytime, enter quit into any input (I will save your progress AND exit the game)." << endl << endl;
  DisplayBoard();
  //switches between players via a do-while loop
  do {
    //resets the variable that keeps track of the type of move (repeated move or new move)
    move_form = 0;
    most_recent_capture = 10;
    notKinged = true;
    //if turn is even, player b, if odd, player a
    if ((turn % 2) == 0) {
      cout << "Player b, it is your turn." << endl << endl;
    } else {
      cout << "Player a, it is your turn." << endl << endl;
    }

    //Asks for coordinates, converts them to array indexes, confirms whether the move is valid and makes the move (no cutting/jumping for now)
    cout << "Where do you want to move from (enter coordinates- e.g. b1 OR save/load/quit)?" << endl;
    exit_value = 1;
    //error validation
    do {
      cin >> starting_pos;
      for (int i = 0; i < 54; ++i) {
        if (starting_pos == valid_inputs[i]) {
          exit_value = 2;
          break;
        }
      }
      if (exit_value != 2) {
        cout << "Invalid coordinates (not on the board/you can't move there), try again: " << endl;
      }
    } while (exit_value == 1);

    //check if the sentinel value was entered
    //if it was, return to main
    if (starting_pos == "load") {
      clearScreen();
      GetFromFile(turn);
      DisplayBoard();
      continue;
    }
    if (starting_pos == "save") {
      SaveToFile(turn);
      clearScreen();
      cout << "Your progress has been saved!" << endl << endl;
      DisplayBoard();
      continue;
    }
    if (starting_pos == "quit") {
      exit_flag = 2;
      clearScreen();
      return;
    }
    //Asks destination coordinates and validates
    cout << "Where do you want to move to (enter coordinates - e.g. c2 or save/load/quit)?" << endl;
    //error validation for this input
    exit_value = 1;
    do {
      cin >> final_pos;
      for (int i = 0; i < 54; ++i) {
        if (final_pos == valid_inputs[i]) {
          exit_value = 2;
          break;
        }
      }
      if (exit_value != 2) {
        cout << "Invalid coordinates (not on the board/you can't move there), try again: " << endl;
      }
    } while (exit_value == 1);
    //checks again for sentinel value
    //if sentinel entered, return to main
    if (final_pos == "load") {
      clearScreen();
      GetFromFile(turn);
      DisplayBoard();
      continue;
    }
    if (final_pos == "save") {
      SaveToFile(turn);
      clearScreen();
      cout << "Your progress has been saved!" << endl << endl;
      DisplayBoard();
      continue;
    }
    if (final_pos == "quit") {
      exit_flag = 2;
      clearScreen();
      return;
    }
    //sends to function to convert string input to coordinates
    ReturnPositions(starting_pos, starting_row, starting_col);
    ReturnPositions(final_pos, final_row, final_col);
    //check whether move was valid
    move_validity = MoveValidity(turn, starting_row, starting_col, final_row, final_col, most_recent_capture, move_form);

    //Crowning Pieces
    if (move_validity) {
      //only crown if another move cannot be from that position
      if (AnotherMovePossible(turn, final_row, final_col, most_recent_capture) == false) {
        if (final_row == 0) {
          if (Board[final_row][final_col] == 'a') {
            Board[final_row][final_col] = 'A';
            notKinged = false;
          }
        }
        if (final_row == 9) {
          if (Board[final_row][final_col] == 'b') {
            Board[final_row][final_col] = 'B';
            notKinged = false;
          }
        }
      }
      //gives user the chance to make another capture if possible
      //but only if the original move was a capture, move_form keeps track of the type of the original move
      if (move_form == 2 && (notKinged == true)) {
        while (notKinged && AnotherMovePossible(turn, final_row, final_col, most_recent_capture)) {
          clearScreen();
          DisplayBoard();
          cout << "I checked and you can make another capture from that position." << endl;
          cout << "You are now at " << final_pos << "." << endl;
          //resets the starting position to the final position of the previous move
          //prepares for the next move in the same turn
          starting_pos = final_pos;
          move_validity = false;
          while (move_validity == false) {
            notKinged = true;
            exit_value = 1;
            cout << "Where do you want to move to (enter coordinates - e.g. c2)?" << endl;
            //error validation for this input
            do {
              cin >> final_pos;
              //only accepts quit as sentinel value here
              for (int i = 0; i < 52; ++i) {
                if (final_pos == valid_inputs[i]) {
                  exit_value = 2;
                  break;
                }
              }
              if (exit_value != 2) {
                cout << "Invalid coordinates (not on the board/you can't move there), try again: " << endl;
              }
            } while (exit_value == 1);

            if (final_pos == "quit") {
              exit_flag = 2;
              clearScreen();
              return;
            }
            //sends to function to convert string input to coordinates
            ReturnPositions(starting_pos, starting_row, starting_col);
            ReturnPositions(final_pos, final_row, final_col);
            move_validity = MoveValidity(turn, starting_row, starting_col, final_row, final_col, most_recent_capture, move_form);

            //Crowning Pieces
            if (move_validity) {
              if (AnotherMovePossible(turn, final_row, final_col, most_recent_capture) == false) {
                if (final_row == 0) {
                  if (Board[final_row][final_col] == 'a') {
                    Board[final_row][final_col] = 'A';
                    notKinged = false;
                  }
                }
                if (final_row == 9) {
                  if (Board[final_row][final_col] == 'b') {
                    Board[final_row][final_col] = 'B';
                    notKinged = false;
                  }
                }
              }
            } else {
              //if original move itself was invalid, repeat without incrementing turn
              clearScreen();
              DisplayBoard();
              cout << "Boss, that was an invalid move, give it another try!" << endl;
              cout << "You are now at " << starting_pos << "." << endl;
            }
          }
        }
      }
      //checks whether one player has no more pieces
      //decide the winner and print it
      if (WhoWins()) {
        clearScreen();
        DisplayBoard();
        cout << "The game is over!" << endl;
        cout << "CONGRATULATIONS! Player " << winner << " is the winner!" << endl;
        return;
      }
      //increment turn to switch player
      turn = turn + 1;
      clearScreen();
      DisplayBoard();
      //if the move was invalid, let them retry
      //stops players from switching by not incrementing the turn variable 
    } else {
        clearScreen();
        DisplayBoard();
        cout << "Boss, that was an invalid move. Give it another try!" << endl;
    }
  } while (keeprunning);
}

/*
Function to check the validity of every move made by the user
Data In: turn, starting_row, starting_col, final_row, final_col, most_recent_capture, move_form
Data Out: True/False based on valdiity
*/
bool MoveValidity(int turn, int starting_row, int starting_col, int final_row, int final_col, int & most_recent_capture, int & move_form) {
  char player, temp;
  int d, d_row, d_col, row_dir, col_dir, mult, r, c, enemy_row, enemy_col, row_copy, col_copy;
  bool isCapture, isMove;
  bool truth_value = false;
  bool isSpaceBtw = true;
  bool hasEnemy;
  //checks which player we are on and whether the piece is king or not
  //checks the turn value and whether the symbol is uppercase
  if (((turn % 2) == 0) && Board[starting_row][starting_col] == 'b') {
    player = 'b';
  } else if (((turn % 2) == 0) && Board[starting_row][starting_col] == 'B') {
    player = 'B';
  } else if (((turn % 2) != 0) && Board[starting_row][starting_col] == 'a') {
    player = 'a';
  } else if (((turn % 2) != 0) && Board[starting_row][starting_col] == 'A') {
    player = 'A';
  } else {
    return false;
  }
  //if the starting position chosen does not belong to the player in the first place
  //, return false immediately 
  if (Board[starting_row][starting_col] != player) {
    return false;
  }
  //calculate the difference in rows and columns
  d_row = final_row - starting_row;
  d_col = final_col - starting_col;

//checks if horizontal and vertical movement are the same
  if (abs(d_row) != abs(d_col)) {
    return false;
  }

  //if it was player a and is an ordinary piece
  if (player == 'a') {
    //check if it is simply a move and not a capture
    //by checking if the change in row/col is just 1
    if ((abs(d_row) == 1) && (abs(d_col) == 1)) {
      //Since in checkers, if you can make a capture, you must make it, this function checks whether you can make a capture from that position
      if (AnotherMovePossible(turn, starting_row, starting_col, most_recent_capture)) {
        return false;
      }
      //cross check with the array containing valid moves in the 4 directions
      //see if this move matches any one valid direction
      for (int d = 0; d < 4; d = d + 3) {
        if (d_row == MoveRow[d] && d_col == MoveCol[d]) {
          truth_value = true;
        }
      }
      //if the move form was valid, swap the pieces
      if (truth_value) {
        if (Board[final_row][final_col] == '#') {
          temp = Board[starting_row][starting_col];
          Board[final_row][final_col] = temp;
          Board[starting_row][starting_col] = '#';
          //only if the moveform is 2, it will check if another move can be made
          //since you only have the opportunity to make another capture if the original move was a capture
          move_form = 1;
          return true;
        } else {
          //if the final position was not free to move to, return false
          return false;
        }
      } else {
        //if the direction was invalid, return false immediately
        return false;
      }
      return false;
    }

    //checks if the 2x2 move is in a valid direction
    //doubles each value in the array and compares to the difference
    if ((abs(d_row) == 2) && (abs(d_col) == 2)) {
      for (int d = 0; d < 4; ++d) {
        if ((d_row == (MoveRow[d] * 2)) && (d_col == MoveCol[d] * 2)) {
          truth_value = true;
        }
      }
      //if the move direction was valid, make the cut and swap
      if (truth_value) {
        if (Board[final_row][final_col] == '#') {
          if ((toupper(Board[starting_row + (d_row / 2)][starting_col + (d_col / 2)])) == 'B') {
            temp = Board[starting_row][starting_col];
            Board[final_row][final_col] = temp;
            //piece between starting and destination will contain the enemy
            Board[starting_row + (d_row / 2)][starting_col + (d_col / 2)] = '#';
            Board[starting_row][starting_col] = '#';
            a_score = a_score + 1;
            move_form = 2;
            return true;
          } else {
            return false;
          }
        } else {
          return false;
        }
      } else {
        return false;
      }
      return false;
    }
    return false;
  }
  //same logic of checking for opposite player's ordinary piece
  //only difference is checking if movement is only forward
  if (player == 'b') {
    //checks whether direction is valid
    if (abs(d_row) == 1 && abs(d_col) == 1) {
      //checks if a capture could have been made
      if (AnotherMovePossible(turn, starting_row, starting_col, most_recent_capture)) {
        return false;
      }
      //only loops through the first two directions
      for (int d = 1; d <= 2; ++d) {
        if (d_row == MoveRow[d] && d_col == MoveCol[d]) {
          truth_value = true;
        }
      }
      if (truth_value) {
        if (Board[final_row][final_col] == '#') {
          temp = Board[starting_row][starting_col];
          Board[final_row][final_col] = temp;
          Board[starting_row][starting_col] = '#';
          move_form = 1;
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
      return false;
    }
    if (abs(d_row) == 2 && abs(d_col) == 2) {
      for (int d = 0; d < 4; ++d) {
        if ((d_row == (MoveRow[d] * 2)) && (d_col == MoveCol[d] * 2)) {
          truth_value = true;
        }
      }
      if (truth_value) {
        if (Board[final_row][final_col] == '#') {
          if ((toupper(Board[starting_row + (d_row / 2)][starting_col + (d_col / 2)])) == 'A') {
            temp = Board[starting_row][starting_col];
            Board[final_row][final_col] = temp;
            Board[starting_row + (d_row / 2)][starting_col + (d_col / 2)] = '#';
            Board[starting_row][starting_col] = '#';
            b_score = b_score + 1;
            move_form = 2;
            return true;
          } else {
            return false;
          }
        } else {
          return false;
        }
      } else {
        return false;
      }
      return false;
    }
    return false;
  }

  if (player == 'A') {
    //makes a copy of the row and column to use for cutting later
    //since i will be manipulating the original variables
    row_copy = final_row;
    col_copy = final_col;
    //to hold the magnitude of the movement horizontally and vertically
    //for example, if d_row and d_col = 6, then, mult = 6.
    mult = 0;
    //determines the direction in which the piece moves
    //by looping through the different multiples and values in the array
    for (int j = 0; j < 4; ++j) {
      for (int i = 1; i < 10; ++i) {
        if (((d_row == (MoveRow[j] * i)) && (d_col == (MoveCol[j] * i)))) {
          d = j;
          break;
        }
      }
    }

    //checks if the final position is empty to move to
    if (Board[final_row][final_col] != '#') {
      return false;
    }
    //loops through the empty spaces until it finds the enemy piece
    while ((Board[final_row][final_col] == '#') && (toupper(Board[final_row][final_col]) != 'B')) {
      final_row = final_row - MoveRow[d];
      final_col = final_col - MoveCol[d];
      mult = mult + 1;
    }
    //backtrack to previous position as the while loop stops at the exact position of B
    final_row = final_row + MoveRow[d];
    final_col = final_col + MoveCol[d];
    //if the position landed on contains the enemy, then the enemy has been found
    hasEnemy = ((toupper(Board[final_row - MoveRow[d]][final_col - MoveCol[d]])) == 'B');
  }

  if (player == 'B') {
    row_copy = final_row;
    col_copy = final_col;
    mult = 0;

    for (int j = 0; j < 4; ++j) {
      for (int i = 1; i < 10; ++i) {
        if (((d_row == (MoveRow[j] * i)) && (d_col == (MoveCol[j] * i)))) {
          d = j;
          break;
        }
      }
    }
    if (Board[final_row][final_col] != '#') {
      return false;
    }
    while ((Board[final_row][final_col] == '#') && (toupper(Board[final_row][final_col]) != 'A')) {
      final_row = final_row - MoveRow[d];
      final_col = final_col - MoveCol[d];
      mult = mult + 1;
    }
    final_row = final_row + MoveRow[d];
    final_col = final_col + MoveCol[d];
    hasEnemy = (toupper(Board[final_row - MoveRow[d]][final_col - MoveCol[d]]) == 'A');
  }

  //this stores the direction of the previous king's capture
  //to prevent the player from jumping the same piece again
  if ((move_form == 2) && (d == most_recent_capture)) {
    return false;
  }

  //calculate the position of the enemy to use for cutting later 
  enemy_row = final_row - MoveRow[d];
  enemy_col = final_col - MoveCol[d];

  if (hasEnemy) {
    //make temporary values that return to the original position of the enemy
    r = final_row - (2 * MoveRow[d]);
    c = final_col - (2 * MoveCol[d]);
    //if the move is a capture
    if (abs(d_row) >= 2) {
      //loop back to the starting position, running through the empty space
      while ((r != starting_row) && (c != starting_col)) {
        if (Board[r][c] != '#') {
          isSpaceBtw = false;
          break;
        }
        r = r - MoveRow[d];
        c = c - MoveCol[d];
      }
    }
  } else {
    //if no enemy was found, loop back to the starting position
    while ((Board[final_row][final_col] == '#') && (final_row != starting_row) && (final_col != starting_col)) {
      //makes sure that there is nothing but spaces in between
      if (Board[final_row][final_col] != '#') {
        isSpaceBtw = false;
      }
      final_row -= MoveRow[d];
      final_col -= MoveCol[d];
      //if there is no enemy and there are only spaces, then it's a valid move
    }
  }
    isMove = (!(hasEnemy) && isSpaceBtw);
  //if there is an enemy and there is space between, then it's a capture
  isCapture = (hasEnemy && isSpaceBtw);

  //make the cut if it's a capture
  if (isCapture) {
    temp = Board[starting_row][starting_col];
    Board[starting_row][starting_col] = '#';
    Board[enemy_row][enemy_col] = '#';
    Board[row_copy][col_copy] = temp;
    //increment the score
    a_score = a_score + 1;
    //move form indicates whether it's a move
    //or capture for checking later
    move_form = 2;
    for (int i = 0; i < 4; ++i) {
      if (((-MoveRow[d]) == MoveRow[i]) &&
        ((-MoveCol[d]) == MoveCol[i])) {
        most_recent_capture = i;
      }
    }
    return true;
  }

  if (isMove) {
    //checks whether a capture could have been made
    //if it could have been made, a move is illegal
    if (AnotherMovePossible(turn, starting_row, starting_col, most_recent_capture)) {
      return false;
    }
    //do the swapping of pieces
    temp = Board[starting_row][starting_col];
    Board[row_copy][col_copy] = temp;
    Board[starting_row][starting_col] = '#';
    //stores the type of the move as a move
    //so the game function won't check if another capture can be made
    move_form = 1;
    return true;
  }
  return false;
}


/*
Function to convert string input into row and column position
By subtracting each character in the string input by the ASCII value of 1 and a
Data In: coordinates (varies based on type of input)
Data Out: row's index, column's index
*/
void ReturnPositions(string coordinates, int & i, int & j) {
  //Subtract ASCII value to extract row and column on array
  //Since passed by reference, updates the variables automatically
  if (coordinates[1] == '1' && coordinates[2] == '0') {
    i = 9;
  } else {
    i = coordinates[1] - '1';
  }
  j = coordinates[0] - 'a';
  return;
}

/*
Checks if another move can be made
Data In: coordinates (turn, final_row, final_col, most_recent_capture)
Data Out: true/false (based on whether another capture can be made from final position)
*/
bool AnotherMovePossible(int turn, int final_row, int final_col, int most_recent_capture) {
  char player_piece;
  bool flag1, flag2;
  flag1 = flag2 = false;

  //based on turn value and whether symbol is upper case, determines the player
  if (((turn % 2) == 0) && Board[final_row][final_col] == 'b') {
    player_piece = 'b';
  }
  if (((turn % 2) == 0) && Board[final_row][final_col] == 'B') {
    player_piece = 'B';
  }
  if (((turn % 2) != 0) && Board[final_row][final_col] == 'a') {
    player_piece = 'a';
  }
  if (((turn % 2) != 0) && Board[final_row][final_col] == 'A') {
    player_piece = 'A';
  }

  //if the piece is normal, just check diagonally in all 4 directions to check if
  //piece directly diagonal has an enemy and the piece after is empty
  if (player_piece == 'a') {
    if ((Board[final_row + 2][final_col + 2] == '#') && ((toupper(Board[final_row + 1][final_col + 1])) == 'B')) {
      return true;
    }
    if ((Board[final_row + 2][final_col - 2] == '#') && ((toupper(Board[final_row + 1][final_col - 1])) == 'B')) {
      return true;
    }
    if ((Board[final_row - 2][final_col + 2] == '#') && ((toupper(Board[final_row - 1][final_col + 1])) == 'B')) {
      return true;
    }
    if ((Board[final_row - 2][final_col - 2] == '#') && ((toupper(Board[final_row - 1][final_col - 1])) == 'B')) {
      return true;
    }
  }
  if (player_piece == 'b') {
    if ((Board[final_row + 2][final_col + 2] == '#') && ((toupper(Board[final_row + 1][final_col + 1])) == 'A')) {
      return true;
    }
    if ((Board[final_row + 2][final_col - 2] == '#') && ((toupper(Board[final_row + 1][final_col - 1])) == 'A')) {
      return true;
    }
    if ((Board[final_row - 2][final_col + 2] == '#') && ((toupper(Board[final_row - 1][final_col + 1])) == 'A')) {
      return true;
    }
    if ((Board[final_row - 2][final_col - 2] == '#') && ((toupper(Board[final_row - 1][final_col - 1])) == 'A')) {
      return true;
    }
  }

//for crown pieces
  if (player_piece == 'A') {
    int s_row = final_row;
    int s_col = final_col;
    bool enemyfound;
    //loop through all 4 directions possible using MoveRow and MoveCol
    for (int d = 0; d < 4; ++d) {
      //reset the flags to false
      //use for breaking out of loops later
      flag1 = flag2 = false;
      //new starting row and column for next capture is old final row and column
      s_row = final_row;
      s_col = final_col;
      enemyfound = false;
      //king_capture_dir holds direction of most recent capture
      //to make sure same piece is not jumped again
      if (d == most_recent_capture) {
        continue;
      }
      //initially check whether simple capture can be made (like an ordinary piece captures)
      if ((toupper(Board[s_row + MoveRow[d]][s_col + MoveCol[d]])) == 'B') {
        if (Board[s_row + (2 * MoveRow[d])][s_col + (2 * MoveCol[d])] == '#') {
          return true;
        } else {
          flag1 = true;
        }
      }
      //in 2x2, if no empty space appears anywhere after the move, then that direction won't work
      if (flag1) {
        //skip to next iteration of loop
        continue;
      }

      //Moves one step diagonally for checking
      s_row += MoveRow[d];
      s_col += MoveCol[d];
      //If the index is out of bounds, we stop checking in this direction and move to next direction
      if ((s_row > 9) || (s_row < 0) || (s_col > 9) || (s_col < 0)) {
        continue;
      }
      //Looping through whitespace that exists between player piece and enemy piece
      while ((Board[s_row][s_col] == '#') && ((toupper(Board[s_row][s_col]) != 'B'))) {
        s_row += MoveRow[d];
        s_col += MoveCol[d];
        //Checks if index is out of bounds, skips straight to end and stops checking if it does
        if ((s_row > 9) || (s_row < 0) || (s_col > 9) || (s_col < 0)) {
          flag2 = true;
          break;
        }
        //checks whether the position moved to contains the enemy piece
        if ((toupper(Board[s_row][s_col])) == 'B') {
          enemyfound = true;
        }
      }
      //goes to next direction if it went out of bounds during checking
      if (flag2) {
        continue;
      }
      //if the enemy piece was actually found, then check if the next piece is a space that can be moved to
      if (enemyfound == true) {
        if (Board[s_row + MoveRow[d]][s_col + MoveCol[d]] == '#') {
          return true;
        }
      } else {
        continue;
      }
    }
    return false;
  }

  //same logic for the other player's crown piece
  if (player_piece == 'B') {
    int s_row = final_row;
    int s_col = final_col;
    bool enemyfound;

    //loop through all 4 directions possible using MoveRow and MoveCol
    for (int d = 0; d < 4; ++d) {
      flag1 = flag2 = false;
      s_row = final_row;
      s_col = final_col;
      enemyfound = false;
      if (d == most_recent_capture) {
        continue;
      }
      if ((toupper(Board[s_row + MoveRow[d]][s_col + MoveCol[d]])) == 'A') {
        if (Board[s_row + (2 * MoveRow[d])][s_col + (2 * MoveCol[d])] == '#') {
          return true;
        } else {
          flag1 = true;
        }
      }
      if (flag1) {
        continue;
      }
      s_row += MoveRow[d];
      s_col += MoveCol[d];
      if ((s_row > 9) || (s_row < 0) || (s_col > 9) || (s_col < 0)) {
        continue;
      }
      while ((Board[s_row][s_col] == '#') && ((toupper(Board[s_row][s_col]) != 'A'))) {
        s_row += MoveRow[d];
        s_col += MoveCol[d];
        if ((s_row > 9) || (s_row < 0) || (s_col > 9) || (s_col < 0)) {
          flag2 = true;
          break;
        }
        if ((toupper(Board[s_row][s_col])) == 'A') {
          enemyfound = true;
        }
      }
      if (flag2) {
        continue;
      }
      if (enemyfound == true) {
        if (Board[s_row + MoveRow[d]][s_col + MoveCol[d]] == '#') {
          return true;
        }
      } else {
        continue;
      }
    }
    return false;
  }
  return false;
}

/*
Function to save to a text file
Data In: Board (Global), turn
Data Out: Printed to Text File
*/
void SaveToFile(int turn) {
  char player;
  //opens the file stream and the file
  ofstream WriteProgress;
  WriteProgress.open("progress.txt");
  //determines whose turn by checking whether turn value is odd or even
  if ((turn % 2) == 0) {
    player = 'b';
  } else {
    player = 'a';
  }
  WriteProgress << "Next Player: " << player << endl;
  //if the row is even, then must add the hyphen at the start of the line
  //because of format that was stated
  for (int i = 0; i <= 9; ++i) {
    if (i % 2 == 0) {
      WriteProgress << "-" << Board[i][1] << Board[i][2] << Board[i][3] <<
        Board[i][4] << Board[i][5] << Board[i][6] << Board[i][7] << Board[i][8] <<
        Board[i][9] << endl;
    } else {
      WriteProgress << Board[i][0] << Board[i][1] << Board[i][2] << Board[i][3] <<
        Board[i][4] << Board[i][5] << Board[i][6] << Board[i][7] << Board[i][8] <<
        Board[i][9] << endl;
    }
  }

  //load the scores of both players inside the file
  WriteProgress << a_score << endl;
  WriteProgress << b_score << endl;

  WriteProgress.close();
  return;
}

/*
Function to read data from the file and update the Board, the score and the turn
Data In: Board (Global), turn
Data Out: Board updated
*/
void GetFromFile(int & turn) {
  char player;
  char space;
  string turn_string;
  char piece;
  ifstream GetProgress;
  GetProgress.open("progress.txt");
  if (GetProgress.fail()) {
    cout << "For some reason, we are unable to locate the progress file, so" << "\n" <<
      "we'll load a new board!" << endl;
    return;
  }
  //Load whose turn into turn variable
  getline(GetProgress, turn_string);
  //the actual player is in the 13th position of the first line 
  player = turn_string[13];
  if (player == 'b') {
    turn = 2;
  } else {
    turn = 1;
  }

  //Load progress into array
  while (!GetProgress.eof()) {
    for (int i = 0; i <= 9; ++i) {
      //if the row number is even, loading will start from the second column
      //load the hyphen into a variable to skip over it
      if (i % 2 == 0) {
        GetProgress >> space >> Board[i][1] >> Board[i][3] >>
          Board[i][5] >> Board[i][7] >> Board[i][9];
        for (int k = 0; k <= 8; k = k + 2) {
          //manually load in whitespace as c++ ignores whitespace in a file
          Board[i][k] = ' ';
        }
      } else {
        //if the row number is odd, loading will start from the first column
        GetProgress >> Board[i][0] >> Board[i][2] >>
          Board[i][4] >> Board[i][6] >> Board[i][8];
        for (int j = 1; j <= 9; j = j + 2) {
          //manually load in whitespace as c++ ignores whitespace in a file
          Board[i][j] = ' ';
        }
      }
    }
    //get the scores from the file
    GetProgress >> a_score;
    GetProgress >> b_score;
  }
  cout << "I've updated the board with your past progress!" << endl << endl;
  return;
}

/*
Function that determines if the game should end
Checks if one player has no more pieces left
Data In: Board (Global)
Data Out: winner, true/false (whether there is a winner)
*/
bool WhoWins() {
  int a_pieces = 0;
  int b_pieces = 0;
  bool truth;
  //loops through every position
  //and count the number of each player's pieces
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      if (toupper(Board[i][j]) == 'A') {
        a_pieces = a_pieces + 1;
      }
      if (toupper(Board[i][j]) == 'B') {
        b_pieces = b_pieces + 1;
      }
    }
  }
  if (a_pieces == 0) {
    truth = true;
    winner = 'b';
  }
  if (b_pieces == 0) {
    truth = true;
    winner = 'a';
  }
  //return whether there is a winner at that point
  return truth;
}
