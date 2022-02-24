#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
//#include <sstream>
#include<windows.h>
#include <cmath>
#include <queue>  // Needed for BFS
#include<array>
//#include<iterator>
#include <utility>  // Needed for BFS
#include <algorithm>  // Needed for BFS

// do not forget to delete unuseed libraries after finishing the project. 
// (oh god, it would be a mess..)

using namespace std;



/*
  
                //////////////////////////////////////////////////////////
           ///////////////////                           ///////////////////
      ///////////////////           GAME VERSION 1.1         ///////////////////
           ///////////////////                           ///////////////////
                //////////////////////////////////////////////////////////

    *NOTE: mark code lines in question with "// TO-DO LITS ENTRY n" if necessary
    
            TO-DO LIST:
    
    1. fix bots (they sometimes cause signal 11) ( probably fixed )
        1.1. upgrade bots.
    
    2. implement statistics and post-game analisys.
    
    3. either implement window interface or fix russian cmd output. ( fixed russian but WTF??! )
        3.1. interface prefered.
    
    4. replace "id - 2" workaround with "id" by shifting ids of GameBoard.  (might not be 
        a good idea at all due to wall and emty ids becoming fluid due to evershifting team size)
    
    5. make most of character class values const as they should be
    
*/




int GameBoard[8][12];  // Array representing map of the game
const string AllianceNameList[6] = {
        "огонь", "земля", "вода",
        "свет", "тьма", "пыль"
    };  // Names Alliances per id.
int TurnOrderTeam1List[4];  // Order of turn for team 1
int TurnOrderTeam2List[4];  // Order of turn for team 2
bool Team1Turn;  // If true first team's turn, otherwise second team's turn
int Team1TurnCounter;  // If team 1 goes this turn, which of it's members goes
int Team2TurnCounter;  // If team 2 goes this turn, which of it's members goes
int Team2AI;  // sets an AI for the second team if needed

int Damage[8];  // damage in current turn dealt to characters per Id (kinda workaround).

// ANALISYS TOOLS

class aCharData {
public:
    //int Id;
    int Y;
    int X;
    int Hp;
};
int aStartField[8][12];
int aLastField[8][12];
typedef array<aCharData, 8> aTurnData;
aTurnData aStartData;
vector<aTurnData> aGameData;

// END OF ANALISYS TOOLS

/*struct PosYX {
    // WIP
    // represents coordinates of square on GameBoard
    int Y;
    int X;
};*/


class character {
public:
    int Id;
    string Team;
    int AllianceId;
    string Name;
    int Hp;
    int Steps;
    int MeleeDmg;
    int RangeDmg;
    int RangeDistance;
};

float FixFPError(float val) {
    // This is a workaround to handle floating point errors near integer values
    if(val == floorf(val)) return val;
    if(val - 0.0001 < floorf(val)) return floorf(val);
    if(val + 0.0001 > ceilf(val)) return ceilf(val);
    return val;
}

character BattleMembers[8];  // Array representing every character in current game

int YofGameBoardById(int seek) {
    // This returns Y coordinate of given id on the map.
    // if multiple ids found, might not work properly
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 12; x++) {
            if (::GameBoard[y][x] == seek) {
                return y;
            }
        }
    }
    cout << endl;
    cout << "\nError in YofGameBoardById():" << endl;
    cout << "  Index \"" << seek << "\"not found" << endl;
    cout << endl;
    return -1;
}

int XofGameBoardById(int seek) {
    // This returns X coordinate of given id on the map.
    // if multiple ids found, might not work properly
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 12; x++) {
            if (::GameBoard[y][x] == seek) {
                return x;
            }
        }
    }
    cout << endl;
    cout << "\nError in XofGameBoardById():" << endl;
    cout << "  Index \"" << seek << "\"not found" << endl;
    cout << endl;
    return -1;
}

