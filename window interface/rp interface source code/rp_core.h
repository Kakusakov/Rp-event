#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winuser.h>  // for  MessageBox()
#include <cmath>
#include <queue>  // Needed for BFS
#include <array>
#include <utility>  // Needed for BFS
#include <algorithm>  // Needed for BFS

// do not forget to delete unuseed libraries after finishing the project. 
// (oh god, it would be a mess..)




/*

                //////////////////////////////////////////////////////////
           ///////////////////                           ///////////////////
      ///////////////////    GAME VERSION 1.2 - INTERFACE    ///////////////////
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

    5. add a way to easily shift team size

    6. turn tempWTF into lambda

    7. all points marked with "//REDO ( STEP 1 COMPLETE )" are fixed. Now they serve as markers for places where things can go wrong

    8. turn messagebox errors into lambda / function

    9. implement a constant for max input bar capcity // or better yet make it so big, that you wont need it

    10. remove all variables from messageboxes cuz they dont vork properly and mess everything up

    11. can add a featur: marks on squares where someone died
*/


const size_t x_max = 12;
const size_t y_max = 8;

int GameBoard[y_max][x_max] = { };  // std::array representing map of the game
const std::wstring AllianceNameList[6] = {
        L"огонь", L"земля", L"вода",
        L"свет", L"тьма", L"пыль"
};  // Names Alliances per id.
int TurnOrderTeam1List[4] = { 2, 3, 4, 5 };  // Order of turn for team 1
int TurnOrderTeam2List[4] = { 6, 7, 8, 9 };  // Order of turn for team 2
std::array<HBRUSH, 8> character_hBrushes = { };
bool Team1Turn = true;  // If true first team's turn, otherwise second team's turn
int Team1TurnCounter = 0;  // If team 1 goes this turn, which of it's members goes
int Team2TurnCounter = 0;  // If team 2 goes this turn, which of it's members goes
int Team2AI;  // sets an AI for the second team if needed

int Damage[8] = { };  // damage in current turn dealt to characters per Id (kinda workaround).

// ANALISYS TOOLS

struct aCharData {
    //int Id;
    int Y;
    int X;
    int Hp;
};
int aStartField[y_max][x_max] = { };
int aLastField[y_max][x_max] = { };
typedef std::array<aCharData, 8> aTurnData;
aTurnData aStartData = { };
std::vector<aTurnData> aGameData;

// END OF ANALISYS TOOLS

/*struct PosYX {
    // WIP
    // represents coordinates of square on GameBoard
    int Y;
    int X;
};*/


class character{
private:
    int id;
    std::wstring team;
    int allianceId;
    std::wstring name;
    int hp;
    int steps;
    int meleeDmg;
    int rangeDmg;
    int rangeDistance;
    HBITMAP image;

public:
    
    character(
        const int inp_id
        //const int bonus
    ) {
        const std::wstring CharacterNames[8] = {
        L"Tr", L"Sl", L"Ne", L"Us",
        L"An", L"It", L"Ru", L"Ky"
        };
        id = inp_id;
        team = (id <= 5) ? L"ghouls" : L"shizy";
        allianceId = id % 6;
        name = CharacterNames[id - 2];
        hp = 30;
        steps = 5;
        meleeDmg = 6;
        rangeDmg = 5;
        rangeDistance = 3;
        image = NULL;  //(HBITMAP)LoadImage(NULL, L"g:\\sans.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
    constexpr int get_id() {
        return id;
    }
    const std::wstring get_team() {
        return team;
    }
    constexpr int get_allianceId() {
        return allianceId;
    }
    const std::wstring get_name() {
        return name;
    }
    constexpr int get_steps() {
        return steps;
    }
    constexpr int get_meleeDmg() {
        return meleeDmg;
    }
    constexpr int get_rangeDmg() {
        return rangeDmg;
    }
    constexpr int get_rangeDistance() {
        return rangeDistance;
    }
    constexpr int get_hp() {
        return hp;
    }
    constexpr HBITMAP get_image() {
        return image;
    }

    //constexpr void set_hp(const int value) {
    //    hp = value;
    //}
    constexpr void set_hp(const HBITMAP bitmap) {
        image = bitmap;
    }
    constexpr void subtract_hp(const int value) {
        hp -= value;
    }

};

float FixFPError(float val) {
    // This is a workaround to handle floating point errors near integer values
    if (val == floorf(val)) return val;
    if (val - 0.0001 < floorf(val)) return floorf(val);
    if (val + 0.0001 > ceilf(val)) return ceilf(val);
    return val;
}

character BattleMembers[8] = { 
    { 2 }, { 3 }, { 4 }, { 5 },
    { 6 }, { 7 }, { 8 }, { 9 } 
};  // std::array representing every character in current game

int YofGameBoardById(const int seek) {
    // This returns Y coordinate of given id on the map.
    // if multiple ids found, might not work properly
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 12; x++) {
            if (::GameBoard[y][x] == seek) {
                return y;
            }
        }
    }
    LPCWSTR temp = (L"Error in YofGameBoardById():\nIndex \"" + std::to_wstring(seek) + L"\"not found").c_str();
    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
    return -1;
}

int XofGameBoardById(const int seek) {
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
    LPCWSTR temp = (L"Error in XofGameBoardById():\nIndex \"" + std::to_wstring(seek) + L"\"not found").c_str();
    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
    return -1;
}

// REDO ( STEP 1 COMPLETE )
void BootData(const HWND hWnd) {
    // This loads all neccessary data before game starts


    int msgBoxId = MessageBox(hWnd, L"включить ботов?", L"выбор", MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL );
    if (msgBoxId == IDYES) {
        ::Team2AI = 1;  // default bots
    }
    else {
        ::Team2AI = 0;  // local human player
    }


    const int field_temp[8][12] = {
        {1, 1, 1, 1, 2, 3, 4, 5, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 6, 7, 8, 9, 1, 1, 1, 1}
    };
    /*
    0 - emty space
    1 - wall
    2-9 - characters
    */
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            const int temp_square = field_temp[i][j];  // current index
            ::GameBoard[i][j] = temp_square;
            if (temp_square != 0 && temp_square != 1) {
                ::aStartField[i][j] = 0;  // ::aStartField is an empty field, it does not have characters.
            }
            else {
                ::aStartField[i][j] = temp_square;
            }
        }
    }
    
    for (int i = 0; i < 8; i++) {
        ::character_hBrushes[i] = CreateSolidBrush(RGB((128 + (i + 1) * 64) % 256, (128 + (i + 1) * 32) % 256, (128 + (i + 1) * 16) % 256));
        ::aStartData[i].Hp = 30;
        ::aStartData[i].X = XofGameBoardById(i + 2);
        ::aStartData[i].Y = YofGameBoardById(i + 2);
    }
}

