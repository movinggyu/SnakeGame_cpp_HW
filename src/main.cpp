// src/main.cpp
#include <ncurses.h>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

#include "Map.h"
#include "Snake.h"
#include "Item.h"
#include "Gate.h"
#include "ScoreBoard.h"

// 게임 설정
static constexpr int INITIAL_SNAKE_LENGTH = 3;
static constexpr int NUM_ITEMS = 3;
// static constexpr int FRAME_DELAY_MS = 150;
static int GATE_SPAWN_DELAY_SEC = 10;
int frameDelayMs = 130; // 과제 구현사항) 새로운 아이템

// 방향키 → Direction 매핑
Direction keyToDir(int ch) {
    switch (ch) {
        case KEY_UP:    return Direction::Up;
        case KEY_DOWN:  return Direction::Down;
        case KEY_LEFT:  return Direction::Left;
        case KEY_RIGHT: return Direction::Right;
        default:        return Direction::Up; // 기본값(실제 사용 전에 setDirection 호출)
    }
}

int main() {
    setlocale(LC_ALL, "");  // 유니코드 출력을 위해
    std::srand((unsigned)std::time(nullptr));

    // 스테이지별 맵 파일 목록
    std::vector<std::string> levels = { // 여러 스테이지의 추가로 벽에대한 변화를 추가했습니다.
        "resources/stage1.txt",
        "resources/stage2.txt",
        "resources/stage3.txt",
        "resources/stage4.txt"
    };

    // ncurses 초기화
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();

    bool quitAll = false;

    // 각 스테이지 순회
    for (size_t stage = 0; stage < levels.size() && !quitAll; ++stage) {
        // 맵, 뱀, 아이템, 게이트, 점수판 초기화
        Map map(levels[stage]);
        map.initColors();
        Snake snake(map);

        // 초기 맵에 남아있는 스네이크 셀을 모두 빈 칸으로 지워 준다.
        for (const auto &seg : snake.getBody()) {
            map.setCell(seg.x, seg.y, Map::EMPTY);
        }

        std::vector<Item> items;
        Gate gate(map);
        ScoreBoard board(INITIAL_SNAKE_LENGTH);
        for (int i = 0; i < NUM_ITEMS; ++i)
            items.emplace_back(map, board);

        bool gateSpawned = false;
        bool gameOver    = false;
        bool stageClear  = false;
        std::time_t startTime = std::time(nullptr);

        // 메인 루프
        while (!gameOver && !stageClear) {
            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                quitAll = true;
                break;
            }
            // 방향키 입력 처리
            if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
                snake.setDirection(keyToDir(ch));
            }

            // 이동 전 충돌 예측
            if (snake.willHitWall(map) || snake.willHitSelf()) {
                gameOver = true;
                break;
            }

            // 뱀 이동
            snake.move(false);

            // 아이템 갱신 및 충돌 처리
            for (auto &item : items) {
                item.updateRespawn();
                bool died = item.checkCollision(snake);
                if (item.checkCollision(snake)) {
                    // 독에 의한 사망 체크
                    if (died) {
                        gameOver = true;
                        break;
                    }
                }
            }

            if (board.isGameOver()) {
                break;
            }

            // 게이트 생성 시점 결정
            int elapsedSec = static_cast<int>(std::time(nullptr) - startTime);
            if (!gateSpawned && elapsedSec >= GATE_SPAWN_DELAY_SEC) {
                gate.generatePair();
                gateSpawned = true;
                GATE_SPAWN_DELAY_SEC += 10;
            }

            // 게이트 통과 처리
            Vec2 head = snake.getHead();
            if (gateSpawned && gate.isGate(head)) {
                // 반대 게이트로 이동
                Vec2 exit = gate.getOtherGate(head);
                // Snake 클래스에 warpTo(x,y) 메서드가 필요합니다.
                snake.warpTo(exit.x, exit.y);
                // 진출 방향 계산
                Direction newDir = gate.computeExitDirection(exit, snake.getDirection());
                snake.forceDirection(newDir);
                board.updateGate();
            }

            // 화면 그리기
            wchar_t block[] = L"█";
            clear();
            map.draw();
            // Snake Head
            attron(COLOR_PAIR(7));
            Vec2 h = snake.getHead();
            mvaddwstr(h.y, h.x, block);
            attroff(COLOR_PAIR(7));

            // Snake Body
            for (auto &seg : snake.getBody()) {
                if (seg.x == h.x && seg.y == h.y) continue;  // 머리는 이미 찍었으니 제외
                attron(COLOR_PAIR(8));
                mvaddwstr(seg.y, seg.x, block);
                attroff(COLOR_PAIR(8));
            }

            // 점수판
            elapsedSec = static_cast<int>(std::time(nullptr) - startTime);
            board.draw(elapsedSec);

            refresh();
            napms(frameDelayMs);

            // 충돌 후 최종 검사
            if (snake.isCollision(map))
                gameOver = true;
            else if (board.checkMission())
                stageClear = true;
        }

        // 스테이지 결과 메시지
        clear();
        if (quitAll) {
            mvprintw(Map::HEIGHT/2, Map::WIDTH/2 - 5, "Quit Game");
            break;
        } else if (gameOver || board.isGameOver()) {
            mvprintw(Map::HEIGHT/2, Map::WIDTH/2 - 5, "Game Over!");
            mvprintw(Map::HEIGHT/2+1, Map::WIDTH/2 - 9, "Press any key to exit.");
            refresh();
            nodelay(stdscr, FALSE);
            getch();
            endwin();
            return 0;    
        } else if (stageClear) {
            mvprintw(Map::HEIGHT/2, Map::WIDTH/2 - 7, "Stage %d Clear!", stage+1);
            refresh();
            napms(2000); // 2초 대기 후 다음 스테이지
        }
    }

    // 전체 클리어 또는 종료 메시지
    clear();
    if (!quitAll)
        mvprintw(Map::HEIGHT/2, Map::WIDTH/2 - 6, "You Win!");
    mvprintw(Map::HEIGHT/2+1, Map::WIDTH/2 - 9, "Press any key to exit.");
    nodelay(stdscr, FALSE);
    getch();

    endwin();
    return 0;
}
