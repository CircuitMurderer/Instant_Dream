// Copyright(C) 2019 2019302180085 Zhao Yuqi incorporated. All rights reserved.
// A C Program game: Instant Dream.
#pragma warning(disable : 4996)
#pragma warning(disable : 6031)
#pragma comment(lib, "winmm.lib")

#include <conio.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define BLANK 1
#define WALL -1
#define FOOD 0
#define LOSE -1
#define WIN 1
#define N 20

typedef struct coor { //坐标
    int x;
    int y;
} coor;

typedef struct rec { //记录
    float timecost;
    int hour;
    int min;
    int sec;
    int year;
    int mon;
    int day;
} rec;

coor player, *monster; //坐标声明
clock_t start[3], end[3], pd[3]; //计时
HANDLE Loc = GetStdHandle(STD_OUTPUT_HANDLE); //定位光标句柄
enum direction { up = -2,
    down,
    left,
    right }; //方向枚举
int map[N][N], eaten, foodcount, wincount, *dir; //地图相关变量

void pos(SHORT x, SHORT y) { //定位光标
    COORD p = { x, y };
    SetConsoleCursorPosition(Loc, p);
}

void color(int a) { //颜色设置
    SetConsoleTextAttribute(Loc, a);
}

void Menu(); //菜单
void Map1(); //地图数据
void Map2(); //
void Map3(); //
void Guide(); //游戏说明
void CurCall(); //通关谢幕
void Control(); //玩家移动
void PrintMap(); //打印地图
int WinorLose(); //胜负判断
void RankList(); //排行榜
int DifAdjust(int); //难度选择
void GameRecord(float); //游戏记录
void Operate(int, int); //实体操作
void MonsMove(coor*, int*); //怪物移动算法
void MonsChase(coor*, int*); //怪物追逐算法
int ListWay(const void*, const void*); //排序方式

int main() {
    CONSOLE_CURSOR_INFO cc; //隐藏光标
    cc.bVisible = 0;
    cc.dwSize = 1;
    SetConsoleCursorInfo(Loc, &cc);

    dir = (int*)malloc(2 * sizeof(int));
    monster = (coor*)malloc(2 * sizeof(coor));
    int speed = 2, course = 1; //速度和进程

loop0:
    Menu(); //主界面
    wincount = 0; //重置胜场
    switch (getch()) { //菜单选项
    case 97:
        break;
    case 98:
        Guide();
        goto loop0;
        break;
    case 99:
        speed = DifAdjust(speed);
        goto loop0;
        break;
    case 100:
        RankList();
        goto loop0;
        break;
    case 101:
        goto loop3;
        break;
    default:
        goto loop0;
        break;
    }
loop1:
    system("cls");
    switch (wincount) { //根据过关次数更换关卡
    case 0:
        Map1();
        break;
    case 1:
        Map2();
        break;
    case 2:
        Map3();
        break;
    case 3:
        CurCall();
        goto loop0;
        break;
    default:
        break;
    }
    start[wincount] = clock(); //开始计时
    while (1) { //核心语句
        Operate(speed, course);
        PrintMap();
        if (WinorLose() == WIN) { //判断胜负
            wincount++;
            break;
        } else if (WinorLose() == LOSE)
            break;
        else
            course++;
        Sleep(10);
    }
loop2:
    switch (getch()) {
    case 97:
        goto loop1;
        break;
    case 98:
        system("cls");
        goto loop0;
        break;
    default:
        goto loop2;
    }
loop3:
    system("cls");
    color(14);
    pos(52, 12);
    printf("后会有期");
    free(monster);
    free(dir);
    Sleep(1000);
    pos(0, 20);
    return 0;
}

void Control() {
    if (kbhit()) {
        switch (getch()) {
        case 119:
            if (map[player.y - 1][player.x] != WALL) {
                if (map[player.y - 1][player.x] == FOOD)
                    eaten++;
                map[player.y][player.x] = BLANK;
                player.y--;
            }
            break;
        case 97:
            if (map[player.y][player.x - 1] != WALL) {
                if (map[player.y][player.x - 1] == FOOD)
                    eaten++;
                map[player.y][player.x] = BLANK;
                player.x--;
            }
            break;
        case 100:
            if (map[player.y][player.x + 1] != WALL) {
                if (map[player.y][player.x + 1] == FOOD)
                    eaten++;
                map[player.y][player.x] = BLANK;
                player.x++;
            }
            break;
        case 115:
            if (map[player.y + 1][player.x] != WALL) {
                if (map[player.y + 1][player.x] == FOOD)
                    eaten++;
                map[player.y][player.x] = BLANK;
                player.y++;
            }
            break;
        case 32:
            getch();
            break;
        default:
            break;
        }
    }
    fflush(stdin);
}

