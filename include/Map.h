#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <locale.h>  // 유니코드 블럭 출력용
#include <wchar.h>

// Map 클래스: 게임 맵 데이터 관리 및 렌더링
class Map {
public:
    static const int WIDTH  = 21;
    static const int HEIGHT = 21;

    // 셀 타입
    enum CellType {
        EMPTY         = 0,
        WALL          = 1,
        IMMUNE_WALL   = 2,
        SNAKE_HEAD    = 3,
        SNAKE_BODY    = 4,
        GROWTH_ITEM   = 5,
        POISON_ITEM   = 6,
        GATE          = 7,

        SPEED_ITEM    = 8  // 과제 구현사항) 새로운 아이템
    };

    // 생성자: 파일 경로 지정 시 로드, 기본값은 기본맵 초기화
    Map(const std::string &filePath = "");

    // ncurses 색상 초기화
    void initColors() const;

    // 맵 화면 그리기
    void draw() const;

    // 셀값 조회 및 설정
    CellType getCell(int x, int y) const;
    void setCell(int x, int y, CellType t);

    // 빈칸 좌표 목록 (아이템 스폰용)
    std::vector<std::pair<int,int>> getEmptyCells() const;

    // 벽 좌표 목록 (Gate 생성용; IMMUNE_WALL 제외)
    std::vector<std::pair<int,int>> getWallCells() const;

private:
    std::vector<std::vector<CellType>> grid_;

    // 파일에서 맵 데이터 로드
    void loadFromFile(const std::string &filePath);

    // 기본 맵 데이터 초기화
    void initDefault();
};

#endif // MAP_H
