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

// 초기 뱀 길이 설정
static constexpr int INITIAL_SNAKE_LENGTH = 3;
// 맵에 생성할 아이템 개수
static constexpr int NUM_ITEMS = 3;
// 게이트 생성 간격 (초 단위)
static int GATE_SPAWN_DELAY_SEC = 10;
// 프레임 지연 시간 (밀리초 단위)
int frameDelayMs = 130; // 새로운 아이템 관련 설정

// 키 입력을 Direction으로 변환
Direction keyToDir(int ch) {
    switch (ch) {
        case KEY_UP:    return Direction::Up;    // 위 방향
        case KEY_DOWN:  return Direction::Down;  // 아래 방향
        case KEY_LEFT:  return Direction::Left;  // 왼쪽 방향
        case KEY_RIGHT: return Direction::Right; // 오른쪽 방향
        default:        return Direction::Up;    // 기본값
    }
}

int main() {
    setlocale(LC_ALL, "");                    // 유니코드 출력 지원
    std::srand(static_cast<unsigned>(std::time(nullptr))); // 랜덤 시드 초기화

    // 스테이지별 맵 파일 경로 목록
    std::vector<std::string> levels = {
        "resources/stage1.txt",
        "resources/stage2.txt",
        "resources/stage3.txt",
        "resources/stage4.txt"
    };

    // ncurses 초기화
    initscr();      // 화면 초기화
    cbreak();       // 라인 버퍼링 비활성화
    noecho();       // 입력 문자 에코 비활성화
    keypad(stdscr, TRUE); // 특수키 입력 허용
    nodelay(stdscr, TRUE); // 논블로킹 입력 모드
    curs_set(0);    // 커서 숨김
    start_color();  // 컬러 모드 시작
    use_default_colors(); // 기본 컬러 사용

    bool quitAll = false; // 전체 게임 종료 플래그

    // 각 스테이지 반복
    for (size_t stage = 0; stage < levels.size() && !quitAll; ++stage) {
        // 맵과 게임 객체 초기화
        Map map(levels[stage]);
        map.initColors();
        Snake snake(map);

        // 초깃값으로 맵에 남은 뱀 셀 모두 빈 칸 처리
        for (const auto &seg : snake.getBody()) {
            map.setCell(seg.x, seg.y, Map::EMPTY);
        }

        // 아이템과 게이트, 점수판 생성
        std::vector<Item> items;
        Gate gate(map);
        ScoreBoard board(INITIAL_SNAKE_LENGTH);
        for (int i = 0; i < NUM_ITEMS; ++i)
            items.emplace_back(map, board);

        bool gateSpawned = false; // 게이트 생성 여부
        bool gameOver    = false; // 게임 오버 여부
        bool stageClear  = false; // 스테이지 클리어 여부
        std::time_t startTime = std::time(nullptr); // 스테이지 시작 시간

        // 메인 게임 루프
        while (!gameOver && !stageClear) {
            int ch = getch(); // 키 입력 읽기
            if (ch == 'q' || ch == 'Q') {
                quitAll = true; // 전체 종료
                break;
            }

            // 방향키 입력 처리
            if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
                snake.setDirection(keyToDir(ch));
            }

            // 이동 전 충돌 예측: 벽 또는 자기 자신
            if (snake.willHitWall(map) || snake.willHitSelf()) {
                gameOver = true;
                break;
            }

            // 뱀 이동 (grow=false)
            snake.move(false);

            // 아이템 상태 업데이트 및 충돌 처리
            for (auto &item : items) {
                item.updateRespawn();    // 재생성 타이밍 처리
                bool died = item.checkCollision(snake); // 충돌 검사
                if (died) { // 독 아이템 충돌 시
                    gameOver = true;
                    break;
                }
            }

            // 점수판 기준 게임 오버 체크
            if (board.isGameOver())
                break;

            // 게이트 생성 시점 확인
            int elapsedSec = static_cast<int>(std::time(nullptr) - startTime);
            if (!gateSpawned && elapsedSec >= GATE_SPAWN_DELAY_SEC) {
                gate.generatePair();      // 게이트 생성
                gateSpawned = true;
                GATE_SPAWN_DELAY_SEC += 10; // 다음 생성 시간 연장
            }

            // 게이트 통과 처리
            Vec2 head = snake.getHead();
            if (gateSpawned && gate.isGate(head)) {
                Vec2 exit = gate.getOtherGate(head);         // 반대 게이트 위치
                snake.warpTo(exit.x, exit.y);               // 위치 워프
                Direction newDir = gate.computeExitDirection(exit, snake.getDirection());
                snake.forceDirection(newDir);               // 방향 강제 설정
                board.updateGate();                         // 통과 횟수 증가
            }

            // 화면 그리기
            clear();       // 화면 초기화
            map.draw();    // 맵 그리기

            // 뱀 머리 그리기
            attron(COLOR_PAIR(7));
            Vec2 h = snake.getHead();
            mvaddwstr(h.y, h.x, L"█");
            attroff(COLOR_PAIR(7));

            // 뱀 몸통 그리기
            attron(COLOR_PAIR(8));
            for (auto &seg : snake.getBody()) {
                if (seg.x == h.x && seg.y == h.y) continue; // 머리 제외
                mvaddwstr(seg.y, seg.x, L"█");
            }
            attroff(COLOR_PAIR(8));

            // 점수판 표시
            board.draw(elapsedSec);
            refresh();    // 화면 업데이트

            // 프레임 지연
            napms(frameDelayMs);

            // 최종 충돌 및 미션 완료 검사
            if (snake.isCollision(map))
                gameOver = true;
            else if (board.checkMission())
                stageClear = true;
        }

        // 스테이지 종료 메시지 처리
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
            napms(2000); // 2초 대기
        }
    }

    // 모든 스테이지 클리어 또는 종료
    clear();
    if (!quitAll)
        mvprintw(Map::HEIGHT/2, Map::WIDTH/2 - 6, "You Win!");
    mvprintw(Map::HEIGHT/2+1, Map::WIDTH/2 - 9, "Press any key to exit.");
    nodelay(stdscr, FALSE);
    getch();

    endwin(); // ncurses 종료
    return 0;
}