void BootData() {
    // This loads all neccessary data before game starts
    
    
    while(true) {
        cout << "ВКЛЮЧИТЬ БОТОВ?\n1.да\n2.нет\n";
        string tinp;
        cin >> tinp;
        if (
            tinp == "" 
            || (tinp[0] != '1' && tinp[0] != '2')
        ) {
            cout << "неправильный ввод, попробуй заново.\n\n";
            continue;
        }
        else if (tinp[0] == '1') {
            ::Team2AI = 1;  // default bots
            cout << "БОТЫ ВКЛЮЧЕНЫ.\n\n";
            break;
        }
        else {
            ::Team2AI = 0;  // local human player
            cout << "БОТЫ ОТКЛЮЧЕНЫ.\n\n";
            break;
        }
    }
    ::Team1Turn = true;
    ::Team1TurnCounter = 0;
    ::Team2TurnCounter = 0;
    
    const int foo[4] = { 2, 3, 4, 5 };
    for(int i = 0; i < 4; i++) {
        ::TurnOrderTeam1List[i] = foo[i]; 
    }
    
    const int bar[4] = { 6, 7, 8, 9 };
    for(int i = 0; i < 4; i++) {
        ::TurnOrderTeam2List[i] = bar[i]; 
    }
    
    const int baz[8][12] = {
        {1, 1, 1, 1, 2, 3, 4, 5, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 6, 7, 8, 9, 1, 1, 1, 1}
    };
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 12; j++) {
            int cBaz = baz[i][j];  // current index
            ::GameBoard[i][j] = cBaz;
            if(cBaz != 0 && cBaz != 1) {
                ::aStartField[i][j] = 0;  // ::aStartField is an empty field, it does not have characters.
            } else {
                ::aStartField[i][j] = cBaz;
            }
        }
    }
    /* 
    0 - emty space 
    1 - wall
    2-9 - characters
    */
    
    const string Team1Name = "ghouls";
    const string Team2Name = "shizy";
    const int AllianceIdList[8] = {
        0, 1, 2, 3,
        4, 5, 0, 1
    };
    const string CharacterNames[8] = {
        "Tr", "Sl", "Ne", "Us",
        "An", "It", "Ru", "Ky"
    }; const int HpList[8] = {
        30, 30, 30, 30,
        30, 30, 30, 30
    }; const int StepsList[8] = {
        5, 5, 5, 5,
        5, 5, 5, 5
    }; const int MeleeDmgList[8] = {
        6, 6, 6, 6,
        6, 6, 6, 6
    }; const int RangeDmgList[8] = {
        5, 5, 5, 5,
        5, 5, 5, 5
    }; const int RangeDistanceList[8] = {
        3, 3, 3, 3,
        3, 3, 3, 3
    };
    int qux;
    for(int i = 2; i <= 9; i++) {
        qux = i - 2;
        ::BattleMembers[qux].Id = i;
        ::BattleMembers[qux].Team = (i <= 5) ? Team1Name : Team2Name;
        ::BattleMembers[qux].AllianceId = AllianceIdList[qux];
        ::BattleMembers[qux].Name = CharacterNames[qux];
        ::BattleMembers[qux].Hp = HpList[qux];
        ::BattleMembers[qux].Steps = StepsList[qux];
        ::BattleMembers[qux].MeleeDmg = MeleeDmgList[qux];
        ::BattleMembers[qux].RangeDmg = RangeDmgList[qux];
        ::BattleMembers[qux].RangeDistance = RangeDistanceList[qux];
        
        ::aStartData[qux].Hp = HpList[qux];
        ::aStartData[qux].X = XofGameBoardById(i);
        ::aStartData[qux].Y = YofGameBoardById(i);
    }
}

character GetCharacterByTurn() {
    // This returns character who's turn is now
    int Id;
    if(::Team1Turn) {
        Id = ::TurnOrderTeam1List[::Team1TurnCounter];
    } else {
        Id = ::TurnOrderTeam2List[::Team2TurnCounter];
    }
    return BattleMembers[Id - 2];
}

/*PosYX YXofGameBoardById(int Id) {
    // WIP
    // This returns Y and X coordinate of given id on the map.
    // if multiple ids found, might not work properly
    PosYX Square;
    Square.X = -1;
    Square.Y = -1;
    for (int y = 0; y < 8; y++) {
        for(int x = 0; x < 12; x++) {
            if (::GameBoard[y][x] == seek) {
                Square.Y = y;
                Square.X = x;
                return Square;
            }
        }
    }
    cout << endl;
    cout << "\nError in XofGameBoardById():" << endl;
    cout << "  Index \"" << seek << "\"not found" << endl;
    cout << endl;
    return Square;
}*/

void PrintGameBoard() {
    // prints map of the game into cmd
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 12; j++) {
            int foo = ::GameBoard[i][j];
            switch(foo){
                case 0:
                    cout << "__";
                    break;
                case 1:
                    cout << "##";
                    break;
                case 2:
                    //cout << "f1";
                    //break;
                case 3:
                    //cout << "f2";
                    //break;
                case 4:
                    //cout << "f3";
                    //break;
                case 5:
                    //cout << "f4";
                    //break;
                case 6:
                    //cout << "b1";
                    //break;
                case 7:
                    //cout << "b2";
                    //break;
                case 8:
                    //cout << "b3";
                    //break;
                case 9:
                    //cout << "b4";
                    if(BattleMembers[foo - 2].Hp > 0) { 
                        cout << BattleMembers[foo - 2].Name;
                    } else {
                        cout << "__";
                    }
                    break;
                default:
                    cout << "\nERROR: unregistred Id = " << foo << " in GameBoard[" << i << "][" << j << "]" << endl;
            }
            cout << "  ";
        }
        cout << endl;
    }
}

void aPrintGameBoard(aTurnData c) {
    int hp[8], x[8], y[8];
    int prCharacter;
    for(int i = 0; i < 8; i++) {
        hp[i] = c[i].Hp;
        x[i] = c[i].X;
        y[i] = c[i].Y;
    }
    int foo;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 12; j++) {
            for(int k = 0; k < 8; k++) {
                prCharacter = 0;
                if(y[k] == i && x[k] == j && hp[k] > 0) {
                    prCharacter = k + 2;
                    break;
                }
            }
            if (prCharacter == 0) {
                foo = ::aStartField[i][j];
            } else {
                foo = prCharacter;
            }
            switch(foo){
                case 0:
                    cout << "__";
                    break;
                case 1:
                    cout << "##";
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    cout << BattleMembers[foo - 2].Name;
                default:
                    cout << "\nerror in aPrintGameBoard(): unregistred Id = " << foo << " in aStartField[" << i << "][" << j << "]" << endl;
            }
            cout << "  ";
        }
        cout << endl;
    }
}

