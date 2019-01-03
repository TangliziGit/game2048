#ifndef _GAME2048_H_
#define _GAME2048_H_

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <termio.h>
#include <iostream>
#include <armadillo>
using namespace arma;

class Game2048{
public:
    Game2048(int boardSize=3);
    void run(void);
private:
    enum{DirLeft, DirUp, DirRight, DirDown, DirNothing};
    enum{DisWin, DisLose, DisNormal};
    enum{RotNormal, RotReverse};

    static const int MaxBoardSize=10, MaxPowSize=17;
    static const int CriterionWin=11;
    static const int NumWidth=6;
    static char SplitChar;
    static int Direct[4][2];
    static char WinSlogan[50], LoseSlogan[50];
    static imat RotateMatrix[4];
    static ivec RotateVector[4];
    static int pow[MaxPowSize];

    imat _points;
    int board[MaxBoardSize][MaxBoardSize];
    int boardSize, score, itemCount;
    bool winFlag;

    int getDirect(void);
    int getScore(void);
    int getItemCount(void);
    void display(int);

    void actions(int);
    void randCreate(void);
    void rotate(int, int);
    int _findNextItem(int, int);
    bool checkWin(void);
    bool checkLose(void);

    char getKeyboardChar(void);
};

int Game2048::pow[Game2048::MaxPowSize]={0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
                    2048, 4096, 8192, 16384, 32768, 65536};
char Game2048::SplitChar='|';
int  Game2048::Direct[4][2]={1,0,0,1,-1,0,0,-1};
char Game2048::WinSlogan[50] ="Congratulations! You Are Winner!",
     Game2048::LoseSlogan[50]="You Losed.";
imat Game2048::RotateMatrix[4]={"1 0; 0 1;",    // Left
                  "0 1;-1 0;",    // Up
                  "-1 0;0 -1;",   // Right
                  "0 -1;1 0;"};   // Down
ivec Game2048::RotateVector[4]={"0 0", "0 1",
                            "1 1", "1 0"};

Game2048::Game2048(int boardSize):
        boardSize(boardSize), score(0), itemCount(0), winFlag(false){
    srand((int)time(NULL));
    memset(board, 0, sizeof(board));
    for (int y=0; y<boardSize; y++)
        for (int x=0; x<boardSize; x++){
            imat tmp;
            tmp << x << endr << y << endr;
            _points.insert_cols(_points.n_cols, tmp);
        }
}

void Game2048::run(void){
    int direct;

    randCreate();
    display(DisNormal);
    while (true){
        direct=getDirect();
        if (direct==DirNothing) continue;
        actions(direct);

        if (checkWin()){
            randCreate();
            display(DisWin);
        }else if (checkLose()){
            display(DisLose);
            break;
        }else{
            randCreate();
            display(DisNormal);
        }
    }
};

void Game2048::rotate(int direct, int type){
    int boardTmp[MaxBoardSize][MaxBoardSize];
    if (type==RotNormal) direct=(4-direct)%4;

    int ptr=0;
    // cout << RotateMatrix[direct].size() << " " << _points.size() << endl;
    imat points=RotateMatrix[direct]*_points;
    ivec shift =(boardSize-1)*RotateVector[direct];
    for (int y=0; y<boardSize; y++)
        for (int x=0; x<boardSize; x++){
            int xx=points(0, ptr)+shift(0), yy=points(1, ptr)+shift(1);
            boardTmp[y][x]=board[yy][xx];
            ptr++;
        }

    // points.print("\npoints!");
    // _points.print("\n_points!");
    // printf("shift %d %d", shift(0), shift(1));

    memcpy(board, boardTmp, sizeof(boardTmp));
}

int Game2048::_findNextItem(int x, int y){
    x++;
    for (; x<boardSize; x++)
        if (board[y][x]!=0) return x;
    return -1;
}

