#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <Windows.h>

#define COURSE_WIDTH (256)
#define COURSE_HEIGHT (16)
#define SCREEN_WIDTH (32)
#define SCREEN_HEIGHT (16)
#define UPDATE_FPS  (60)
#define UPDATE_INTERVAL (1000 / UPDATE_FPS)
#define DRAW_FPS  (10)
#define DRAW_INTERVAL (1000 / DRAW_FPS)

//ベクトル
typedef struct {
    float x, y;
} VEC2;

//プレイヤー
typedef struct {
    VEC2 position;
    VEC2 velocity;
    bool jumping;
} PLAYER;

//マップデータをテキストで
char course[COURSE_HEIGHT][COURSE_WIDTH] = 
{
    "                                                                                                                                                                                                                    ",
    "                                                                                                                                                                                                                    ",
    "                                                                                                                                                                                                                    ",
    "                   ccc              cccc                           ccc              cccc                           ccc              cccc                           ccc              cccc              g             ",
    "                   ccc     ccccc    cccc                ccc        ccc              cccc                ccc        ccc     ccccc    cccc                ccc        ccc     ccccc    cccc              g ccc         ",
    "                           ccccc                        ccc                                             ccc                ccccc                        ccc                ccccc                      g ccc         ",
    "                      q                                                         bbbbbbbb   bbbq              q           bbb    bqqb                                                        bb        g             ",
    "                                                                                                                                                                                           bbb        g             ",
    "                                                                                                                                                                                          bbbb        g             ",
    "                                                                                                                                                                                         bbbbb        g    fff      ",
    "                q   bqbqb                                                    bqb              b     bb    q  q  q     b          bb                                     bbqb            bbbbbb        g    fff      ",
    "  m                                   pp      pp  m      pp                                       m                                     bb  bb    m   bbb                              bbbbbbb    m   g   fffff     ",
    " mmm                        pp        pp      pp mmm     pp      m                               mmm             m                     bbb  bbb  mmm bbbb  bbb   m pp              pp bbbbbbbb   mmm  g   fffff  m  ",
    "mmmmm      tttttmmm    ttt  pp        pp tttt ppmmmmm    pptttttmmm    ttt               tttt   mmmmm      tttttmmm    ttt            bbbbttbbbbmmmmbbbbb  bbbbtmmmpp  ttt         ppbbbbbbbbb  mmmmm b   ffffftmmm ",
    "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb  bbbbbbbbbbbbbbb   bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb  bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
    "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb  bbbbbbbbbbbbbbb   bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb  bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
};

char screen[SCREEN_HEIGHT][SCREEN_WIDTH]; //マップ、キャラ保持

char aa[256][3]; //文字コードの文字列変換

PLAYER player; //プレイヤー構造体

bool keyPressed[256]; //それぞれのキーの押した状態を管理する配列

clock_t startTime; //開始の記録

bool goalReached = false; //ゴールに到達したか

float goalTime = 0.0f; //ゴールした時の経過時間

//D
void DrawScreen() 
{
    system("cls");

    //ゴール時間の処理

    float elapsed; 
    if(goalReached)
    {
        elapsed = goalTime; //ゴール済みなら記録された時間を使う
    }
    else
    {
        elapsed = (float)(clock() - startTime) / CLOCKS_PER_SEC; //CPUクロック - 1秒間に何クロックで秒数にしている
    }

    printf("Time: %.1f秒\n", elapsed);

    if (goalReached)
    {
        printf("ゴールしました！おめでとう！\n");
    }

    int scrollX = (int)roundf(player.position.x) - SCREEN_WIDTH / 2; //player.position.xをscreenのwidthと割ることで、真ん中にしている

    if (scrollX < 0)
    {
        scrollX = 0; //もしスクロールの位置が負に行ったら、0に戻す
    }

    if (scrollX > COURSE_WIDTH - SCREEN_WIDTH) //コース外を移さない処理
    {
        scrollX = COURSE_WIDTH - SCREEN_WIDTH;
    }

    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            screen[y][x] = course[y][scrollX + x];
        }
    }
    int x = (int)roundf(player.position.x) - scrollX;
    int y = (int)roundf(player.position.y);
    if ((x >= 0) && (x < SCREEN_WIDTH) && (y >= 0) && (y < SCREEN_HEIGHT))
        screen[y][x] = '@';

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++)
            printf("%s", aa[screen[y][x]]);
        printf("\n");
    }
}

void Init() {
    player.position = { 20.0f, 13.0f };
    player.velocity = {};
    player.jumping = false;
    memset(keyPressed, 0, sizeof keyPressed);
    startTime = clock();
    goalReached = false;
    goalTime = 0.0f;
    DrawScreen();
}

bool IntersectCourse(VEC2 _v) {
    int x = (int)_v.x;
    int y = (int)_v.y;
    if (x < 0 || x >= COURSE_WIDTH || y < 0 || y >= COURSE_HEIGHT)
        return true;
    switch (course[y][x]) {
    case 'b': case 'p': case 'q':
        return true;
    }
    return false;
}