void PrintTurnOrder() {
    // Prints turn order for next 8 turns including current
    
    // Copying counters to prevent accidentaly changing their value
    bool Counter = ::Team1Turn;
    int Counter1 = ::Team1TurnCounter;
    int Counter2 = ::Team2TurnCounter;
    
    int guy1Id;  // person from team 1
    int guy2Id;  // person from team 2
    cout << "Порядок хода: ";
    for(int i = 0; i < 8; i++) {
        if(Counter) {
            guy1Id = ::TurnOrderTeam1List[Counter1];
            cout << ::BattleMembers[guy1Id - 2].Name << " ";
            Counter1 = (Counter1 + 1) % 4;
            while(true) {
                guy2Id = ::TurnOrderTeam2List[Counter2];
                if(::BattleMembers[guy2Id - 2].Hp <= 0) {
                    Counter2 = (Counter2 + 1) % 4;
                    i++;
                } else {
                    break;
                }
            }
        } else {
            guy2Id = ::TurnOrderTeam2List[Counter2];
            cout << ::BattleMembers[guy2Id - 2].Name << " ";
            Counter2 = (Counter2 + 1) % 4;
            while(true) {
                guy1Id = ::TurnOrderTeam1List[Counter1];
                if(::BattleMembers[guy1Id - 2].Hp <= 0) {
                    Counter1 = (Counter1 + 1) % 4;
                    i++;
                } else {
                    break;
                }
            }
        }
        Counter = !Counter;
    }
    cout << endl;
    cout << endl;
}

void aDisplayTurn(int turn) {
    aTurnData c;  // data to display
    if(turn < 0 || turn > ::aGameData.size()) {
        cout << "error in aDisplayTurn(): invalid turn " << turn << endl;
        return;
    } else if(turn == 0) {  // turn 0 is the start turn, not ::aGameData[0].
        c = ::aStartData;
    } else {
        c = ::aGameData[turn - 1];
    }
    aPrintGameBoard(c);
}

void PrintTurn() {
    // This prints stats of character who goes this turn
    character ThisCharacter = GetCharacterByTurn();
    cout << "ход команды " << ThisCharacter.Team << endl;
    cout << " ход игрока " << ThisCharacter.Name << endl;
    cout << "  Альянс: " << AllianceNameList[ThisCharacter.AllianceId] << endl;
    cout << "  Хп: " << ThisCharacter.Hp << endl;
    cout << "  Шагов за ход: " << ThisCharacter.Steps << endl;
    cout << "  Ближний урон: " << ThisCharacter.MeleeDmg << endl;
    cout << "  Дальний урон: " << ThisCharacter.RangeDmg << endl;
    cout << "  Дальность: " << ThisCharacter.RangeDistance << endl;
    cout << endl;
}

void PrintStats() {
    // This prints short stats of every character in current game
    for(int i = 0; i < 8; i++){
        if(::BattleMembers[i].Hp > 0) {
            cout << "Статистика игрока  " << ::BattleMembers[i].Name << " команды " << ::BattleMembers[i].Team << endl;
            cout << "  Альянс: " << AllianceNameList[::BattleMembers[i].AllianceId] << endl;
            cout << "  Хп: " << ::BattleMembers[i].Hp << endl;
            // Some stats are omited for the sake of clean interface.
            // full stats can be accessed by using /s command
            //cout << "  Steps per turn: " << ::BattleMembers[i].Steps << endl;
            //cout << "  Melee Dmg: " << ::BattleMembers[i].MeleeDmg << endl;
            //cout << "  Range Dmg: " << ::BattleMembers[i].RangeDmg << endl;
            //cout << "  Range: " << ::BattleMembers[i].RangeDistance << endl;
            cout << endl;
        } else {
            cout << "Игрок " << ::BattleMembers[i].Name << " команды " << ::BattleMembers[i].Team << " мертв." << endl;
            cout << endl;
        }
    }
}

int TeamVictory() {
    /*
    This checks if any team won.
    
    return values:
    0 - game not yet ended
    1 - team 1 won
    2 - team 2 won
    */
    if(
            ::BattleMembers[0].Hp <= 0
        &&
            ::BattleMembers[1].Hp <= 0
        &&
            ::BattleMembers[2].Hp <= 0
        &&
            ::BattleMembers[3].Hp <= 0
    ) {
        cout << "Команда " << ::BattleMembers[4].Team << " победила!" << endl;
        return 2;
    } else if(
            ::BattleMembers[4].Hp <= 0
        &&
            ::BattleMembers[5].Hp <= 0
        &&
            ::BattleMembers[6].Hp <= 0
        &&
            ::BattleMembers[7].Hp <= 0
    ) {
        cout << "Команда " << ::BattleMembers[0].Team << " победила!" << endl;
        return 1;
    } else {
        return 0;
    }
}

void PrintAllStats() {
    // This prints stats of every character in current game.
    // Unlike PrintStats(), this does not omit any of the stats
    for(int i = 0; i < 8; i++){
        if(::BattleMembers[i].Hp > 0) {
            cout << "Полная статистика игрока  " << ::BattleMembers[i].Name << " команды " << ::BattleMembers[i].Team << endl;
            cout << "  Альянс: " << AllianceNameList[::BattleMembers[i].AllianceId] << endl;
            cout << "  Хп: " << ::BattleMembers[i].Hp << endl;
            cout << "  Шагов за ход: " << ::BattleMembers[i].Steps << endl;
            cout << "  Ближний урон: " << ::BattleMembers[i].MeleeDmg << endl;
            cout << "  Дальний урон: " << ::BattleMembers[i].RangeDmg << endl;
            cout << "  Дальность: " << ::BattleMembers[i].RangeDistance << endl;
            cout << endl;
        } else {
            cout << "Игрок " << ::BattleMembers[i].Name << " команды " << ::BattleMembers[i].Team << " мертв." << endl;
            cout << endl;
        }
    }
}

bool TempWTF(int Temp69, int ThisCharacter_Id) {
    // This is micro-sub-fuction of ProcessInput, used in 'q' branch.
    return Temp69 != 0 
    && Temp69 != ThisCharacter_Id 
    && ::BattleMembers[Temp69 - 2].Hp - ::Damage[Temp69 - 2] > 0;
}