void MonsMove(coor* mon, int* dir) {
    if (*dir % 2 == 0 && *dir >= 0) {
        if (map[mon->y][mon->x - 1] != WALL) {
            mon->x--;
        } else {
            *dir += 1;
            mon->x++;
        }
    } else if (*dir % 2 != 0 && *dir >= 0) {
        if (map[mon->y][mon->x + 1] != WALL) {
            mon->x++;
        } else {
            *dir += 1;
            mon->x--;
        }
    } else if (*dir % 2 != 0 && *dir < 0) {
        if (map[mon->y + 1][mon->x] != WALL) {
            mon->y++;
        } else {
            *dir -= 1;
            mon->y--;
        }
    } else if (*dir % 2 == 0 && *dir < 0) {
        if (map[mon->y - 1][mon->x] != WALL) {
            mon->y--;
        } else {
            *dir -= 1;
            mon->y++;
        }
    } else
        ;
}

void PrintMap() {
    int i, j;
    for (i = 0; i < N; i++) {
        pos(22, i + 4);
        for (j = 0; j < N; j++) {
            if (i == monster->y && j == monster->x || i == (monster + 1)->y && j == (monster + 1)->x) {
                color(12);
                printf("◆");
            } else if (i == player.y && j == player.x) {
                color(14);
                printf("★");
            } else
                switch (map[i][j]) {
                case BLANK:
                    printf("  ");
                    break;
                case FOOD: {
                    color(7);
                    printf("·");
                    break;
                }
                case WALL: {
                    color(3);
                    printf("■");
                    break;
                }
                }
        }
        color(11);
        pos(66, i + 4);
        printf("┆");
    }
    pos(72, 6);
    printf("┌-----------------------------┐");
    pos(72, 10);
    printf("└-----------------------------┘");
    for (i = 7; i < 10; i++) {
        pos(72, i);
        printf("┆");
        pos(102, i);
        printf("┆");
    }
    color(14);
    pos(76, 8);
    printf("已收集的梦境碎片 %3d/%3d", eaten, foodcount);
    pos(74, 13);
    printf("★----你           W----上移");
    pos(74, 14);
    printf("◆----梦魇         A----左移");
    pos(74, 15);
    printf("■----墙壁         S----下移");
    pos(74, 16);
    printf("·----梦境碎片     D----右移");
    pos(74, 18);
    printf("可按空格键暂停,再按任意键恢复。");
    pos(74, 21);
    if ((eaten + 1) % 10 == 0) {
        color(4);
        printf("梦见秣陵惆怅事，桃花柳絮满江城。");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 9 == 0) {
        color(2);
        printf("撩乱春愁如柳絮，依依梦里无寻处。");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 8 == 0) {
        color(3);
        printf("  世事漫随流水，算来一梦浮生。  ");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 7 == 0) {
        color(5);
        printf("   梦里不知身是客，一晌贪欢。   ");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 6 == 0) {
        color(6);
        printf("  梦魂纵有也成虚。那堪和梦无。  ");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 5 == 0) {
        color(11);
        printf("醉后不知天在水，满船清梦压星河。");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 4 == 0) {
        color(13);
        printf("别绪如丝梦不成，那堪孤枕梦边城。");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else if ((eaten + 1) % 3 == 0) {
        color(14);
        printf("故欹单枕梦中寻，梦又不成灯又烬。");
        pos(72, 20);
        printf("▽");
        pos(105, 22);
        printf("△");
    } else
        ;
}

int WinorLose() {
    int i, j;
    if (eaten == foodcount) {
        end[wincount] = clock();
        pd[wincount] = end[wincount] - start[wincount];
        for (i = 0; i < N; i++) {
            pos(22, i + 4);
            for (j = 0; j < N; j++)
                printf("  ");
        }
        color(10);
        pos(38, 8);
        printf("力  挽  颓  风");
        color(14);
        pos(34, 12);
        printf("你成功找回了本回合的梦。");
        pos(38, 14);
        printf("接下来要怎么办？");
        pos(34, 18);
        printf("A.继续前进   B.返回菜单");
        return WIN;
    } else if (monster->x == player.x && monster->y == player.y || (monster + 1)->x == player.x && (monster + 1)->y == player.y) {
        for (i = 0; i < N; i++) {
            pos(22, i + 4);
            for (j = 0; j < N; j++)
                printf("  ");
        }
        color(12);
        pos(38, 8);
        printf("满  身  疮  痍");
        color(14);
        pos(34, 12);
        printf("你被永远地困在了梦境中。");
        pos(38, 14);
        printf("接下来要怎么办？");
        pos(34, 18);
        printf("A.重新挑战   B.返回菜单");
        return LOSE;
    } else
        return 0;
}

void Map1() {
    int i, j;
    *dir = up;
    *(dir + 1) = down;
    eaten = 0;
    foodcount = 177;
    player.y = player.x = 10;
    monster->y = monster->x = 15;
    (monster + 1)->x = (monster + 1)->y = 4;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            map[i][j] = FOOD;
            if (i == 0 || j == 0 || i == N - 1 || j == N - 1)
                map[i][j] = WALL;
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            map[2 + i][2 + j] = map[2 + i][5 + j] = WALL;
            map[2 + i][13 + j] = map[2 + i][16 + j] = WALL;
            map[2 + i][9 + j] = map[6 + i][9 + j] = WALL;
            map[14 + i][9 + j] = map[17 + i][9 + j] = WALL;
        }
        map[1][9 + i] = map[2 + i][7] = map[2 + i][12] = WALL;
        map[15][2 + i] = map[15][17 - i] = WALL;
        map[15][6 + i] = map[15][12 + i] = WALL;
        map[7][6 + i] = map[7][12 + i] = WALL;
        map[9][7 + i] = map[9][11 + i] = WALL;
        map[5][2 + i] = map[5][16 + i] = WALL;
    }
    for (i = 0; i < 5; i++) {
        map[5 + i][5] = map[5 + i][14] = WALL;
        map[11 + i][5] = map[11 + i][14] = WALL;
    }
    for (i = 0; i < 6; i++) {
        map[5][7 + i] = map[11][7 + i] = WALL;
        map[17][2 + i] = map[17][17 - i] = WALL;
        map[13][7 + i] = WALL;
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            map[7 + i][1 + j] = map[7 + i][18 - j] = WALL;
            map[11 + i][1 + j] = map[11 + i][18 - j] = WALL;
        }
    }
    map[10][7] = map[10][12] = WALL;
}