int main() {
    // 表示変換
    sprintf_s(aa[0], "Ｘ");
    sprintf_s(aa[' '], "　");
    sprintf_s(aa['b'], "■");
    sprintf_s(aa['p'], "□");
    sprintf_s(aa['q'], "？");
    sprintf_s(aa['m'], "へ");
    sprintf_s(aa['t'], "Ｙ");
    sprintf_s(aa['c'], "～");
    sprintf_s(aa['g'], "｜");
    sprintf_s(aa['f'], "田");
    sprintf_s(aa['@'], "★");

    while (1) {
        Init();
        clock_t lastUpdateClock = clock();
        clock_t lastDrawClock = clock();

        while (!goalReached) {
            clock_t nowClock = clock();
            keyPressed['a'] = GetAsyncKeyState('A') & 0x8000;
            keyPressed['d'] = GetAsyncKeyState('D') & 0x8000;
            bool spacePressed = GetAsyncKeyState(VK_SPACE) & 0x8000;

            if (nowClock >= lastUpdateClock + UPDATE_INTERVAL) {
                lastUpdateClock = nowClock;
                float acceleration = 0.02f;
                if (keyPressed['d']) player.velocity.x += acceleration;
                if (keyPressed['a']) player.velocity.x -= acceleration;
                player.velocity.y += 0.01f;
                player.velocity.x *= 0.9f;
                player.position.x += player.velocity.x;
                player.position.y += player.velocity.y;

                VEC2 left = { player.position.x, player.position.y + 0.5f };
                if (IntersectCourse(left)) {
                    player.position.x = floorf(player.position.x) + 1.0f;
                    player.velocity.x = 0;
                }
                VEC2 right = { player.position.x + 1.0f, player.position.y + 0.5f };
                if (IntersectCourse(right)) {
                    player.position.x = floorf(player.position.x);
                    player.velocity.x = 0;
                }
                VEC2 down = { player.position.x + 0.5f, player.position.y + 1.0f };
                if (IntersectCourse(down)) {
                    player.position.y = floorf(player.position.y);
                    player.velocity.y = 0;
                    player.jumping = false;
                }
                else {
                    player.jumping = true;
                }
                VEC2 up = { player.position.x + 0.5f, player.position.y };
                if (IntersectCourse(up)) {
                    player.position.y = floorf(player.position.y) + 1.0f;
                    player.velocity.y = 0;
                }

                if (spacePressed && !player.jumping) {
                    player.velocity.y = -0.25f;
                    player.jumping = true;
                }
            }

            if (nowClock >= lastDrawClock + DRAW_INTERVAL) {
                lastDrawClock = nowClock;
                DrawScreen();
            }

            int px = (int)roundf(player.position.x);
            int py = (int)roundf(player.position.y);
            if (course[py][px] == 'g') {
                goalReached = true;
            }
        }

        // ゴール後の処理
        goalTime = (float)(clock() - startTime) / CLOCKS_PER_SEC;
        char name[] = "Player1";
        char command[512];

        sprintf_s(command, sizeof(command),
            "curl -s -L -X POST "
            "-H \"Content-Type: application/x-www-form-urlencoded\" "
            "-d \"name=%s&time=%.2f\" "
            "\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\" " //APIのURL、ID
            "> top3.json",
            name, goalTime);



        system(command);
        Sleep(1000);

        // JSONの中身を読み取って上位3表示
        FILE* fp;
        if (fopen_s(&fp, "top3.json", "r") == 0) {
            char buffer[1024];
            size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp);
            buffer[bytesRead] = '\0';
            fclose(fp);

            //printf("\n=== 上位3タイム ===\n");

            char* ptr = buffer;
            int rank = 1;
            while ((ptr = strchr(ptr, '{')) != NULL && rank <= 3) {
                char nameBuf[256];
                float time;

                // JSONオブジェクトを一時的に取り出す
                char jsonObject[256];
                char* end = strchr(ptr, '}');
                if (end == NULL) break;

                size_t len = end - ptr + 1;
                strncpy_s(jsonObject, sizeof(jsonObject), ptr, len);
                jsonObject[len] = '\0';

                if (sscanf_s(jsonObject, "{\"name\":\"%[^\"]\",\"time\":%f}", nameBuf, (unsigned)_countof(nameBuf), &time) == 2) {
                    printf("%d位: %s - %.2f秒\n", rank, nameBuf, time);
                    rank++;
                }

                ptr = end + 1; // 次のJSONオブジェクトへ
            }
        }

        // ゴール後の処理の最後に追加

        printf("\nもう一度プレイしますか？ (y/n): ");

        // キーバッファをクリア
        while (_kbhit()) _getch();

        char choice = tolower(_getch());
        if (choice != 'y') break;
    }

    return 0;
}