int IsValidMovement(
    char Move,
    int ThisCharacter_Id,
    int CurrentY,
    int CurrentX
) {
    /* 
    This is sub-function of ProcessInput(), used if move in queston is
    'w', 'a', 's' or 'd', to check if making this move will break the 
    rules of the game.
    
    
    Error codes:
    0 - success
    1 - Out of map
    2 - Walking through walls
    3 - Walking through characters
    */
    int ThisSquareId;
    
    if(CurrentY < 1 && Move == 'w') return 1;
    if(CurrentY > 6 && Move == 's') return 1;
    if(CurrentX < 1 && Move == 'a') return 1;
    if(CurrentX > 10 && Move == 'd') return 1;
    
    if(Move == 'w') ThisSquareId = ::GameBoard[CurrentY - 1][CurrentX];
    else if(Move == 's') ThisSquareId = ::GameBoard[CurrentY + 1][CurrentX];
    else if(Move == 'a') ThisSquareId = ::GameBoard[CurrentY][CurrentX - 1];
    else if(Move == 'd') ThisSquareId = ::GameBoard[CurrentY][CurrentX + 1];
    else cout << "\n\nERROR in IsValidMovement(): invalid char '" << Move << "' passed as Move.\n" << endl;
    
    if(ThisSquareId == 1) return 2;
    
    if(
            ThisSquareId != 0 
        && 
            ::BattleMembers[ThisSquareId - 2].Hp - ::Damage[ThisSquareId - 2] > 0
        &&
            ThisSquareId != ThisCharacter_Id
    ) {
        return 3;
    }
    return 0;
}

void PromptInput() {
    // This tells player most part of neccessary info about game state.
    PrintGameBoard();
    PrintStats();
    PrintTurnOrder();
    PrintTurn();
    cout << "Введи /h для помощи по командам." << endl;
    cout << "Пожалуйста, введи ход:" << endl;
}

