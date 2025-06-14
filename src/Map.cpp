#include "Map.h"
#include <fstream>
#include <stdexcept>
#include <ncurses.h>
#include <iostream>

// 색상 초기화: ncurses 페어 설정
void Map::initColors() const {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // WALL
    init_pair(2, COLOR_WHITE, COLOR_BLACK);   // IMMUNE_WALL
    init_pair(3, COLOR_BLACK, COLOR_BLACK);   // EMPTY
    init_pair(4, COLOR_WHITE, COLOR_GREEN);   // GROWTH_ITEM
    init_pair(5, COLOR_WHITE, COLOR_RED);     // POISON_ITEM
    init_pair(6, COLOR_CYAN, COLOR_BLACK);    // GATE
    init_pair(7, COLOR_YELLOW, COLOR_BLACK);  // SNAKE_HEAD
    init_pair(8, COLOR_BLUE, COLOR_BLACK);    // SNAKE_BODY
    init_pair(9, COLOR_WHITE, COLOR_MAGENTA); // SPEED_ITEM  // 과제 구현사항) 새로운 아이템
}

// 생성자: 파일 경로 주어지면 로드, 아니면 기본맵 초기화
Map::Map(const std::string &filePath) {
    grid_.assign(HEIGHT, std::vector<CellType>(WIDTH, EMPTY));
    if (!filePath.empty()) {
        try {
            loadFromFile(filePath);
        } catch (...) {
            initDefault(); // 로드 실패 시 기본맵
        }
    } else {
        initDefault();
    }
}

// 파일에서 맵 데이터 읽어 grid_에 저장
void Map::loadFromFile(const std::string &filePath) {
    std::ifstream ifs(filePath);
    if (!ifs.is_open())
        throw std::runtime_error("Map 파일을 열 수 없습니다: " + filePath);
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int v; ifs >> v;
            if (!ifs || v < EMPTY || v > SPEED_ITEM)
                throw std::runtime_error("Map 파일 형식 오류");
            grid_[y][x] = static_cast<CellType>(v);
        }
    }
}

// 기본맵 생성: 외곽은 WALL, 내부는 EMPTY + 중앙 IMMUNE_WALL
void Map::initDefault() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            grid_[y][x] = (y==0 || y==HEIGHT-1 || x==0 || x==WIDTH-1) ? WALL : EMPTY;
        }
    }
    int midY = HEIGHT/2, midX = WIDTH/2;
    grid_[midY][midX] = IMMUNE_WALL;
}

// 전체 맵 출력: 각 CellType에 대응하는 블록 문자와 색상 사용
void Map::draw() const {
    wchar_t symbol[] = L"█";
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int pair = 3;
            switch (grid_[y][x]) {
                case WALL:        pair = 1; break;
                case IMMUNE_WALL: pair = 2; break;
                case EMPTY:       pair = 3; break;
                case GROWTH_ITEM: pair = 4; symbol[0] = L'+';break;
                case POISON_ITEM: pair = 5; symbol[0] = L'-'; break;
                case GATE:        pair = 6; break;
                case SNAKE_HEAD:  pair = 7; break;
                case SNAKE_BODY:  pair = 8; break;
                case SPEED_ITEM:  pair = 9; symbol[0] = L'*'; // 과제 구현사항) 새로운 아이템
                default:          pair = 3; break;
            }
            attron(COLOR_PAIR(pair));
            mvaddwstr(y, x, symbol);
            symbol[0] = L'█';
            attroff(COLOR_PAIR(pair));
        }
    }
}

// 좌표 유효성 검사 후 셀 타입 반환
Map::CellType Map::getCell(int x, int y) const {
    if (x<0||x>=WIDTH||y<0||y>=HEIGHT) return WALL;
    return grid_[y][x];
}

// 경계 내에만 셀 타입 설정
void Map::setCell(int x, int y, CellType t) {
    if (x<0||x>=WIDTH||y<0||y>=HEIGHT) return;
    grid_[y][x] = t;
}

// EMPTY인 모든 좌표 반환
std::vector<std::pair<int,int>> Map::getEmptyCells() const {
    std::vector<std::pair<int,int>> list;
    for (int y=0; y<HEIGHT; ++y)
        for (int x=0; x<WIDTH; ++x)
            if (grid_[y][x]==EMPTY)
                list.emplace_back(x,y);
    return list;
}

// 일반 WALL 좌표 반환
std::vector<std::pair<int,int>> Map::getWallCells() const {
    std::vector<std::pair<int,int>> list;
    for (int y=0; y<HEIGHT; ++y)
        for (int x=0; x<WIDTH; ++x)
            if (grid_[y][x]==WALL)
                list.emplace_back(x,y);
    return list;
}
