#include "AIController.h"
#include <utility>
#include <cstring>
#include <vector>
#include <algorithm>
extern int ai_side; //0: black, 1: white
std::string ai_name = "your_ai_name_here";

int turn = 0;
int board[15][15];

struct Direction {
    int dx;
    int dy;
}d[4];

long long score[2][20][2]; //第一维表示现在轮到哪一方，第三维表示什么颜色的连子，第二维：
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
    score[0][0][0] = 10000000, score[0][0][1] = -1000000; //5
    score[0][1][0] = 600000, score[0][1][1] = -300000;  //活4
    score[0][2][0] = 600000, score[0][2][1] = -200000;  //死4
    score[0][3][0] = 100000, score[0][3][1] = -50000;   //活3
    score[0][4][0] = 20000, score[0][4][1] = -10000;   //死3
    score[0][5][0] = 10000, score[0][5][1] = -5000;    //活2
    score[0][6][0] = 1000, score[0][6][1] = -500;     //死2
    score[0][7][0] = 50, score[0][7][1] = -25;         //活1
    score[0][8][0] = 20, score[0][8][1] = -10;        //死1
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

int GetScoreOfContinousChesses(int len, int colour, int type, int perspective)
{
    if(len >= 5)
    {
        return score[perspective][0][colour];
    }
    else
    {
        if(type == 0)
        {
            return score[perspective][2 * (5 - len) - 1][colour];
        }
        else if(type == 1)
        {
            return score[perspective][2 * (5 - len)][colour];
        }
        else
        {
            return 0;
        }
    }
}

int OneStepChange(int new_x, int new_y, int new_colour, int owner_colour)
{
    int ans = 0;
    int x_copy = new_x, y_copy = new_y;
    for(int dir = 0; dir < 4; dir++) {
        new_x = x_copy, new_y = y_copy;
        int len_plus = 0, len_minus = 0;
        int i = new_x, j = new_y;
        int dx = d[dir].dx, dy = d[dir].dy;

        //新加的
        while (Inboard(new_x, new_y) && board[new_x][new_y] == new_colour) {
            len_plus++;
            new_x += dx, new_y += dy;
        }
        while (Inboard(i, j) && board[i][j] == new_colour) {
            len_minus++;
            i -= dx, j -= dy;
        }
        int type_plus = 1, type_minus = 1;
        if (Inboard(new_x, new_y) && board[new_x][new_y] == -1) {
            type_plus--;
        }
        ans -= GetScoreOfContinousChesses(len_plus - 1, new_colour, type_plus, owner_colour);
        if (Inboard(i, j) && board[i][j] == -1) {
            type_minus--;
        }
        ans -= GetScoreOfContinousChesses(len_minus - 1, new_colour, type_minus, owner_colour);
        ans += GetScoreOfContinousChesses(len_plus + len_minus - 1, new_colour, type_plus + type_minus, owner_colour);

        //扣掉的
        if (len_plus == 1) {
            int l_other = 0;
            while (Inboard(new_x, new_y) && board[new_x][new_y] == 1 - new_colour) {
                l_other++;
                new_x += dx, new_y += dy;
            }
            int type_other = 1;
            if (Inboard(new_x, new_y) && board[new_x][new_y] == -1) {
                type_other--;
            }
            ans += GetScoreOfContinousChesses(l_other, 1 - new_colour, type_other + 1, owner_colour) -
                   GetScoreOfContinousChesses(l_other, 1 - new_colour, type_other, owner_colour);
        }
        if(len_minus == 1) {
            int l_other = 0;
            while (Inboard(i, j) && board[i][j] == 1 - new_colour) {
                l_other++;
                i -= dx, j -= dy;
            }
            int type_other = 1;
            if (Inboard(i, j) && board[i][j] == -1) {
                type_other--;
            }
            ans += GetScoreOfContinousChesses(l_other, 1 - new_colour, type_other + 1, owner_colour) -
                   GetScoreOfContinousChesses(l_other, 1 - new_colour, type_other, owner_colour);
        }
    }
    return ans;
}

int UpdateScore(int x_yours, int y_yours, int x_his, int y_his, int your_colour, int YourFacingLastScore) //计算新的得分
{
    board[x_his][y_his] = -1;
    int delta1 = OneStepChange(x_yours, y_yours, your_colour, your_colour);
    board[x_his][y_his] = 1 - your_colour;
    int delta2 = OneStepChange(x_his, y_his, 1 - your_colour, your_colour);
    return YourFacingLastScore + delta1 + delta2;
}



std::vector<std::pair<int, int>> C3DetectionAndReaction() {

}


int depth_limit()
{
    if(turn == 2)
    {
        return 3;
    }
    else
    {
        return 5;
    }

}
const int INF = 1e9;

int minX = 15, maxX = -1, minY = 15, maxY = -1;
void UpdateArea(int x, int y) {
    minX = std::max(std::min(minX, x - 3), 0);
    maxX = std::min(std::max(maxX, x + 3), 14);
    minY = std::max(std::min(minY, y - 3), 0);
    maxY = std::min(std::max(maxY, y + 3), 14);
}