void ProcessInput() {
    // This processes player's turn description untill can execute it,
    // then, (you guessed it), executes it.
    character ThisCharacter = GetCharacterByTurn();
    bool ErrorInInputOccured;
    bool TurnSkiped;
    while(true) {
        int UsedStepsCounter = 0;
        int CharIndexCounter = 0;
        int UnskipedChars = 0;
        ErrorInInputOccured = false; // True if turn description is incorrect
        TurnSkiped = false; // True if player chose to skip character's turn
        const int StartY = YofGameBoardById(ThisCharacter.Id);
        const int StartX = XofGameBoardById(ThisCharacter.Id);
        int ShiftY = 0;
        int ShiftX = 0;
        for(int i = 0; i < 8; i++) {
            ::Damage[i] = 0;
        }  // Damage that will be delt to a coresponding id of character in ::BattleMembers.
    
        string TurnDescription;  // whatever player typed to describe his turn.
        cin >> TurnDescription;
        cout << endl;
        for(char& c : TurnDescription) {
            // This checks if turn is valid, and, if it is, gets ready to
            // pass data to the part whrere it gets executed.
            if(ErrorInInputOccured == true || TurnSkiped == true) {
                break;
            }
            if(UsedStepsCounter > ThisCharacter.Steps) {
                cout << "Ошибка: слишком много шагов" << endl;
                cout << "Пожалуйста введи ход:" << endl;
                break;
            }
            if(UnskipedChars != CharIndexCounter) {
                UnskipedChars ++;
                continue;
            }
            int CurrentY = StartY + ShiftY;
            int CurrentX = StartX + ShiftX;
            int ThisSquareId;
            int repeatedQs = 1;
            
            
            // c - each character in turn description
            c = tolower(c);
            
            
            if(c == ' ') {
                CharIndexCounter++;
            } else if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
                // If c represents movement / skip
                if(c == 's' && TurnDescription.size() != CharIndexCounter + 1 && TurnDescription[CharIndexCounter + 1] == 'k') {
                    TurnSkiped = true;
                    break;
                }  // This if statement is responsible for skiping turn
                
                int ErrorCode = IsValidMovement(c, ThisCharacter.Id, CurrentY, CurrentX);
                if(ErrorCode == 0) {
                    if (c == 'w') ShiftY--;
                    else if (c == 's') ShiftY++;
                    else if (c == 'a') ShiftX--;
                    else if (c == 'd') ShiftX++;
                    else cout << "\n\nERROR in ProcessInput(): char '" << c << "' does not represent movement\n" << endl;
                    
                    
                    UsedStepsCounter++;
                    CharIndexCounter++;
                } else if(ErrorCode == 1) {
                    ErrorInInputOccured = true;
                    cout << "Ошибка: персонажи не могут выйти за пределы карты" << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                } else if(ErrorCode == 2) {
                    ErrorInInputOccured = true;
                    cout << "Ошибка: нет, через стены проходить нельзя" << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                } else if(ErrorCode == 3) {
                    ErrorInInputOccured = true;
                    cout << "Ошибка: нельзя проходить через других персонажей" << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                } else cout << "\n\nERROR in ProcessInput(): ErrorCode has incorrect value of " << ErrorCode << "\n" << endl;
            } else if(c == 'e') {
                // If c represents melee attack
                int IdAdjacent[4];
                if(CurrentY > 0) IdAdjacent[0] = GameBoard[CurrentY - 1][CurrentX];
                else IdAdjacent[0] = 0;
                if(CurrentY < 7) IdAdjacent[1] = GameBoard[CurrentY + 1][CurrentX];
                else IdAdjacent[1] = 0;
                if(CurrentX > 0) IdAdjacent[2] = GameBoard[CurrentY][CurrentX - 1];
                else IdAdjacent[2] = 0;
                if(CurrentX < 11) IdAdjacent[3] = GameBoard[CurrentY][CurrentX + 1];
                else IdAdjacent[3] = 0;
                for(int j = 0; j < 4; j++){
                    if(
                            IdAdjacent[j] != 0 
                        && 
                            IdAdjacent[j] != 1
                        &&
                            ThisCharacter.Team != ::BattleMembers[IdAdjacent[j] - 2].Team
                    ) {
                        int BMPos = IdAdjacent[j] - 2;
                        if(ThisCharacter.AllianceId / 3 == ::BattleMembers[BMPos].AllianceId / 3) {
                            if((ThisCharacter.AllianceId + 1) % 3 == ::BattleMembers[BMPos].AllianceId % 3) {
                                ::Damage[BMPos] += ThisCharacter.MeleeDmg + 2;
                            } else if((ThisCharacter.AllianceId - 1) % 3 == ::BattleMembers[BMPos].AllianceId % 3) {
                                ::Damage[BMPos] += ThisCharacter.MeleeDmg - 2;
                            } else {
                                ::Damage[BMPos] += ThisCharacter.MeleeDmg;
                            }
                        } else {
                            ::Damage[BMPos] += ThisCharacter.MeleeDmg;
                        }
                    }
                }
                UsedStepsCounter++;
                CharIndexCounter++;
            } else if(c == 'q') { 
                /* 
                If c represents range attack.
                
                Ok, so the first part is easy: we just need to get the id of the target
                using the name specified in turn description.
                */
                string Target = "";
                while (true) {
                    if (TurnDescription[CharIndexCounter + 1] == 'q') {
                        repeatedQs++;
                        CharIndexCounter++;
                    }
                    else break;
                }
                if(TurnDescription[CharIndexCounter + 1] != '(') {
                    ErrorInInputOccured = true;
                    cout << "Ошибка: после каждой \'q\' нужно указать имя в круглых скобках" << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                    break;
                } 
                int length = TurnDescription.length();
                int TargetSize = 0;
                for(int k = CharIndexCounter + 2; k <= length; k++) {
                    if(k == length) {
                        ErrorInInputOccured = true;
                        cout << "Ошибка: все скобки, содержащие имя, должны быть закрты" << endl;
                        cout << "Пожалуйста введи ход:" << endl;
                        break;
                    }
                    char Stuff = TurnDescription[k];
                    if(Stuff != ')') {
                        Target += Stuff;
                        TargetSize ++;
                    } else {
                        CharIndexCounter += 3 + TargetSize;
                        break;
                    }
                }
                for(char& d : Target) {
                    d = tolower(d);
                };
                int TargetId = -1;
                for(int k = 0; k <= 8; k++) {
                    if(k == 8) {
                        ErrorInInputOccured = true;
                        cout << "Ошибка: нет персонажей указанным именем" << endl;
                        cout << "Пожалуйста введи ход:" << endl;
                        break;
                    }
                    string PossibleTarget = ::BattleMembers[k].Name;
                    for(char& d : PossibleTarget) {
                        d = tolower(d);
                    };
                    if(PossibleTarget == Target) {
                        TargetId = k + 2;
                        break;
                    }
                }
                if(TargetId == ThisCharacter.Id) {
                    ErrorInInputOccured = true;
                    cout << "Ошибка: не пытайся застрелиться. Пожалуйста. Ты нам нужен." << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                    break;
                } else if(::BattleMembers[TargetId - 2].Team == ThisCharacter.Team) {
                    ErrorInInputOccured = true;
                    cout << "Ошибка: стрелять по своим - плохая идея" << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                    break;
                } else {
                    /*
                    Oh god, now goes the second part and I have literaly no idea how 
                    do you decipher what it does or what for, but it works.
                        
                    In short - we need to find out if there are any obstacles in the way, 
                    and throw an in-game error if so.
                    */
                    int TargetY = YofGameBoardById(TargetId);
                    int TargetX = XofGameBoardById(TargetId);
                    int DeltaY = TargetY - CurrentY;
                    int DeltaX = TargetX - CurrentX;
                    float Slope;
                    bool STWError = false;
                    if(abs(DeltaY) + abs(DeltaX) > ThisCharacter.RangeDistance) {
                        ErrorInInputOccured = true;
                        cout << "Ошибка: недостаточно дальности для выстрела" << endl;
                        cout << "Пожалуйста введи ход:" << endl;
                        break;
                    } else if(DeltaY == 0) {
                        int FooSign = (DeltaX > 0) ? 1 : -1;
                        int DeltaFoo = DeltaX * FooSign;
                        for(int l = 1; l < DeltaFoo; l++) {
                            ThisSquareId = GameBoard[CurrentY][CurrentX + (l * FooSign)];
                            if(TempWTF(ThisSquareId, ThisCharacter.Id)) {
                                ErrorInInputOccured = true;
                                cout << "Ошибка: нельзя стрелять через стены или других персонажей" << endl;
                                cout << "Пожалуйста введи ход:" << endl;
                                break;
                            }
                        }
                    } else if(DeltaX == 0) {
                        int FooSign = (DeltaY > 0) ? 1 : -1;
                        int DeltaFoo = DeltaY * FooSign;
                        for(int k = 1; k < DeltaFoo; k++) {
                            ThisSquareId = GameBoard[CurrentY + (k * FooSign)][CurrentX];
                            if(TempWTF(ThisSquareId, ThisCharacter.Id)) {
                                ErrorInInputOccured = true;
                                cout << "Ошибка: нельзя стрелять через стены или других персонажей" << endl;
                                cout << "Пожалуйста введи ход:" << endl;
                                break;
                            }
                        }
                    } else {
                        int XInv = (DeltaX > 0) ? 1 : -1;
                        int YInv = (DeltaY > 0) ? 1 : -1;
                        Slope = (float)DeltaX * XInv / (float)DeltaY * YInv;
                        int Temp1;
                        int Temp2;
                        int Temp3;
                        int y;
                        float x = FixFPError(0.5 + Slope / 2);
                        for(y = 1; y < DeltaY; y++) {
                            if(x == floorf(x)) {
                                if(y != 1) {
                                    if(Slope > 1) {
                                        Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                        Temp2 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                        Temp3 = GameBoard[y * YInv + CurrentY][((int)floorf(x) - 1) * XInv + CurrentX];
                                    } else if(Slope < 1) {
                                        Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentX][y * YInv + CurrentY];
                                        Temp2 = GameBoard[(int)floorf(x) * XInv + CurrentX][(y - 1) * YInv + CurrentY];
                                        Temp3 = GameBoard[((int)floorf(x) - 1) * XInv + CurrentX][y * YInv + CurrentY];
                                    }
                                    if(
                                            TempWTF(Temp1, ThisCharacter.Id)
                                        || 
                                            TempWTF(Temp2, ThisCharacter.Id)
                                        || 
                                            TempWTF(Temp3, ThisCharacter.Id)
                                    ) {
                                        STWError = true;
                                        break;
                                    }
                                } else {
                                    if(Slope > 1) {
                                        Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                        Temp2 = GameBoard[y * YInv + CurrentY][((int)floorf(x) - 1) * XInv + CurrentX];
                                    } else if(Slope < 1) {
                                        Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentX][y * YInv + CurrentY];
                                        Temp2 = GameBoard[((int)floorf(x) - 1) * XInv + CurrentX][y * YInv + CurrentY];
                                    }
                                    if( 
                                            TempWTF(Temp1, ThisCharacter.Id)
                                        || 
                                            TempWTF(Temp2, ThisCharacter.Id)
                                    ) {
                                        STWError = true;
                                        break;
                                    }
                                }
                            } else {
                                if(y != 1) {
                                    if(Slope >= 1) {
                                        Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                        Temp2 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                    } else if(Slope < 1) {
                                        Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentX][y * YInv + CurrentY];
                                        Temp2 = GameBoard[(int)floorf(x) * XInv + CurrentX][(y - 1) * YInv + CurrentY];
                                    }
                                    if( 
                                            TempWTF(Temp1, ThisCharacter.Id)
                                        || 
                                            TempWTF(Temp2, ThisCharacter.Id)
                                    ) {
                                        STWError = true;
                                        break;
                                    }
                                } else {
                                    if(Slope > 1) {
                                        Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                    } else if (Slope < 1) {
                                        Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentX][y * YInv + CurrentY];
                                    }
                                    if(TempWTF(Temp1, ThisCharacter.Id)) {
                                        STWError = true;
                                        break;
                                    }
                                }
                            }
                            x = FixFPError(Slope + x);
                        }
                        if(Slope == 1) {
                            Temp1 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                            Temp2 = GameBoard[y * YInv + CurrentY][((int)floorf(x) - 1) * XInv + CurrentX];
                            if( 
                                TempWTF(Temp1, ThisCharacter.Id)
                            || 
                                TempWTF(Temp2, ThisCharacter.Id)
                            ) {
                                STWError = true;
                                // break will cause bug.
                            } 
                        } else {
                            Temp1 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                            if(TempWTF(Temp1, ThisCharacter.Id)
                            ) {
                                STWError = true;
                                // break will cause bug.
                            }
                        }
                    }
                    if(STWError == true) {
                        ErrorInInputOccured = true;
                        cout << "Ошибка: нельзя стрелять через стены или других персонажей" << endl;
                        cout << "Пожалуйста введи ход:" << endl;
                        break;
                    }
                    int BMPos = TargetId - 2;
                    if(ThisCharacter.AllianceId / 3 == ::BattleMembers[BMPos].AllianceId / 3) {
                        if((ThisCharacter.AllianceId + 1) % 3 == ::BattleMembers[BMPos].AllianceId % 3) {
                            ::Damage[BMPos] += (ThisCharacter.RangeDmg + 2) * repeatedQs;
                        } else if((ThisCharacter.AllianceId - 1) % 3 == ::BattleMembers[BMPos].AllianceId % 3) {
                            ::Damage[BMPos] += (ThisCharacter.RangeDmg - 2) * repeatedQs;
                        } else {
                            ::Damage[BMPos] += ThisCharacter.RangeDmg * repeatedQs;
                        }
                    } else {
                        ::Damage[BMPos] += ThisCharacter.RangeDmg * repeatedQs;
                    }
                }
                UsedStepsCounter++;
            } else if(c == '/') {
                ErrorInInputOccured = true;  // Not really an error but used to prevent accidental making of turn
                
                if(CharIndexCounter != 0) {
                    // Used to prevent typos triggering commands
                    cout << "Ошибка: при вводе команд \'/\' должен быть первым символом в строке." << endl;
                    cout << "Пожалуйста введи ход:" << endl;
                    break;
                }
                
                char OrderName = tolower(TurnDescription[1]);
                const char Order1 = 'h'; // "help"
                const char Order2 = 's'; // "stats"
                const char Order3 = 'm'; // "map"
                const char Order4 = 'r'; // "rules"
                switch(OrderName) {
                    case Order1:
                        cout << "Существующие команды:" << endl;
                        cout << "/h - описывает все команды" << endl;
                        cout << "/r - кратко описывает правила ввода хода" << endl;
                        cout << "/s - показывает полную статистику всех персонажей" << endl;
                        cout << "/m - показывает карту (еще раз)" << endl;
                        
                        break;
                    case Order2:
                        PrintAllStats();
                        
                        break;
                    case Order3:
                        PrintGameBoard();
                        
                        break;
                    case Order4:
                        cout << "Чтобы сделать ход введи последовательность из букв латинского алфавита." << endl;
                        cout << "Каждая из нижеописанных из букв отвечает за отдельное действие:" << endl;
                        cout << "w - ход на одну клетку вверх" << endl;
                        cout << "s - ход на одну клетку вниз" << endl;
                        cout << "a - ход на одну клетку влево" << endl;
                        cout << "d - ход на одну клетку вправо" << endl;
                        cout << "Нельзя проходить через стены или других персонажей." << endl;
                        cout << "e - удар в ближнем бою по соседним клеткам." << endl; 
                        cout << "Ближняя атака не наносит урон по клеткам, " << endl; 
                        cout << " находящимcя по диагонали от применяющего ее персонажа." << endl;
                        cout << "q(ИМЯ) - атакует персонажа с выбранным именем в дальнем бою." << endl;
                        cout << "Дальнюю атаку нельзя совершить, если между стрелком и целью есть персонажи или стены." << endl;
                        cout << "последовательность букв \"sk\" пропускает ход." << endl;
                        cout << "За один ход нельзя совершить больше шагов (действий), " << endl;
                        cout << " чем указано в параметре \"шагов зв ход\" персонажа." << endl;
                        
                        break;
                    default:
                        cout << "Ошибка: команды /" << OrderName << " не существует." << endl;
                        
                        break;
                }
                cout << "Пожалуйста введи ход:" << endl;
                break;
            } else {
                ErrorInInputOccured = true;
                cout << "Ошибка: неизвестный символ в описании хода" << endl;
                cout << "Пожалуйста введи ход:" << endl;
                break;
            }
            UnskipedChars ++;
        }
        if (
                ErrorInInputOccured == false 
            &&
                UsedStepsCounter <= ThisCharacter.Steps
        ) {
            ::GameBoard[StartY][StartX] = 0;
            ::GameBoard[StartY + ShiftY][StartX + ShiftX] = ThisCharacter.Id;
            for(int j = 0; j < 8; j++) {
                ::BattleMembers[j].Hp -= ::Damage[j];  
            }
            break;
        }
    }
}

