// src/Map.cpp
#include "Map.h"
#include <fstream>
#include <stdexcept>
#include <ncurses.h>
#include <iostream>

void Map::initColors() const {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // WALL
    init_pair(2, COLOR_WHITE, COLOR_BLACK);    // IMMUNE_WALL
    init_pair(3, COLOR_BLACK, COLOR_BLACK);   // EMPTY
    init_pair(4, COLOR_WHITE, COLOR_GREEN);   // GROWTH_ITEM
    init_pair(5, COLOR_WHITE, COLOR_RED); // POISON_ITEM
    init_pair(6, COLOR_CYAN, COLOR_BLACK);  // GATE
    init_pair(7, COLOR_YELLOW, COLOR_BLACK);     // SNAKE_HEAD
    init_pair(8, COLOR_BLUE, COLOR_BLACK);    // SNAKE_BODY
    
    init_pair(9, COLOR_WHITE, COLOR_MAGENTA);    // SPEED_ITEM // 과제 구현사항) 새로운 아이템
}

// 생성자: 파일이름이 비어있으면 initDefault(), 아니면 loadFromFile()
Map::Map(const std::string &filePath) {
    // 맵 크기 초기화
    grid_.assign(HEIGHT, std::vector<CellType>(WIDTH, EMPTY));

    if (!filePath.empty()) {
        try {
            loadFromFile(filePath);
        } catch (...) {
            // 파일 로드 실패 시 기본맵 사용
            initDefault();
        }
    } else {
        initDefault();
    }
}

// 파일에서 21×21 정수값을 읽어 grid_에 채움
void Map::loadFromFile(const std::string &filePath) {
    std::ifstream ifs(filePath);
    if (!ifs.is_open())
        throw std::runtime_error("Map 파일을 열 수 없습니다: " + filePath);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int v;
            ifs >> v;
            if (!ifs || v < EMPTY || v > GATE)
                throw std::runtime_error("Map 파일 형식 오류");
            grid_[y][x] = static_cast<CellType>(v);
        }
    }
}

// 기본 맵: 외곽은 WALL, 내부는 EMPTY
void Map::initDefault() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1)
                grid_[y][x] = WALL;
            else
                grid_[y][x] = EMPTY;
        }
    }
    // 예시: 맵 중앙에 면역벽 추가
    int midY = HEIGHT / 2, midX = WIDTH / 2;
    grid_[midY][midX] = IMMUNE_WALL;
}

// ncurses를 통해 전체 맵을 그린다.
// WALL: '#', IMMUNE_WALL: 'X', EMPTY: ' ', 나머지는 '.' (임시)
void Map::draw() const {
    wchar_t block[] = L"█";

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int colorPair = 0;
            switch (grid_[y][x]) {
                case WALL:        colorPair = 1; break;
                case IMMUNE_WALL: colorPair = 2; break;
                case EMPTY:       colorPair = 3; break;
                case GROWTH_ITEM: colorPair = 4; block[0] = L'+'; break;
                case POISON_ITEM: colorPair = 5; block[0] = L'-'; break;
                case GATE:        colorPair = 6; break;
                case SNAKE_HEAD:  colorPair = 7; break;
                case SNAKE_BODY:  colorPair = 8; break;
                case SPEED_ITEM:  colorPair = 9; block[0] = L'*'; break; // 과제 구현사항) 새로운 아이템
                default:          colorPair = 3; break;
            }

            attron(COLOR_PAIR(colorPair));
            mvaddwstr(y, x, block); // x좌표에 *2 해주면 네모가 터미널에서 더 정렬됨
            block[0] = L'█';
            attroff(COLOR_PAIR(colorPair));
        }
    }
}

// 경계 검사 후 셀 반환
Map::CellType Map::getCell(int x, int y) const {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return WALL;  // 경계 밖은 벽으로 처리
    return grid_[y][x];
}

// 경계 검사 후 셀 설정
void Map::setCell(int x, int y, CellType t) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;
    grid_[y][x] = t;
}

// EMPTY 셀 좌표 리스트 반환
std::vector<std::pair<int,int>> Map::getEmptyCells() const {
    std::vector<std::pair<int,int>> empties;
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            if (grid_[y][x] == EMPTY)
                empties.emplace_back(x,y);
    return empties;
}

// 일반 WALL (IMMUNE_WALL 제외) 좌표 리스트 반환
std::vector<std::pair<int,int>> Map::getWallCells() const {
    std::vector<std::pair<int,int>> walls;
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            if (grid_[y][x] == WALL)
                walls.emplace_back(x,y);
    return walls;
}