character GetCharacterByTurn() {
    // This returns character who's turn is now
    int Id;
    if (::Team1Turn) {
        Id = ::TurnOrderTeam1List[::Team1TurnCounter];
    }
    else {
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

/* UNNECESSARY
void PrintGameBoard() {
    // prints map of the game into cmd
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            int foo = ::GameBoard[i][j];
            switch (foo) {
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
                if (BattleMembers[foo - 2].Hp > 0) {
                    cout << BattleMembers[foo - 2].get_name();
                }
                else {
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
} // REDO
*/

/* UNNECESSARY

void aPrintGameBoard(aTurnData c) {
    int hp[8], x[8], y[8];
    int prCharacter;
    for (int i = 0; i < 8; i++) {
        hp[i] = c[i].Hp;
        x[i] = c[i].X;
        y[i] = c[i].Y;
    }
    int foo;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            for (int k = 0; k < 8; k++) {
                prCharacter = 0;
                if (y[k] == i && x[k] == j && hp[k] > 0) {
                    prCharacter = k + 2;
                    break;
                }
            }
            if (prCharacter == 0) {
                foo = ::aStartField[i][j];
            }
            else {
                foo = prCharacter;
            }
            switch (foo) {
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
                cout << BattleMembers[foo - 2].get_name();
            default:
                cout << "\nerror in aPrintGameBoard(): unregistred Id = " << foo << " in aStartField[" << i << "][" << j << "]" << endl;
            }
            cout << "  ";
        }
        cout << endl;
    }
}
*/

void PrintTurnOrder(const HDC hDC, const int x, const int y) {
    // Prints turn order for next 8 turns including current

    // Copying counters to prevent changing their value
    bool Counter = ::Team1Turn;
    int Counter1 = ::Team1TurnCounter;
    int Counter2 = ::Team2TurnCounter;

    int guy1Id;  // person from team 1
    int guy2Id;  // person from team 2
    std::wstring out = L"Порядок хода: ";
    //cout << "Порядок хода: ";
    for (int i = 0; i < 8; i++) {
        if (Counter) {
            guy1Id = ::TurnOrderTeam1List[Counter1];
            out += ::BattleMembers[guy1Id - 2].get_name() + L" ";
            Counter1 = (Counter1 + 1) % 4;
            while (true) {
                guy2Id = ::TurnOrderTeam2List[Counter2];
                if (::BattleMembers[guy2Id - 2].get_hp() <= 0) {
                    Counter2 = (Counter2 + 1) % 4;
                    i++;
                }
                else {
                    break;
                }
            }
        }
        else {
            guy2Id = ::TurnOrderTeam2List[Counter2];
            out += ::BattleMembers[guy2Id - 2].get_name() + L" ";
            Counter2 = (Counter2 + 1) % 4;
            while (true) {
                guy1Id = ::TurnOrderTeam1List[Counter1];
                if (::BattleMembers[guy1Id - 2].get_hp() <= 0) {
                    Counter1 = (Counter1 + 1) % 4;
                    i++;
                }
                else {
                    break;
                }
            }
        }
        Counter = !Counter;
    }
    TextOut(hDC, x, y, out.c_str(), out.length());
}

/* UNNECESSARY
void aDisplayTurn(int turn) {
    aTurnData c;  // data to display
    if (turn < 0 || turn > ::aGameData.size()) {
        cout << "error in aDisplayTurn(): invalid turn " << turn << endl;
        return;
    }
    else if (turn == 0) {  // turn 0 is the start turn, not ::aGameData[0].
        c = ::aStartData;
    }
    else {
        c = ::aGameData[turn - 1];
    }
    aPrintGameBoard(c);
}
*/

/* UNNECESSARY
void PrintTurn() {
    // This prints stats of character who goes this turn
    character ThisCharacter = GetCharacterByTurn();
    cout << "ход команды " << ThisCharacter.get_team() << endl;
    cout << " ход игрока " << ThisCharacter.get_name() << endl;
    cout << "  Альянс: " << AllianceNameList[ThisCharacter.get_allianceId()] << endl;
    cout << "  Хп: " << ThisCharacter.Hp << endl;
    cout << "  Шагов за ход: " << ThisCharacter.get_steps() << endl;
    cout << "  Ближний урон: " << ThisCharacter.get_meleeDmg() << endl;
    cout << "  Дальний урон: " << ThisCharacter.get_rangeDmg() << endl;
    cout << "  Дальность: " << ThisCharacter.get_rangeDistance() << endl;
    cout << endl;
}
*/

/* UNNECESSARY
void PrintStats() {
    // This prints short stats of every character in current game
    for (int i = 0; i < 8; i++) {
        if (::BattleMembers[i].Hp > 0) {
            cout << "Статистика игрока  " << ::BattleMembers[i].get_name() << " команды " << ::BattleMembers[i].get_team() << endl;
            cout << "  Альянс: " << AllianceNameList[::BattleMembers[i].get_allianceId()] << endl;
            cout << "  Хп: " << ::BattleMembers[i].Hp << endl;
            // Some stats are omited for the sake of clean interface.
            // full stats can be accessed by using /s command
            //cout << "  Steps per turn: " << ::BattleMembers[i].get_steps() << endl;
            //cout << "  Melee Dmg: " << ::BattleMembers[i].get_meleeDmg() << endl;
            //cout << "  Range Dmg: " << ::BattleMembers[i].get_rangeDmg() << endl;
            //cout << "  Range: " << ::BattleMembers[i].get_rangeDistance() << endl;
            cout << endl;
        }
        else {
            cout << "Игрок " << ::BattleMembers[i].get_name() << " команды " << ::BattleMembers[i].get_team() << " мертв." << endl;
            cout << endl;
        }
    }
}
*/

void print_character_data(const HDC hDC, const int x, const int y, const int id) {
    const int hight_interval = 20;
    const int pos = id - 2;
    RECT r_temp = { };
    std::wstring temp;
    for (int i = 0; i < 8; i++) {
        switch (i) {
        case 0:
            r_temp = { x, y + hight_interval * i, x + hight_interval, (y + hight_interval * i) + hight_interval };
            FillRect(hDC, &r_temp, character_hBrushes[pos]);
            break;
        case 1:
            temp = L"Имя: " + ::BattleMembers[pos].get_name();
            break;
        case 2:
            temp = L"Альянс: " + ::AllianceNameList[::BattleMembers[pos].get_allianceId()];
            break;
        case 3:
            if (!(::BattleMembers[pos].get_hp() <= 0))
                temp = L" Хп: " + std::to_wstring(::BattleMembers[pos].get_hp());
            else 
                temp = L" Хп: мертв";
            break;
        case 4:
            temp = L"Шагов за ход: " + std::to_wstring(::BattleMembers[i].get_steps());
            break;
        case 5:
            temp = L"Ближний урон: " + std::to_wstring(::BattleMembers[i].get_meleeDmg());
            break;
        case 6:
            temp = L"Дальний урон: " + std::to_wstring(::BattleMembers[i].get_rangeDmg());
            break;
        case 7:
            temp = L"Дальность: " + std::to_wstring(::BattleMembers[i].get_rangeDistance());
            break;
        // no default
        }
        TextOut(hDC, x, y + hight_interval * i, (temp).c_str(), temp.length());
    }
}

void every_print_character_data(const HDC hDC, const int x, const int y) {
    for (int i = 0; i < 8; i++) {
        print_character_data(hDC, x + (i * 170) % 680, (i < 4) ? y : y + 200, i + 2);
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
    if (
        ::BattleMembers[0].get_hp() <= 0
        &&
        ::BattleMembers[1].get_hp() <= 0
        &&
        ::BattleMembers[2].get_hp() <= 0
        &&
        ::BattleMembers[3].get_hp() <= 0
        ) {
        LPCWSTR temp = (L"Команда " + ::BattleMembers[4].get_team() + L" победила!").c_str();
        MessageBox(0, temp, L"Error", MB_OK | MB_ICONINFORMATION);
        return 2;
    }
    else if (
        ::BattleMembers[4].get_hp() <= 0
        &&
        ::BattleMembers[5].get_hp() <= 0
        &&
        ::BattleMembers[6].get_hp() <= 0
        &&
        ::BattleMembers[7].get_hp() <= 0
        ) {
        LPCWSTR temp = (L"Команда " + ::BattleMembers[0].get_team() + L" победила!").c_str();
        MessageBox(0, temp, L"Error", MB_OK | MB_ICONINFORMATION);
        return 1;
    }
    else {
        return 0;
    }
}

/* UNNECASSARY
void PrintAllStats() {
    // This prints stats of every character in current game.
    // Unlike PrintStats(), this does not omit any of the stats
    for (int i = 0; i < 8; i++) {
        if (::BattleMembers[i].Hp > 0) {
            cout << "Полная статистика игрока  " << ::BattleMembers[i].get_name() << " команды " << ::BattleMembers[i].get_team() << endl;
            cout << "  Альянс: " << AllianceNameList[::BattleMembers[i].get_allianceId()] << endl;
            cout << "  Хп: " << ::BattleMembers[i].Hp << endl;
            cout << "  Шагов за ход: " << ::BattleMembers[i].get_steps() << endl;
            cout << "  Ближний урон: " << ::BattleMembers[i].get_meleeDmg() << endl;
            cout << "  Дальний урон: " << ::BattleMembers[i].get_rangeDmg() << endl;
            cout << "  Дальность: " << ::BattleMembers[i].get_rangeDistance() << endl;
            cout << endl;
        }
        else {
            cout << "Игрок " << ::BattleMembers[i].get_name() << " команды " << ::BattleMembers[i].get_team() << " мертв." << endl;
            cout << endl;
        }
    }
}
*/

constexpr bool TempWTF(const int Temp69, const int ThisCharacter_Id, const int Target_Id) {
    // This is micro-sub-fuction of ProcessInput, used in 'q' branch.
    return Temp69 != 0
        && Temp69 != ThisCharacter_Id
        && ::BattleMembers[Temp69 - 2].get_hp() - ::Damage[Temp69 - 2] > 0
        && Temp69 != Target_Id;
}

// REDO ( STEP 1 COMPLETE )
int IsValidMovement(
    wchar_t Move,
    const int ThisCharacter_Id,
    const int CurrentY,
    const int CurrentX
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

    if (CurrentY < 1 && Move == L'w') return 1;
    if (CurrentY > 6 && Move == L's') return 1;
    if (CurrentX < 1 && Move == L'a') return 1;
    if (CurrentX > 10 && Move == L'd') return 1;

    if (Move == L'w') ThisSquareId = ::GameBoard[CurrentY - 1][CurrentX];
    else if (Move == L's') ThisSquareId = ::GameBoard[CurrentY + 1][CurrentX];
    else if (Move == L'a') ThisSquareId = ::GameBoard[CurrentY][CurrentX - 1];
    else if (Move == L'd') ThisSquareId = ::GameBoard[CurrentY][CurrentX + 1];
    else {
        // ???!!!!?!?!?!?!??!
        std::wstring t3 = L"ERROR in IsValidMovement():\n invalid char passed as move";
        LPCWSTR temp = (t3).c_str();
        MessageBox(0, temp, L"error", MB_OK | MB_ICONERROR);
    }

    

    if (ThisSquareId == 1) return 2;

    if (
        ThisSquareId != 0
        &&
        ::BattleMembers[ThisSquareId - 2].get_hp() - ::Damage[ThisSquareId - 2] > 0
        &&
        ThisSquareId != ThisCharacter_Id
        ) {
        return 3;
    }
    return 0;
}

/*UNNEEDED
void PromptInput() {
    // This tells player most part of neccessary info about game state.
    //PrintGameBoard();
    PrintStats();
    PrintTurnOrder();
    //PrintTurn();


    // code from below can be reimplemented using help menue
    // 
    //cout << "Введи /h для помощи по командам." << endl;  
    //cout << "Пожалуйста, введи ход:" << endl;
}*/

// REDO ( STEP 1 COMPLETE )
bool ProcessInput(std::wstring TurnDescription) {
    // This processes player's turn description untill can execute it,
    // then, (you guessed it), executes it.
    character ThisCharacter = GetCharacterByTurn();
    bool ErrorInInputOccured;
    bool TurnSkiped;
    int UsedStepsCounter = 0;
    int CharIndexCounter = 0;
    int UnskipedChars = 0;
    ErrorInInputOccured = false; // True if turn description is incorrect
    TurnSkiped = false; // True if player chose to skip character's turn
    const int StartY = YofGameBoardById(ThisCharacter.get_id());
    const int StartX = XofGameBoardById(ThisCharacter.get_id());
    int ShiftY = 0;
    int ShiftX = 0;
    for (int i = 0; i < 8; i++) {
        ::Damage[i] = 0;
    }  // Damage that will be delt to a coresponding id of character in ::BattleMembers.

    //string TurnDescription;  // whatever player typed to describe his turn.
    //cin >> TurnDescription;
    for (wchar_t& c : TurnDescription) {
        // This checks if turn is valid, and, if it is, gets ready to
        // pass data to the part whrere it gets executed.
        if (ErrorInInputOccured == true || TurnSkiped == true) {
            break;
        }
        if (UsedStepsCounter > ThisCharacter.get_steps()) {
            //cout << "Ошибка: слишком много шагов" << endl;
            //cout << "Пожалуйста введи ход:" << endl;
            std::wstring t1 = L"Ошибка: слишком много шагов";
            LPCWSTR temp = (t1).c_str();
            MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
            break;
        }
        if (UnskipedChars != CharIndexCounter) {
            UnskipedChars++;
            continue;
        }
        int CurrentY = StartY + ShiftY;
        int CurrentX = StartX + ShiftX;
        int ThisSquareId;
        int repeatedQs = 1;


        // c - each character in turn description
        c = tolower(c);


        if (c == L' ') {
            CharIndexCounter++;
            //UnskipedChars++;
        }
        else if (c == L'w' || c == L'a' || c == L's' || c == L'd') {
            // If c represents movement / skip
            if (c == L's' && TurnDescription.size() != CharIndexCounter + 1 && TurnDescription[CharIndexCounter + 1] == L'k') {
                TurnSkiped = true;
                break;
            }  // This if statement is responsible for skiping turn

            int ErrorCode = IsValidMovement(c, ThisCharacter.get_id(), CurrentY, CurrentX);
            if (ErrorCode == 0) {
                if (c == L'w') ShiftY--;
                else if (c == L's') ShiftY++;
                else if (c == L'a') ShiftX--;
                else if (c == L'd') ShiftX++;
                else {
                    //cout << "\n\nERROR in ProcessInput(): char '" << c << "' does not represent movement\n" << endl;
                    LPCWSTR temp = (L"ERROR in ProcessInput(): char '" + std::to_wstring(c) + L"' does not represent movement").c_str();
                    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                    return false;
                }


                UsedStepsCounter++;
                CharIndexCounter++;
                //UnskipedChars++;
            }
            else if (ErrorCode == 1) {
                ErrorInInputOccured = true;
                //cout << "Ошибка: персонажи не могут выйти за пределы карты" << endl;
                //cout << "Пожалуйста введи ход:" << endl;
                std::wstring t1 = L"Ошибка: персонажи не могут выйти за пределы карты";
                LPCWSTR temp = (t1).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
            else if (ErrorCode == 2) {
                ErrorInInputOccured = true;
                //cout << "Ошибка: нет, через стены проходить нельзя" << endl;
                //cout << "Пожалуйста введи ход:" << endl;
                std::wstring t1 = L"Ошибка: нет, через стены проходить нельзя";
                LPCWSTR temp = (t1).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
            else if (ErrorCode == 3) {
                ErrorInInputOccured = true;
                //cout << "Ошибка: нельзя проходить через других персонажей" << endl;
                //cout << "Пожалуйста введи ход:" << endl;
                std::wstring t1 = L"Ошибка: нельзя проходить через других персонажей";
                LPCWSTR temp = (t1).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
            else {
                //cout << "\n\nERROR in ProcessInput(): ErrorCode has incorrect value of " << ErrorCode << "\n" << endl;
                LPCWSTR temp = (L"ERROR in ProcessInput(): ErrorCode has incorrect value of " + std::to_wstring(ErrorCode)).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
        }
        else if (c == L'e') {
            // If c represents melee attack
            int IdAdjacent[4];
            if (CurrentY > 0) IdAdjacent[0] = GameBoard[CurrentY - 1][CurrentX];
            else IdAdjacent[0] = 0;
            if (CurrentY < 7) IdAdjacent[1] = GameBoard[CurrentY + 1][CurrentX];
            else IdAdjacent[1] = 0;
            if (CurrentX > 0) IdAdjacent[2] = GameBoard[CurrentY][CurrentX - 1];
            else IdAdjacent[2] = 0;
            if (CurrentX < 11) IdAdjacent[3] = GameBoard[CurrentY][CurrentX + 1];
            else IdAdjacent[3] = 0;
            for (int j = 0; j < 4; j++) {
                if (
                    IdAdjacent[j] != 0
                    &&
                    IdAdjacent[j] != 1
                    &&
                    ThisCharacter.get_team() != ::BattleMembers[IdAdjacent[j] - 2].get_team()
                    ) {
                    int BMPos = IdAdjacent[j] - 2;
                    if (ThisCharacter.get_allianceId() / 3 == ::BattleMembers[BMPos].get_allianceId() / 3) {
                        if ((ThisCharacter.get_allianceId() + 1) % 3 == ::BattleMembers[BMPos].get_allianceId() % 3) {
                            ::Damage[BMPos] += ThisCharacter.get_meleeDmg() + 2;
                        }
                        else if ((ThisCharacter.get_allianceId() - 1) % 3 == ::BattleMembers[BMPos].get_allianceId() % 3) {
                            ::Damage[BMPos] += ThisCharacter.get_meleeDmg() - 2;
                        }
                        else {
                            ::Damage[BMPos] += ThisCharacter.get_meleeDmg();
                        }
                    }
                    else {
                        ::Damage[BMPos] += ThisCharacter.get_meleeDmg();
                    }
                }
            }
            UsedStepsCounter++;
            CharIndexCounter++;
            //UnskipedChars++;
        }
        else if (c == L'q') {
            /*
            If c represents range attack.

            Ok, so the first part is easy: we just need to get the id of the target
            using the name specified in turn description.
            */
            std::wstring Target = L"";
            while (true) {
                if (TurnDescription[CharIndexCounter + 1] == L'q') {
                    repeatedQs++;
                    CharIndexCounter++;
                }
                else break;
            }
            if (TurnDescription[CharIndexCounter + 1] != L'(') {
                ErrorInInputOccured = true;
                //cout << "Ошибка: после каждой \'q\' нужно указать имя в круглых скобках" << endl;
                //cout << "Пожалуйста введи ход:" << endl;
                std::wstring t1 = L"Ошибка: после каждой \'q\' нужно указать имя в круглых скобках";
                LPCWSTR temp = (t1).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
            int length = TurnDescription.length();
            int TargetSize = 0;
            for (int k = CharIndexCounter + 2; k <= length; k++) {
                if (k == length) {
                    ErrorInInputOccured = true;
                    //cout << "Ошибка: все скобки, содержащие имя, должны быть закрты" << endl;
                    //cout << "Пожалуйста введи ход:" << endl;
                    std::wstring t1 = L"Ошибка: все скобки, содержащие имя, должны быть закрты";
                    LPCWSTR temp = (t1).c_str();
                    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                    return false;
                }
                wchar_t Stuff = TurnDescription[k];
                if (Stuff != L')') {
                    Target += Stuff;
                    TargetSize++;
                }
                else {
                    CharIndexCounter += 3 + TargetSize;
                    break;
                }
            }
            for (wchar_t& d : Target) {
                d = tolower(d);
            };
            int TargetId = -1;
            for (int k = 0; k <= 8; k++) {
                if (k == 8) {
                    ErrorInInputOccured = true;
                    //cout << "Ошибка: нет персонажей указанным именем" << endl;
                    //cout << "Пожалуйста введи ход:" << endl;
                    std::wstring t1 = L"Ошибка: нет персонажей c указанным именем";
                    LPCWSTR temp = (t1).c_str();
                    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                    return false;
                }
                std::wstring PossibleTarget = ::BattleMembers[k].get_name();
                for (wchar_t& d : PossibleTarget) {
                    d = tolower(d);
                };
                if (PossibleTarget == Target) {
                    TargetId = k + 2;
                    break;
                }
            }
            if (TargetId == ThisCharacter.get_id()) {
                ErrorInInputOccured = true;
                //cout << "Ошибка: не пытайся застрелиться. Пожалуйста. Ты нам нужен." << endl;
                //cout << "Пожалуйста введи ход:" << endl;
                std::wstring t1 = L"Ошибка: не пытайся застрелиться. Пожалуйста. Ты нам нужен.";
                LPCWSTR temp = (t1).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
            else if (::BattleMembers[TargetId - 2].get_team() == ThisCharacter.get_team()) {
                ErrorInInputOccured = true;
                //cout << "Ошибка: стрелять по своим - плохая идея" << endl;
                //cout << "Пожалуйста введи ход:" << endl;
                std::wstring t1 = L"Ошибка: стрелять по своим - плохая идея";
                LPCWSTR temp = (t1).c_str();
                MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                return false;
            }
            else {
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
                double dx_per_dy = FixFPError((DeltaY != 0) ? DeltaX / DeltaY : 1337);
                double dy_per_dx = FixFPError((DeltaX != 0) ? DeltaY / DeltaX : 1337);
                int dx = 0;
                int dy = 0;
                double s_x = FixFPError(CurrentX + 0.5 + ((DeltaY > 0) ? dx_per_dy * 0.5 : dx_per_dy * -0.5));
                double s_y = FixFPError(CurrentY + 0.5 + ((DeltaX > 0) ? dy_per_dx * 0.5 : dy_per_dx * -0.5));
                bool STWError = false;
                if (abs(DeltaY) + abs(DeltaX) > ThisCharacter.get_rangeDistance()) {
                    ErrorInInputOccured = true;
                    //cout << "Ошибка: недостаточно дальности для выстрела" << endl;
                    //cout << "Пожалуйста введи ход:" << endl;
                    std::wstring t1 = L"Ошибка: недостаточно дальности для выстрела";
                    LPCWSTR temp = (t1).c_str();
                    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                    break;
                }
                for (
                    int y = (DeltaY > 0) ? CurrentY + 1 : CurrentY - 1;
                    (DeltaY > 0) ? y <= TargetY : y >= TargetY;
                    (DeltaY > 0) ? y++ : y--
                    ) {
                    double cx = FixFPError(s_x + dy * dx_per_dy);
                    if (cx != floorf(cx)) {
                        if (TempWTF(GameBoard[y][(int)floorf(cx)], ThisCharacter.get_id(), TargetId)) {
                            STWError = true;
                            break;
                        }
                    }
                    else {
                        if (
                            TempWTF(GameBoard[y][(int)cx], ThisCharacter.get_id(), TargetId)
                            || TempWTF(GameBoard[y][(DeltaX > 0) ? (int)cx - 1 : (int)cx + 1], ThisCharacter.get_id(), TargetId)
                            ) {
                            STWError = true;
                            break;
                        }
                    }
                    dy++;
                }
                for (
                    int x = (DeltaX > 0) ? CurrentX + 1 : CurrentX - 1;
                    (DeltaX > 0) ? x <= TargetX : x >= TargetX;
                    (DeltaX > 0) ? x++ : x--
                    ) {
                    double cy = (s_y + dx * dy_per_dx);
                    if (cy != floorf(cy)) {
                        if (TempWTF(GameBoard[(int)floorf(cy)][x], ThisCharacter.get_id(), TargetId)) {
                            STWError = true;
                            break;
                        }
                    }
                    else {
                        if (
                            TempWTF(GameBoard[(int)cy][x], ThisCharacter.get_id(), TargetId)
                            || TempWTF(GameBoard[(int)cy][(DeltaY > 0) ? (int)cy - 1 : (int)cy + 1], ThisCharacter.get_id(), TargetId)
                            ) {
                            STWError = true;
                            break;
                        }
                    }
                    dx++;
                }
                /*float Slope;


                else if (DeltaY == 0) {
                    int FooSign = (DeltaX > 0) ? 1 : -1;
                    int DeltaFoo = DeltaX * FooSign;
                    for (int l = 1; l < DeltaFoo; l++) {
                        ThisSquareId = GameBoard[CurrentY][CurrentX + (l * FooSign)];
                        if (TempWTF(ThisSquareId, ThisCharacter.get_id(), TargetId)) {
                            ErrorInInputOccured = true;
                            //cout << "Ошибка: нельзя стрелять через стены или других персонажей" << endl;
                            //cout << "Пожалуйста введи ход:" << endl;
                            std::wstring t1 = L"Ошибка: нельзя стрелять через стены или других персонажей";
                            LPCWSTR temp = (t1).c_str();
                            MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                            break;
                        }
                    }
                }
                else if (DeltaX == 0) {
                    int FooSign = (DeltaY > 0) ? 1 : -1;
                    int DeltaFoo = DeltaY * FooSign;
                    for (int k = 1; k < DeltaFoo; k++) {
                        ThisSquareId = GameBoard[CurrentY + (k * FooSign)][CurrentX];
                        if (TempWTF(ThisSquareId, ThisCharacter.get_id(), TargetId)) {
                            ErrorInInputOccured = true;
                            //cout << "Ошибка: нельзя стрелять через стены или других персонажей" << endl;
                            //cout << "Пожалуйста введи ход:" << endl;
                            std::wstring t1 = L"Ошибка: нельзя стрелять через стены или других персонажей";
                            LPCWSTR temp = (t1).c_str();
                            MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                            break;
                        }
                    }
                }
                else {
                    int XInv = (DeltaX > 0) ? 1 : -1;  // -1
                    int YInv = (DeltaY > 0) ? 1 : -1;  // -1
                    Slope = (float)DeltaX * XInv / (float)DeltaY * YInv;  // 0.5
                    int Temp1;
                    int Temp2;
                    int Temp3;
                    int y;
                    float x = FixFPError(0.5 + Slope / 2);  // 0.75
                    for (y = 1; y <= ( DeltaY * YInv ); y++) {
                        if (Slope == 1) {
                            Temp1 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                            Temp2 = GameBoard[y * YInv + CurrentY][((int)floorf(x) - 1) * XInv + CurrentX];
                            if (
                                TempWTF(Temp1, ThisCharacter.get_id(), TargetId)
                                ||
                                TempWTF(Temp2, ThisCharacter.get_id(), TargetId)
                                ) {
                                STWError = true;
                                // break will cause bug.
                            }
                        }
                        else if (x == floorf(x)) {
                            //if (y != 1) {
                            if (Slope > 1) {
                                Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentY][y * YInv + CurrentX];
                                Temp2 = GameBoard[(int)floorf(x) * XInv + CurrentY][(y - 1) * YInv + CurrentX];
                                //Temp3 = GameBoard[((int)floorf(x) - 1) * XInv + CurrentY][y * YInv + CurrentX];
                            }
                            else if (Slope < 1) {

                                Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                Temp2 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                //Temp3 = GameBoard[y * YInv + CurrentY][((int)floorf(x) - 1) * XInv + CurrentX];
                            }
                            if (
                                TempWTF(Temp1, ThisCharacter.get_id(), TargetId)
                                ||
                                TempWTF(Temp2, ThisCharacter.get_id(), TargetId)
                                //||
                                //TempWTF(Temp3, ThisCharacter.get_id(), TargetId)
                                ) {
                                STWError = true;
                                break;
                            }
                            //}
                            else {
                                if (Slope > 1) {
                                    Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                    Temp2 = GameBoard[y * YInv + CurrentY][((int)floorf(x) - 1) * XInv + CurrentX];
                                }
                                else if (Slope < 1) {
                                    Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentY][y * YInv + CurrentX];
                                    Temp2 = GameBoard[((int)floorf(x) - 1) * XInv + CurrentY][y * YInv + CurrentX];
                                }
                                if (
                                    TempWTF(Temp1, ThisCharacter.get_id())
                                    ||
                                    TempWTF(Temp2, ThisCharacter.get_id())
                                    ) {
                                    STWError = true;
                                    break;
                                }
                            }
                            }
                            else {
                                //if (y != 1) {
                                if (Slope > 1) {
                                    Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                    Temp2 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                }
                                else if (Slope < 1) {
                                    Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentY][y * YInv + CurrentX];
                                    Temp2 = GameBoard[(int)floorf(x) * XInv + CurrentY][(y - 1) * YInv + CurrentX];
                                }
                                if (
                                    TempWTF(Temp1, ThisCharacter.get_id(), TargetId)
                                    ||
                                    TempWTF(Temp2, ThisCharacter.get_id(), TargetId)
                                    ) {
                                    STWError = true;
                                    break;
                                }
                                //}
                                else {
                                    if (Slope > 1) {
                                        Temp1 = GameBoard[y * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                                    }
                                    else if (Slope < 1) {
                                        Temp1 = GameBoard[(int)floorf(x) * XInv + CurrentY][y * YInv + CurrentX];
                                    }
                                    if (TempWTF(Temp1, ThisCharacter.get_id())) {
                                        STWError = true;
                                        break;
                                    }
                                }
                                }
                            x = FixFPError(Slope + x);
                        }
                        else {
                            Temp1 = GameBoard[(y - 1) * YInv + CurrentY][(int)floorf(x) * XInv + CurrentX];
                            if (TempWTF(Temp1, ThisCharacter.get_id())
                                ) {
                                STWError = true;
                                // break will cause bug.
                            }
                        }*/
                if (STWError == true) {
                    ErrorInInputOccured = true;
                    //cout << "Ошибка: нельзя стрелять через стены или других персонажей" << endl;
                    //cout << "Пожалуйста введи ход:" << endl;
                    std::wstring t1 = L"Ошибка: нельзя стрелять через стены или других персонажей";
                    LPCWSTR temp = (t1).c_str();
                    MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
                    return false;
                }
            }
            int BMPos = TargetId - 2;
            if (ThisCharacter.get_allianceId() / 3 == ::BattleMembers[BMPos].get_allianceId() / 3) {
                if ((ThisCharacter.get_allianceId() + 1) % 3 == ::BattleMembers[BMPos].get_allianceId() % 3) {
                    ::Damage[BMPos] += (ThisCharacter.get_rangeDmg() + 2) * repeatedQs;
                }
                else if ((ThisCharacter.get_allianceId() - 1) % 3 == ::BattleMembers[BMPos].get_allianceId() % 3) {
                    ::Damage[BMPos] += (ThisCharacter.get_rangeDmg() - 2) * repeatedQs;
                }
                else {
                    ::Damage[BMPos] += ThisCharacter.get_rangeDmg() * repeatedQs;
                }
            }
            else {
                ::Damage[BMPos] += ThisCharacter.get_rangeDmg() * repeatedQs;
            }
            UsedStepsCounter += repeatedQs;
            //UnskipedChars++;
        }
        else {
            ErrorInInputOccured = true;
            //cout << "Ошибка: неизвестный символ в описании хода" << endl;
            //cout << "Пожалуйста введи ход:" << endl;
            std::wstring t1 = L"Ошибка: неизвестный символ в описании хода";
            PCWSTR temp = (t1).c_str();
            MessageBox(0, temp, L"Error1", MB_OK | MB_ICONERROR);
            return false;
        }
        
        // commands can be reimplemented using help menue
        /*
        else if (c == '/') {
            ErrorInInputOccured = true;  // Not really an error but used to prevent accidental making of turn

            if (CharIndexCounter != 0) {
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
            switch (OrderName) {
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
        }*/
        UnskipedChars++;
    }
    if (
        ErrorInInputOccured == false
        &&
        UsedStepsCounter <= ThisCharacter.get_steps()
        ) {
        ::GameBoard[StartY][StartX] = 0;
        ::GameBoard[StartY + ShiftY][StartX + ShiftX] = ThisCharacter.get_id();
        for (int j = 0; j < 8; j++) {
            ::BattleMembers[j].subtract_hp(::Damage[j]);
        }
        return true;
    }
    else {
        return false;
    }
}

void ChangeTurn() {
    // Changes turn both for teams and for individual characters
    int foo;
    if (::Team1Turn) {
        ::Team1TurnCounter = (::Team1TurnCounter + 1) % 4;
        while (true) {
            foo = ::TurnOrderTeam2List[::Team2TurnCounter];
            if (::BattleMembers[foo - 2].get_hp() <= 0) {
                ::Team2TurnCounter = (::Team2TurnCounter + 1) % 4;
            }
            else {
                break;
            }
        }
    }
    else {
        ::Team2TurnCounter = (::Team2TurnCounter + 1) % 4;
        while (true) {
            foo = ::TurnOrderTeam1List[::Team1TurnCounter];
            if (::BattleMembers[foo - 2].get_hp() <= 0) {
                ::Team1TurnCounter = (::Team1TurnCounter + 1) % 4;
            }
            else {
                break;
            }
        }
    }
    ::Team1Turn = !::Team1Turn;
}

/*void DEBUG_GB_AND_BM() {
    // analog of PromptInput() made for debugging
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            cout << ::GameBoard[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    for (int i = 0; i < 8; i++) {
        cout << "::BattleMembers[" << i << "] satas:" << endl;
        cout << "  .get_id() is " << ::BattleMembers[i].get_id() << endl;
        cout << "  .get_allianceId() is " << ::BattleMembers[i].get_allianceId() << endl;
        cout << "  .get_team() is \"" << ::BattleMembers[i].get_team() << "\"" << endl;
        cout << "  .get_name() is \"" << ::BattleMembers[i].get_name() << "\"" << endl;
        cout << "  .Hp is " << ::BattleMembers[i].Hp << endl;
        cout << "  .get_steps() is " << ::BattleMembers[i].get_steps() << endl;
        cout << "  .get_meleeDmg() is " << ::BattleMembers[i].get_meleeDmg() << endl;
        cout << "  .get_rangeDmg() is " << ::BattleMembers[i].get_rangeDmg() << endl;
        cout << "  .get_rangeDistance() is " << ::BattleMembers[i].get_rangeDistance() << endl;
        cout << endl;
    }
}*/

void BotsTurn() {
    // This replaces second team players by bots.
    character ThisBot = GetCharacterByTurn();
    for (int i = 0; i < 8; i++) {
        ::Damage[i] = 0;
    }
    /*std::pair<int, int> EnYX[4] = {
        {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}
    };*/
    int EnDist[4] = {
        -1, -1, -1, -1
    };
    bool found_enemy[4] = { 0, 0, 0, 0 };
    std::vector<std::pair<int, int>> path[4];

    // Using BFS to find shortest path to enemy
    bool used[8][12];
    //int dst[8][12];
    std::pair<int, int> pr[8][12];
    const int y1 = YofGameBoardById(ThisBot.get_id());
    const int x1 = XofGameBoardById(ThisBot.get_id());

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            //dst[i][j] = -1;
            pr[i][j] = { -1, -1 };
            used[i][j] = false;
        }
    }

    std::queue<std::pair<int, int>> q;    //Мы храним уже не номера вершин, а координаты клеток.
    q.push({ y1, x1 });       //Предполагается использование С++11, в противном случае используйте make_std::pair
    used[y1][x1] = true;
    //dst[y1][x1] = 0;

    while (!q.empty()) {
        std::pair<int, int> cur = q.front();
        q.pop();

        int cy = cur.first, cx = cur.second, cid = ::GameBoard[cy][cx];



        if ((cid == 2 || cid == 3 || cid == 4 || cid == 5) && (::BattleMembers[cid - 2].get_hp() - ::Damage[cid - 2]) > 0) {
            //EnYX[cid] = {cy, cx};
            //EnDist[cid] = dst[cy][cx];
            //cout << "\nEnemy found, id is: " << cid << endl;
            found_enemy[cid - 2] = true;
            int cy1 = cy, cx1 = cx;
            std::pair<int, int> cpair = { pr[cy1][cx1].first, pr[cy1][cx1].second };
            while (cpair.first != y1 || cpair.second != x1) {   //пока существует предыдущая вершина
                cy1 = cpair.first;
                cx1 = cpair.second;        //переходим в неё
                path[cid - 2].push_back({ cy1, cx1 });    //и дописываем к пути
                cpair = { pr[cy1][cx1].first, pr[cy1][cx1].second };
            }
        }

        std::vector<std::pair<int, int>> moves{
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        for (auto move : moves) {
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
                            ::BattleMembers[cid - 2].get_hp() - ::Damage[cid - 2] <= 0
                            || cid == ThisBot.get_id()
                            )
                        )
                    )
                ) {
                q.push({ y, x });
                used[y][x] = true;
                //dst[y][x] = dst[cy][cx] + 1;
                pr[y][x] = { cy, cx };
            }
        }
    }

    //Восстановим кратчайший путь до вершины k (которую, предполагается, мы уже ввели)
    //Для восстановления пути пройдём его в обратном порядке, начиная с j, и развернём.
    for (int i = 0; i < 4; i++) {
        if (path[i].size() == 0) continue;
        reverse(path[i].begin(), path[i].end());
    }

    for (int i = 0; i < 4; i++) {
        if (found_enemy[i])  EnDist[i] = path[i].size();
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

    for (int i = 0; i < 4; i++) {
        if (EnDist[i] < CloseDist) {
            CloseDist = EnDist[i];
            CloseEnemyId = i + 2;
        }
    }

    if (CloseEnemyId == -1) {
        //cout << "\nCloseEnemyId == -1\n";
        return;
    }
    if (CloseDist == 0) {
        //cout << "\nCloseDist == 0\n";
        return;
    }


    if (CloseDist >= ThisBot.get_steps()) {
        ::GameBoard[y1][x1] = 0;
        std::pair<int, int> Lastpair = path[CloseEnemyId - 2][ThisBot.get_steps() - 1];
        ::GameBoard[Lastpair.first][Lastpair.second] = ThisBot.get_id();
    }
    else {
        ::GameBoard[y1][x1] = 0;
        std::pair<int, int> Lastpair = path[CloseEnemyId - 2].back();
        ::GameBoard[Lastpair.first][Lastpair.second] = ThisBot.get_id();
    }
}

void CollectStatistics() {
    // Stores data about game in ANALISYS.
    std::array<aCharData, 8> arr;
    int cHp;
    aCharData* el;

    for (int i = 0; i < 8; i++) {
        cHp = ::BattleMembers[i].get_hp();
        el = &arr[i];
        (*el).Hp = cHp;
        if (cHp > 0) {
            (*el).X = XofGameBoardById(i + 2);
            (*el).Y = YofGameBoardById(i + 2);
        }
        else {
            (*el).X = -1;  // is not a valid square id.
            (*el).Y = -1;
        }
    }
    ::aGameData.push_back(arr);
}

/*void DoAnalisys() {
    bool gotResp = false;
    while (!gotResp) {
        string resp;
        cout << "\nopen  game analisys menue?\n1.Yes\n2.No\n";
        cin >> resp;
        switch (resp[0]) {
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
}*/

/* OUTDATED
void ChooseTurnAI(const int state) {
    switch (state) {
    case 0:
        //PromptInput();
        //ProcessInput();
        break;
    case 1:
        BotsTurn();
        break;
    default:
        LPCWSTR temp = (L"Error in ChooseTurnAI():  unknown state \"" + std::to_wstring(state) + L"\"").c_str();
        MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
    }
}

void MakeTheGameWorkBots(std::wstring input) {
    // Main game loop. Name speaks for itself.
    while (true) {
        ProcessInput(input);
        CollectStatistics();
        if (TeamVictory() != 0) break;
        ChangeTurn();
        update_info();

        ChooseTurnAI(::Team2AI);
        CollectStatistics();
        if (TeamVictory() != 0) break;
        ChangeTurn();
        update_info();
    }
}

void MakeTheGameWorkPlayer(std::wstring input) {
    // Main game loop. Name speaks for itself.
    while (true) {
        ProcessInput(input);
        CollectStatistics();
        if (TeamVictory() != 0) break;
        ChangeTurn();
        update_info();
    }
}
*/

void MakeTurn(HWND parent_window, std::wstring input) {
    // Makes turn based on input
    switch (::Team2AI) {
    case 0:
        if (!ProcessInput(input)) break;
        CollectStatistics();
        if (TeamVictory() != 0) {
            DestroyWindow(parent_window);
            PostQuitMessage(0);
            break;
        }
        ChangeTurn();
        break;
    case 1:
        if (!ProcessInput(input)) break;
        CollectStatistics();
        if (TeamVictory() != 0) {
            DestroyWindow(parent_window);
            PostQuitMessage(0);
            break;
        }
        ChangeTurn();

        BotsTurn();
        CollectStatistics();
        if (TeamVictory() != 0) {
            DestroyWindow(parent_window);
            PostQuitMessage(0);
            break;
        }
        //DestroyWindow(parent_window)
        ChangeTurn();
        break;
    default:
        LPCWSTR temp = (L"Error in MakeTurn:  unknown AI state \"" + std::to_wstring(::Team2AI) + L"\"").c_str();
        MessageBox(0, temp, L"Error", MB_OK | MB_ICONERROR);
        break;
    }
}