void ChangeTurn() {
    // Changes turn both for teams and for individual characters
    int foo;
    if(::Team1Turn) {
        ::Team1TurnCounter = (::Team1TurnCounter + 1) % 4;
        while(true) {
            foo = ::TurnOrderTeam2List[::Team2TurnCounter];
            if(::BattleMembers[foo - 2].Hp <= 0) {
                ::Team2TurnCounter = (::Team2TurnCounter + 1) % 4;
            } else {
                break;
            }
        }
    } else {
        ::Team2TurnCounter = (::Team2TurnCounter + 1) % 4;
        while(true) {
            foo = ::TurnOrderTeam1List[::Team1TurnCounter];
            if(::BattleMembers[foo - 2].Hp <= 0) {
                ::Team1TurnCounter = (::Team1TurnCounter + 1) % 4;
            } else {
                break;
            }
        }
    }
    ::Team1Turn = !::Team1Turn;
}

void DEBUG_GB_AND_BM() {
    // analog of PromptInput() made for debugging
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 12; j++){
            cout << ::GameBoard[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    for(int i = 0; i < 8; i++){
        cout << "::BattleMembers[" << i << "] satas:" << endl;
        cout << "  .Id is " << ::BattleMembers[i].Id << endl;
        cout << "  .AllianceId is " << ::BattleMembers[i].AllianceId << endl;
        cout << "  .Team is \"" << ::BattleMembers[i].Team << "\"" << endl;
        cout << "  .Name is \"" << ::BattleMembers[i].Name << "\"" << endl;
        cout << "  .Hp is " << ::BattleMembers[i].Hp << endl;
        cout << "  .Steps is " << ::BattleMembers[i].Steps << endl;
        cout << "  .MeleeDmg is " << ::BattleMembers[i].MeleeDmg << endl;
        cout << "  .RangeDmg is " << ::BattleMembers[i].RangeDmg << endl;
        cout << "  .RangeDistance is " << ::BattleMembers[i].RangeDistance << endl;
        cout << endl;
    }
}


void BotsTurn() {
    // This replaces second team players by bots.
    character ThisBot = GetCharacterByTurn();
    for(int i = 0; i < 8; i++) {
        ::Damage[i] = 0;
    }
    /*pair<int, int> EnYX[4] = {
        {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}
    };*/
    int EnDist[4] = {
        -1, -1, -1, -1
    };
    vector<pair<int, int>> path[4];
    
    // Using BFS to find shortest path to enemy
    bool used[8][12];
    //int dst[8][12];
    pair<int, int> pr[8][12];
    const int y1 = YofGameBoardById(ThisBot.Id); 
    const int x1 = XofGameBoardById(ThisBot.Id);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            //dst[i][j] = -1;
            pr[i][j] = {-1, -1};
            used[i][j] = false;
        }
    }

    queue<pair<int, int>> q;    //Мы храним уже не номера вершин, а координаты клеток.
    q.push({y1, x1});       //Предполагается использование С++11, в противном случае используйте make_pair
    used[y1][x1] = true;
    //dst[y1][x1] = 0;

    while (!q.empty()) {
        pair<int, int> cur = q.front();
        q.pop();
        
        int cy = cur.first, cx = cur.second, cid = ::GameBoard[cy][cx];
        
        
        
        if((cid == 2 || cid == 3 || cid == 4 || cid == 5) && (::BattleMembers[cid - 2].Hp - ::Damage[cid - 2]) > 0) {
            //EnYX[cid] = {cy, cx};
            //EnDist[cid] = dst[cy][cx];
            //cout << "\nEnemy found, id is: " << cid << endl;
            int cy1 = cy, cx1 = cx;
            pair<int, int> cpair = {pr[cy1][cx1].first, pr[cy1][cx1].second};
            while (cpair.first != y1 || cpair.second != x1) {   //пока существует предыдущая вершина
                cy1 = cpair.first;
                cx1 = cpair.second;        //переходим в неё
                path[cid - 2].push_back({cy1, cx1});    //и дописываем к пути
                cpair = { pr[cy1][cx1].first, pr[cy1][cx1].second };
            }
        }
        
        vector<pair<int, int>> moves {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };
    
        for (auto move: moves) {
            int dy = move.first, dx = move.second;
            int x = cx + dx, y = cy + dy;
            if (x >= 0 && x < 12
                && y >= 0 && y < 8
                && !used[y][x]
                && (
                    cid == 0
                    || (
                        cid != 1
                        && (
                            ::BattleMembers[cid - 2].Hp - ::Damage[cid - 2] <= 0
                            || cid == ThisBot.Id
                            )
                        )
                    )
            ) {
                q.push({y, x});
                used[y][x] = true;
                //dst[y][x] = dst[cy][cx] + 1;
                pr[y][x] = {cy, cx};
            }
        }
    }
    
    //Восстановим кратчайший путь до вершины k (которую, предполагается, мы уже ввели)
    //Для восстановления пути пройдём его в обратном порядке, начиная с j, и развернём.
    for (int i = 0; i < 4; i++) {
        if(path[i].size() == 0) continue;
        reverse(path[i].begin(), path[i].end());
    }
    
    for(int i = 0; i < 4; i++) {
        if(path[i].size() != 0)  EnDist[i] = path[i].size() - 1;
        else EnDist[i] = 3e3;
    }
    
    // OH GOD, WE FINALY GOT IT.
    
    int CloseEnemyId = -1;
    int CloseDist = 3e3;

    /*int i = 1;
    for (auto person : path) {
        i++;
        cout << i << "\n\n";
        for (auto el : person) {
            cout << el.first << " " << el.second << "\n";
        }
    }*/
    
    for(int i = 0; i < 4; i++) {
        if(EnDist[i] < CloseDist) {
            CloseDist = EnDist[i];
            CloseEnemyId = i + 2;
        }
    }
    
    if (CloseEnemyId == -1) {
        cout << "\nCloseEnemyId == -1\n";
        return;
    }
    if (CloseDist == 0) {
        cout << "\nCloseDist == 0\n";
        return;
    }
    
    
    if(CloseDist >= ThisBot.Steps) {
        ::GameBoard[y1][x1] = 0;
        pair<int, int> LastPair = path[CloseEnemyId - 2][ThisBot.Steps - 1];
        ::GameBoard[LastPair.first][LastPair.second] = ThisBot.Id;
    } else {
        ::GameBoard[y1][x1] = 0;
        pair<int, int> LastPair = path[CloseEnemyId - 2].back();
        ::GameBoard[LastPair.first][LastPair.second] = ThisBot.Id;
    }
}