void Map2() {
    int i, j;
    *dir = right;
    *(dir + 1) = left;
    eaten = 0;
    foodcount = 183;
    player.y = player.x = 1;
    monster->x = 5;
    monster->y = 14;
    (monster + 1)->x = 14;
    (monster + 1)->y = 5;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            map[i][j] = FOOD;
            if (i == 0 || j == 0 || i == N - 1 || j == N - 1)
                map[i][j] = WALL;
            if (i == 2 && j != 1 && j != N - 2 || i == 17 && j != 1 && j != 18)
                map[i][j] = WALL;
            else if (j == 17 && i != 1 && i != N - 2 || j == 2 && i != 1 && i != 18)
                map[i][j] = WALL;
            else if (j == 15 && i >= 4 && i < 16 || j == 4 && i >= 4 && i < 16)
                map[i][j] = WALL;
            else if (i == 15 && j >= 4 && j < 16 || i == 4 && j >= 4 && j < 16)
                map[i][j] = WALL;
        }
    }
    for (i = 0; i < 4; i++) {
        map[1 + i][9] = map[18 - i][9] = map[9][1 + i] = map[9][18 - i] = WALL;
        map[1 + i][10] = map[18 - i][10] = map[10][1 + i] = map[10][18 - i] = WALL;
    }
    map[2][8] = map[2][11] = map[8][2] = map[11][2] = FOOD;
    map[17][8] = map[17][11] = map[8][17] = map[11][17] = FOOD;
    for (j = 0; j < 2; j++) {
        map[4][6 + j] = map[4][13 - j] = map[6 + j][4] = map[13 - j][4] = FOOD;
        map[15][6 + j] = map[15][13 - j] = map[6 + j][15] = map[13 - j][15] = FOOD;
        map[9 + j][10] = map[9 + j][9] = WALL;
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            map[6 + i][6 + j] = map[13 - i][13 - j] = WALL;
            map[6 + i][13 - j] = map[13 - i][6 + j] = WALL;
        }
    }
    map[8][8] = map[8][11] = map[11][8] = map[11][11] = FOOD;
    map[4][6] = map[4][13] = map[6][4] = map[13][4] = WALL;
    map[15][6] = map[15][13] = map[13][15] = map[6][15] = WALL;
}

