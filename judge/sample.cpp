#include "AIController.h"
#include <utility>
#include <cstring>
#include <vector>
extern int ai_side; //0: black, 1: white
std::string ai_name = "your_ai_name_here";

int turn = 0;
int board[15][15];

struct ContinousChesses {
    int colour; //哪一方的
    int num; //长度
    int type; //两端被堵了几端
    int dir; //0向右，1向上，2向右上，3向右下
    std::pair<int, int> start; //起点
    ContinousChesses(int c, int n, int t, int d, std::pair<int, int> s): colour(c), num(n), dir(d), type(t), start(std::move(s)){}
};

struct Direction {
    int dx;
    int dy;
}d[4];

int score[2][20][2]; //第一维表示现在轮到哪一方，第三维表示什么颜色的连子，第二维：
/*  0: 5
    1：活4
    2：死4
    3：活3
    4：死3
    5：活2
    6：死2
    7：活1
    8: 死1
*/

//init function is called once at the beginning
void init() {
	/* TODO: Replace this by your code */
	memset(board, -1, sizeof(board));
    d[0] = {0, 1}; //0向右
    d[1] = {1, 0}; //1向下
    d[2] = {-1, 1}; //2向右上
    d[3] = {1, 1}; //3向右下
    score[0][0][0] = 10000000, score[0][0][1] = -1000000;
    score[0][1][0] = 600000, score[0][1][1] = -300000;
    score[0][2][0] = 600000, score[0][2][1] = -200000;
    score[0][3][0] = 100000, score[0][3][1] = -50000;
    score[0][4][0] = 20000, score[0][4][1] = -10000;
    score[0][5][0] = 10000, score[0][5][1] = -5000;
    score[0][6][0] = 1000, score[0][6][1] = -500;
    score[0][7][0] = 50, score[0][7][1] = -25;
    score[0][8][0] = 20, score[0][8][1] = -10;
    for(int j = 0; j < 9; j++)
        for(int k = 0; k < 2; k++)
            score[1][j][k] = -score[0][j][1 - k];
}

// loc is the action of your opponent
// Initially, loc being (-1,-1) means it's your first move
// If this is the third step(with 2 black ), where you can use the swap rule, your output could be either (-1, -1) to indicate that you choose a swap, or a coordinate (x,y) as normal.

bool Inboard(int x, int y) {
    return x >= 0 && x < 15 && y >= 0 && y < 15;
}

std::vector<ContinousChesses> Find(int num, int colour) { //可以优化，一次dir把所有长度的搜光
    std::vector<ContinousChesses> ret;
    for(int i = 0; i < 15; i++)
        for(int j = 0; j < 15; j++)
        {
            for(int dir = 0; dir < 4; dir++)
            {
                int x = i, y = j;
                bool ans = true;
                for(int cnt = 0; cnt < num; cnt++)
                {
                    if(!Inboard(x, y) || board[x][y] != colour)
                    {
                        ans = false;
                        break;
                    }
                    x += d[dir].dx;
                    y += d[dir].dy;
                }
                if(ans)
                {
                    if(Inboard(x, y) && board[x][y] == colour)
                         continue;
                    if(Inboard(i - d[dir].dx, j - d[dir].dy) && board[i - d[dir].dx][j - d[dir].dy] == colour)
                         continue;

                    int type = 2;
                    if(Inboard(x, y) && board[x][y] == -1)
                        type--;
                    if(Inboard(i - d[dir].dx, j - d[dir].dy) && board[i - d[dir].dx][j - d[dir].dy] == -1)
                        type--;
                    ret.push_back(ContinousChesses(colour, num, type, dir, std::make_pair(i, j)));
                }
            }
        }
    return ret;
}

int judge_final() { //返回哪一方赢
    bool black_wins = Find(5, 0).empty();
    if(black_wins)
        return 0;
    bool white_wins = Find(5, 1).empty();
    if(white_wins)
        return 1;
    return -1;
}

int getScore(int t) {        //black is to maxmize it
    int ans = 0;
    for(int colour = 0; colour < 2; colour++)
        for(int num = 1; num <= 5; num++)
        {
            auto ret = Find(num, colour);
            if (num == 5)
            {
                ans += ret.size() * score[t][0][colour];
            }
            else
            {
                for (int i = 0; i < ret.size(); i++)
                {
                    if (ret[i].type == 0)
                        ans += score[t][2 * (5 - num) - 1][colour];
                    else if (ret[i].type == 1)
                        ans += score[t][2 * (5 - num)][colour];
                }
            }
        }
    return ans;
}

int depth_limit = 1;
const int INF = 1e8;




int getMaxValue(int dep, int Min, int Max);
int getMinValue(int dep, int Min, int Max) {
    if(dep > depth_limit)
    {
        return getScore(1);
    }

    for(int x = 0; x < 15; x++)
        for(int y = 0; y < 15; y++)
        {
            if(board[x][y] == -1)
            {
                board[x][y] = 1;
                int downMax = getMaxValue(dep + 1, Min, Max);
                if(downMax < Min)
                {
                    Min = downMax;
                }
                board[x][y] = -1;
                if(Min <= Max)  //alpha-beta pruning
                {
                    return Min;
                }
            }
        }
    return Min;
}

int getMaxValue(int dep, int Min, int Max) {
    if(dep > depth_limit)
    {
        return getScore(0);
    }
    for(int x = 0; x < 15; x++)
        for(int y = 0; y < 15; y++)
        {
            if(board[x][y] == -1)
            {
                board[x][y] = 0;
                int downMin = getMinValue(dep + 1, Min, Max);
                if(downMin > Max)
                {
                    Max = downMin;
                }
                board[x][y] = -1;
                if(Max >= Min)  //alpha-beta pruning
                {
                    return Max;
                }
            }
        }
    return Max;
}

std::pair<int, int> action(std::pair<int, int> loc) {
	/* TODO: Replace this by your code */
    turn++;
    board[loc.first][loc.second] = 1 - ai_side;
    if(ai_side == 0)
    {
        int ret_x = -2, ret_y = -2;
        int ans = -INF;
        for(int x = 0; x < 15; x++)
            for(int y = 0; y < 15; y++)
            {
                if(board[x][y] == -1)
                {
                    board[x][y] = 0;
                    int downMin = getMinValue(0, INF, ans);
                    if(downMin > ans)
                    {
                        ans = downMin;
                        ret_x = x, ret_y = y;
                    }
                    board[x][y] = -1;
                }
            }
        board[ret_x][ret_y] = 0;
        return std::make_pair(ret_x, ret_y);
    }
    else
    {
        int ret_x = -2, ret_y = -2;
        int ans = INF;
        for(int x = 0; x < 15; x++)
            for(int y = 0; y < 15; y++)
            {
                if(board[x][y] == -1)
                {
                    board[x][y] = 1;
                    int downMax = getMaxValue(0, ans, -INF);
                    if(downMax < ans)
                    {
                        ans = downMax;
                        ret_x = x, ret_y = y;
                    }
                    board[x][y] = -1;
                }
            }
        board[ret_x][ret_y] = 1;
        return std::make_pair(ret_x, ret_y);
    }
}