int getMaxValue(int dep, int Min, int Max, int MyFacingScore, int HisFacingScore, int his_x, int his_y, int minX, int maxX, int minY, int maxY);
int getMinValue(int dep, int Min, int Max, int MyFacingScore, int HisFacingScore, int his_x, int his_y, int minX, int maxX, int minY, int maxY) {
    if(dep > depth_limit())
    {
        return MyFacingScore;
    }
    std::vector<std::pair<int, std::pair<int, int>>> scores;
    for(int x = minX; x <= maxX; x++)
    {
        for(int y = minY; y <= maxY; y++)
        {
            if(board[x][y] == -1)
            {
                board[x][y] = 1;
                scores.emplace_back(std::make_pair(UpdateScore(his_x, his_y, x, y, 0, HisFacingScore), std::make_pair(x, y)));
                board[x][y] = -1;
            }
        }
    }
    std::sort(scores.begin(), scores.end(), [](auto a, auto b) {
        return a.first < b.first;
    });

    for(int i = 0; i < std::min(40, (int)scores.size()); i++)
    {
        auto u = scores[i];
        int x = u.second.first, y = u.second.second;
        board[x][y] = 1;
        int downMax = getMaxValue(dep + 1, Min, Max, u.first, MyFacingScore, x, y, 
                                  std::max(std::min(minX, x - 3), 0), maxX = std::min(std::max(maxX, x + 3), 14), 
                                  minY = std::max(std::min(minY, y - 3), 0), maxY = std::min(std::max(maxY, y + 3), 14));
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
    return Min;
}

int getMaxValue(int dep, int Min, int Max, int MyFacingScore, int HisFacingScore, int his_x, int his_y, int minX, int maxX, int minY, int maxY) {  //I am 0
    if(dep > depth_limit())
    {
        return MyFacingScore;
    }
    std::vector<std::pair<int, std::pair<int, int>>> scores;
    for(int x = minX; x <= maxX; x++)
    {
        for(int y = minY; y <= maxY; y++)
        {
            if(board[x][y] == -1)
            {
                board[x][y] = 0;
                scores.emplace_back(std::make_pair(UpdateScore(his_x, his_y, x, y, 1, HisFacingScore), std::make_pair(x, y)));
                board[x][y] = -1;
            }
        }
    }
    std::sort(scores.begin(), scores.end(), [](auto x, auto y){
        return x.first > y.first;
    });
    for(int i = 0; i < std::min(40, (int)scores.size()); i++)
    {
        auto u = scores[i];
        int x = u.second.first, y = u.second.second;
        if(board[x][y] == -1)
        {
            board[x][y] = 0;
            int downMin = getMinValue(dep + 1, Min, Max, u.first, MyFacingScore, x, y,
                                      std::max(std::min(minX, x - 3), 0), maxX = std::min(std::max(maxX, x + 3), 14), 
                                      minY = std::max(std::min(minY, y - 3), 0), maxY = std::min(std::max(maxY, y + 3), 14));
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


int my_last_x = -1, my_last_y = -1;
int MyFacingScore = 0;
int HisFacingScore = 0;

std::pair<int, int> FlippingMemory;

std::pair<int, int> first_step(std::pair<int, int> loc) {
    turn++;
    int his_x = loc.first, his_y = loc.second;
    UpdateArea(his_x, his_y);
    board[his_x][his_y] = 1 - ai_side;
    if(ai_side == 0)
    {
        UpdateArea(7, 7);
        my_last_x = 7;
        my_last_y = 7;
        board[my_last_x][my_last_y] = 0;
        HisFacingScore = 4 * score[1][7][0];
    }
    else
    {
        if((his_x == 0 && his_y == 1) || (his_x == 0 && his_y == 14) || (his_x == 14 && his_y == 0) || (his_x == 14 && his_y == 14))
        {
            MyFacingScore = 3 * score[1][8][0];
        }
        else if(his_x == 0 || his_x == 14 || his_y == 0 || his_y == 14)
        {
            MyFacingScore = 3 * score[1][8][0] + score[1][7][0];
        }
        else
        {
            MyFacingScore = 4 * score[1][7][0];
        }
        bool he_is_left = false, he_is_upper = false;
        if(his_x < 7)
            he_is_left = true;
        if(his_y < 7)
            he_is_upper = true;
        if(he_is_left && he_is_upper)
        {
            my_last_x = his_x + 1;
            my_last_y = his_y + 1;
        }
        else if(he_is_left)
        {
            my_last_x = his_x + 1;
            my_last_y = his_y;
        }
        else if(he_is_upper)
        {
            my_last_x = his_x;
            my_last_y = his_y + 1;
        }
        else
        {
            my_last_x = his_x - 1;
            my_last_y = his_y;
        }
        UpdateArea(my_last_x, my_last_y);
        board[my_last_x][my_last_y] = 1;
        HisFacingScore = UpdateScore(his_x, his_y, my_last_x, my_last_y, 0, HisFacingScore);
    }
    return std::make_pair(my_last_x, my_last_y);
}


std::pair<int, int> action(std::pair<int, int> loc) {
    if(turn == 0)
    {
        return first_step(loc);
    }
    else if(ai_side == 0 && turn == 1)
    {
        turn++;
        FlippingMemory = loc;
        int his_x = loc.first, his_y = loc.second;
        UpdateArea(his_x, his_y);
        board[his_x][his_y] = 1 - ai_side;
        MyFacingScore = UpdateScore(my_last_x, my_last_y, his_x, his_y, ai_side, MyFacingScore);
        my_last_x = 14 - his_x;
        my_last_y = 14 - his_y;
        HisFacingScore = UpdateScore(his_x, his_y, my_last_x, my_last_y, 1 - ai_side, HisFacingScore);
        board[my_last_x][my_last_y] = 0;
        UpdateArea(my_last_x, my_last_y);
        return std::make_pair(my_last_x, my_last_y);
    }
    else
    {
        turn++;
        int his_x = loc.first, his_y = loc.second;

        //换手，换手时需重算分数

        // if(his_x == -1 && his_y == -1)
        // {
        //     board[7][7] = 1;
        //     board[FlippingMemory.first][FlippingMemory.second] = 0;
        //     board[14 - FlippingMemory.first][14 - FlippingMemory.second] = 1;
        //     MyFacingScore = 
        //     HisFacingScore = UpdateScore(my_last_x, my_last_y, his_x, his_y, ai_side, );

        // }
        // else
        // {
            UpdateArea(his_x, his_y);
            board[his_x][his_y] = 1 - ai_side;
            MyFacingScore = UpdateScore(my_last_x, my_last_y, his_x, his_y, ai_side, MyFacingScore);
        // }
        if(ai_side == 0)
        {
            int ret_x = -2, ret_y = -2;
            int ans = -INF;
            std::vector<std::pair<int, std::pair<int, int>>> scores;
            for(int x = minX; x <= maxX; x++)
            {
                for(int y = minY; y <= maxY; y++)
                {
                    if(board[x][y] == -1)
                    {
                        board[x][y] = 0;
                        scores.emplace_back(std::make_pair(UpdateScore(his_x, his_y, x, y, 1, HisFacingScore), std::make_pair(x, y)));
                        board[x][y] = -1;
                    }
                }
            }
            std::sort(scores.begin(), scores.end(), [](auto x, auto y){
                return x.first > y.first;
            });
            for(int i = 0; i < std::min(40, (int)scores.size()); i++)
            {
                auto u = scores[i];
                int x = u.second.first, y = u.second.second;
                if(board[x][y] == -1)
                {
                    board[x][y] = 0;
                    int downMin = getMinValue(0, INF, ans, u.first, MyFacingScore, x, y,
                                              std::max(std::min(minX, x - 3), 0), maxX = std::min(std::max(maxX, x + 3), 14), 
                                              minY = std::max(std::min(minY, y - 3), 0), maxY = std::min(std::max(maxY, y + 3), 14));
                    if(downMin > ans)
                    {
                        ans = downMin;
                        ret_x = x, ret_y = y;
                    }
                    board[x][y] = -1;
                }
            }
            HisFacingScore = UpdateScore(his_x, his_y, ret_x, ret_y, 1 - ai_side, HisFacingScore);
            my_last_x = ret_x, my_last_y = ret_y;
            board[ret_x][ret_y] = 0;
            UpdateArea(ret_x, ret_y);
            return std::make_pair(ret_x, ret_y);
        }
        else
        {
            int ret_x = -2, ret_y = -2;
            int ans = INF;
            std::vector<std::pair<int, std::pair<int, int>>> scores;
            for(int x = minX; x <= maxX; x++)
            {
                for(int y = minY; y <= maxY; y++)
                {
                    if(board[x][y] == -1)
                    {
                        board[x][y] = 1;
                        scores.emplace_back(std::make_pair(UpdateScore(his_x, his_y, x, y, 0, HisFacingScore), std::make_pair(x, y)));
                        board[x][y] = -1;
                    }
                }
            }
            std::sort(scores.begin(), scores.end(), [](auto a, auto b) {
                return a.first < b.first;
            });

            for(int i = 0; i < std::min(40, (int)scores.size()); i++)
            {
                auto u = scores[i];
                int x = u.second.first, y = u.second.second;
                board[x][y] = 1;
                int downMax = getMaxValue(0, ans, -INF, u.first, MyFacingScore, x, y,
                                          std::max(std::min(minX, x - 3), 0), maxX = std::min(std::max(maxX, x + 3), 14), 
                                          minY = std::max(std::min(minY, y - 3), 0), maxY = std::min(std::max(maxY, y + 3), 14));
//                std::cout << downMax << '!' << std::endl;
                if(downMax < ans)
                {
                    ans = downMax;
                    ret_x = x, ret_y = y;
                }
                board[x][y] = -1;
            }
            HisFacingScore = UpdateScore(his_x, his_y, ret_x, ret_y, 1 - ai_side, HisFacingScore);
            my_last_x = ret_x, my_last_y = ret_y;
            board[ret_x][ret_y] = 1;
            UpdateArea(ret_x, ret_y);
            return std::make_pair(ret_x, ret_y);
        }
    }
}