void Map3() {
    int i, j;
    *dir = up;
    *(dir + 1) = left;
    eaten = 0;
    foodcount = 175;
    player.x = 9;
    player.y = 3;
    monster->y = 13;
    monster->x = 13;
    (monster + 1)->x = 18;
    (monster + 1)->y = 11;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            map[i][j] = FOOD;
            if (i == 0 || j == 0 || i == N - 1 || j == N - 1)
                map[i][j] = WALL;
        }
    }
    for (i = 0; i < 4; i++) {
        map[2][2 + i] = map[2][17 - i] = WALL;
        map[4][2 + i] = map[4][17 - i] = WALL;
        map[8][2 + i] = map[8][17 - i] = WALL;
        map[10][2 + i] = map[10][17 - i] = WALL;
    }
    for (i = 0; i < 6; i++) {
        map[2][7 + i] = map[6][7 + i] = WALL;
        map[9][7 + i] = map[10][7 + i] = WALL;
        map[18][7 + i] = map[8][7 + i] = WALL;
        map[16][7 + i] = map[12][7 + i] = WALL;
        map[12 + i][5] = map[12 + i][14] = WALL;
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            map[4 + i][9 + j] = WALL;
            map[16 + i][2 + j] = map[16 + i][17 - j] = WALL;
            map[6 + i][2 + j] = map[6 + i][17 - j] = WALL;
        }
        map[5 + i][5] = map[5 + i][14] = WALL;
        map[3 + i][7] = map[3 + i][12] = WALL;
        map[14][6 + i] = map[14][12 + i] = WALL;
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            map[12 + i][1 + j] = map[12 + i][18 - j] = WALL;
        }
        map[13 + i][9] = map[13 + i][10] = WALL;
    }
}