void ChooseTurnAI(int state){
    /*if(state == "default bots") {
        BotsTurn();
    } else if(state == "player") {
        PromptInput();
        ProcessInput();
    } else {
        cout << "Error in ChooseTurnAI():  unknown state \"" << state << "\"" << endl;
    }*/
    switch(state) {
        case 0:
            PromptInput();
            ProcessInput();
            break;
        case 1:
            BotsTurn();
            break;
        default:
            cout << "Error in ChooseTurnAI():  unknown state \"" << state << "\"" << endl;
    }
}

void CollectStatistics() {
    // Stores data about game in ANALISYS.
    array<aCharData, 8> arr;
    int cHp;
    aCharData *el;
    
    for(int i = 0; i < 8; i++) {
        cHp = ::BattleMembers[i].Hp;
        el = &arr[i];
        (*el).Hp = cHp;
        if(cHp > 0) {
            (*el).X = XofGameBoardById(i + 2);
            (*el).Y = YofGameBoardById(i + 2);
        } else {
            (*el).X = -1;  // is not a valid square id.
            (*el).Y = -1;
        }
    }
    ::aGameData.push_back(arr);
}

void DoAnalisys() {
    bool gotResp = false;
    while(!gotResp) {
        string resp;
        cout << "\nopen  game analisys menue?\n1.Yes\n2.No\n";
        cin >> resp;
        switch(resp[0]) {
            case '2':
            case 'N':
            case 'n':
                cout << "bye!\n";
                return;  // exits the game
            case '1':
            case 'Y':
            case 'y':
                gotResp = true;
                break;
            default:
                cout << "invalid input, try again:\n";
                break;
        }
    }
    // ANALISYS STARTS
}

void MakeTheGameWork() {
    // Main game loop. Name speaks for itself.
    while(true) {
        if(Team1Turn){
            PromptInput();
            ProcessInput();
        } else {
            ChooseTurnAI(::Team2AI);
            //BotsTurn();
        }
        CollectStatistics();
        if(TeamVictory() != 0) break;
        ChangeTurn();
    }
}

int main() {
    
    // I really hoped this would fix russian languege in console problem, but,
    // for some obscure reason, it probably didn't.
    // 
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    BootData();
    MakeTheGameWork();
    DoAnalisys();
    
    return 0;
}