void Game2048::actions(int direct){
    // the item in board, which means the power of 2.
    // and the rotate option helps reduce the amount of code.
    rotate(direct, RotNormal);

    // display(DisNormal);

    for (int y=0; y<boardSize; y++){
        int *line=board[y];
        // int sta[MaxBoardSize], staSize=0;
        int que[MaxBoardSize], queSize=0;
        for (int ptr=_findNextItem(-1, y), pptr=_findNextItem(ptr, y);
                ptr!=-1;){
            if (pptr==-1 && ptr!=-1){
                if (line[ptr]!=0) que[queSize++]=line[ptr];
                line[ptr]=0;
                break;
            }else if (line[ptr]==line[pptr]){
                score+=line[ptr];

                que[queSize++]=line[ptr]+1;
                line[pptr]=line[ptr]=0;
                itemCount--;

                ptr=_findNextItem(pptr, y);
                pptr=_findNextItem(ptr, y);
            }else{
                que[queSize++]=line[ptr];
                // que[queSize++]=line[ptr];
                line[ptr]=0;

                ptr=pptr;
                pptr=_findNextItem(pptr, y);
            }
        }
        // for (int i=0; i<queSize; i++)
        //     printf("%d ", que[i]);
        // printf("--- \n");
        
        int ptr=0;
        while (ptr<queSize){
            line[ptr]=que[ptr];
            ptr++;
        }
    }

    // display(DisNormal);

    rotate(direct, RotReverse);
}

void Game2048::randCreate(void){
    int x, y;
    while (true){
        x=rand()%boardSize, y=rand()%boardSize;
        if (board[y][x]==0){
            board[y][x]=1;      // TODO: rand it rationally
            itemCount++;
            break;
        }
    }
}

int Game2048::getDirect(void){
    char input=getKeyboardChar();
    int direct;

    switch (input){
        case 'w': direct=DirUp;
                  break;
        case 'a': direct=DirLeft;
                  break;
        case 's': direct=DirDown;
                  break;
        case 'd': direct=DirRight;
                  break;
        default:  direct=DirNothing;
    }
    return direct;
}


void Game2048::display(int type){
    system("clear");
    if (type==DisNormal){
        printf("\n");
        for (int y=0; y<boardSize; y++){
            for (int x=0; x<boardSize; x++){
                if (board[y][x])
                    printf("%c%*d", SplitChar, NumWidth, pow[board[y][x]]);
                else
                    printf("%c%*c", SplitChar, NumWidth, ' ');
            }
            printf("%c\n", SplitChar);
        }
        printf("Score: %d\nItemCount: %d\n", getScore(), getItemCount());
    }else if (type==DisWin){
        printf("\n\n%s\n", WinSlogan);
        display(DisNormal);
    }else if (type==DisLose){
        printf("\n\n%s\n", LoseSlogan);
        display(DisNormal);
    }
}

int Game2048::getItemCount(void){
    return itemCount;
}

int Game2048::getScore(void){
    return score;
}

bool Game2048::checkWin(void){
    if (winFlag) return false;
    for (int y=0; y<boardSize; y++)
        for (int x=0; x<boardSize; x++)
            if (board[y][x]>=CriterionWin) winFlag=true;
    return winFlag;
}

bool Game2048::checkLose(void){
    if (itemCount!=boardSize*boardSize) return false;
    bool loseFlag=true;
    for (int y=0; y<boardSize; y++){
        for (int x=0; x<boardSize; x++){
            for (int dir=0; dir<4; dir++){
                int xx=x+Direct[dir][0], yy=y+Direct[dir][1];
                
                if (xx<0 || yy<0 || xx>=boardSize || yy>=boardSize) continue;
                if (board[y][x]==board[yy][xx]){
                    loseFlag=false;
                    break;
                }
            }
        }
        if (!loseFlag) break;
    }
    return loseFlag;
}

char Game2048::getKeyboardChar(){
    char in;
    struct termios new_settings;
    struct termios stored_settings;

    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);

    in=getchar();
    tcsetattr(0,TCSANOW,&stored_settings);
    return in;
}


#endif // _GAME2048_H_