void MonsChase(coor* mon, int* dir) {
    int wcount = 0, i;
    if (*dir >= 0 && mon->x == player.x && mon->y > player.y) {
        for (i = mon->y - 1; i != player.y; i--)
            if (map[i][mon->x] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = up;
    } else if (*dir >= 0 && mon->x == player.x && mon->y < player.y) {
        for (i = mon->y + 1; i != player.y; i++)
            if (map[i][mon->x] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = down;
    } else if (*dir < 0 && mon->y == player.y && mon->x < player.x) {
        for (i = mon->x + 1; i != player.x; i++)
            if (map[mon->y][i] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = right;
    } else if (*dir < 0 && mon->y == player.y && mon->x > player.x) {
        for (i = mon->x - 1; i != player.x; i--)
            if (map[mon->y][i] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = left;
    } else if (*dir < 0 && *dir % 2 == 0 && mon->x == player.x && mon->y < player.y) {
        for (i = mon->y + 1; i != player.y; i++)
            if (map[i][mon->x] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = down;
    } else if (*dir < 0 && *dir % 2 != 0 && mon->x == player.x && mon->y > player.y) {
        for (i = mon->y - 1; i != player.y; i--)
            if (map[i][mon->x] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = up;
    } else if (*dir >= 0 && *dir % 2 == 0 && mon->y == player.y && mon->x < player.x) {
        for (i = mon->x + 1; i != player.x; i++)
            if (map[mon->y][i] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = right;
    } else if (*dir >= 0 && *dir % 2 != 0 && mon->y == player.y && mon->x > player.x) {
        for (i = mon->x - 1; i != player.x; i--)
            if (map[mon->y][i] == WALL)
                wcount++;
        if (wcount == 0)
            *dir = left;
    } else
        ;
}

void Operate(int speed, int course) {
    MonsChase(monster, dir);
    MonsChase(monster + 1, dir + 1);
    if (course % (5 * speed) == 0) {
        MonsMove(monster, dir);
        MonsMove(monster + 1, dir + 1);
    }
    Control();
}

int DifAdjust(int speed) {
    int n;
    system("cls");
    color(11);
    pos(52, 6);
    printf("难度设置");
    color(14);
    pos(38, 13);
    printf("梦魇的移动速度会随着难度的提升而提升。");
    pos(51, 10);
    printf("当前难度:%d。", 4 - speed);
    pos(44, 18);
    printf("请输入想要调节的难度(1~3):");
    while (1) {
        scanf("%d", &n);
        if (n > 0 && n <= 3) {
            system("cls");
            return 4 - n;
        } else {
            pos(44, 18);
            printf("输入错误!请重新输入难度(1~3):                       ");
            pos(73, 18);
        }
    }
}

void Menu() {
    int i = 0;
    color(11);
    for (i = 5; i <= 15; i++) {
        pos(27, i);
        printf("┆");
        pos(84, i);
        printf("┆");
    }
    pos(27, 5);
    printf("┌------------------------------------------Instant Dream ◇");
    pos(27, 16);
    printf("└--------------------------------------------------------┘");
    color(3);
    pos(45, 7);
    printf("「  须  臾  之  梦  」");
    color(3);
    pos(44, 9);
    printf("\"在永恒的夜晚做须臾之梦。\"");
    color(11);
    for (i = 19; i <= 21; i++) {
        pos(19, i);
        printf("☆");
        pos(93, i);
        printf("☆");
    }
    color(14);
    pos(39, 12);
    printf("你在一次意外中迷失在了自己的梦境里。");
    pos(33, 13);
    printf("要回到现实，你需要收集梦境碎片并防止被梦魇追到。");
    pos(46, 14);
    printf("那么，你准备好了吗？");
    color(6);
    pos(26, 20);
    printf("A.开始        B.说明        C.难度        D.排行        E.退出");
}

void Guide() {
    system("cls");
    color(11);
    pos(52, 4);
    printf("游戏说明");
    color(14);
    pos(31, 8);
    printf("游戏共有3关，收集到每一关的所以梦境碎片即可。");
    pos(31, 10);
    printf("操作方式很简单，通过按下W、A、S、D键来控制移动，");
    pos(31, 12);
    printf("在你(★)收集梦境碎片(·)的时候，要躲避梦魇(◆)的追击。");
    pos(31, 14);
    printf("梦魇没有看到你(与你之间有墙壁或不在一直线)时，会在直线上徘徊。");
    pos(31, 16);
    printf("一旦它看到你(与你之间在同一直线且中间没有墙壁)，它就会向你追过来。");
    pos(31, 18);
    printf("想办法摆脱梦魇的追击(墙壁、拐角)，或者在它追上你之前收集齐所有的碎片。");
    pos(31, 20);
    printf("每通关一次,你所花费的时间和当前时间都会被记录。前十名会进入排行榜。");
    pos(46, 23);
    printf("按任意键返回主菜单。。。");
    getch();
    system("cls");
}

void CurCall() {
    int i;
    float tc;
    clock_t sum = 0;
    for (i = 0; i < 3; i++)
        sum = sum + pd[i];
    tc = ((float)sum) / CLK_TCK;
    GameRecord(tc);
    system("cls");
    color(6);
    pos(52, 10);
    printf("恭    喜");
    color(14);
    pos(46, 13);
    printf("你找回了所有遗失的梦。");
    pos(42, 15);
    printf("迎接着你的，将会是崭新的黎明。");
    pos(46, 22);
    printf("按任意键返回主菜单。。。");
    getch();
    system("cls");
}

void GameRecord(float tc) {
    rec* gr;
    time_t t;
    time(&t);
    struct tm* ti;
    ti = localtime(&t);
    gr = (rec*)malloc(sizeof(rec));
    gr->timecost = tc;
    gr->year = ti->tm_year;
    gr->mon = ti->tm_mon;
    gr->day = ti->tm_mday;
    gr->hour = ti->tm_hour;
    gr->min = ti->tm_min;
    gr->sec = ti->tm_sec;
    FILE* fp = fopen("record.dat", "ab");
    if (fp == NULL)
        fp = fopen("record.dat", "wb");
    fwrite(gr, sizeof(rec), 1, fp);
    fclose(fp);
    free(gr);
}

int ListWay(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void RankList() {
    system("cls");
    int i = 0;
    rec gd[550];
    FILE* fp = fopen("record.dat", "rb");
    if (fp == NULL) {
        color(14);
        pos(52, 12);
        printf("暂无记录");
        pos(46, 22);
        printf("按任意键返回主菜单。。。");
        getch();
        system("cls");
        return;
    }
    rewind(fp);
    while (!feof(fp)) {
        if (i >= 550)
            break;
        fread(&gd[i], sizeof(rec), 1, fp);
        i++;
    }
    qsort(gd, i - 1, sizeof(rec), ListWay);
    color(11);
    pos(46, 3);
    printf("排行榜(仅显示前十名)");
    color(14);
    pos(42, 5);
    printf("用时(秒)\t\t时间\n");
    color(15);
    for (int j = 0; j < 10; j++) {
        if (j == i - 1)
            break;
        pos(43, 7 + j * 2);
        printf("%6.2f", gd[j].timecost);
        pos(58, 7 + j * 2);
        printf("%d/%02d/%02d ", gd[j].year + 1900, gd[j].mon + 1, gd[j].day);
        printf("%02d:%02d:%02d\n", gd[j].hour, gd[j].min, gd[j].sec);
    }
    fclose(fp);
    color(14);
    pos(46, 27);
    printf("按任意键返回主菜单。。。");
    getch();
    system("cls");
}
