#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <locale.h> // 유니코드 블럭 사용시 필요
#include <wchar.h>

// Stage1: 맵 렌더링 및 데이터 로딩
class Map {
public:
    // (가로, 세로) 최소 크기
    static const int WIDTH  = 21;
    static const int HEIGHT = 21;

    // 셀 타입 정의
    enum CellType {
        EMPTY         = 0,
        WALL          = 1,
        IMMUNE_WALL   = 2,
        SNAKE_HEAD    = 3,
        SNAKE_BODY    = 4,
        GROWTH_ITEM   = 5,
        POISON_ITEM   = 6,
        GATE          = 7,

        SPEED_ITEM = 8 // 과제 구현사항) 새로운 아이템
    };

    // 생성자: 파일 경로로부터 맵을 로드하거나,
    // filePath가 빈 문자열("")이면 하드코딩 초기화
    Map(const std::string &filePath = "");

    // 색 정의
    void initColors() const;

    // 렌더링: ncurses 상에서 벽, 빈 칸을 그린다
    void draw() const;

    // 좌표 (x,y)의 현재 셀 값을 반환
    CellType getCell(int x, int y) const;

    // 좌표 (x,y)에 새로운 셀 값을 설정
    void setCell(int x, int y, CellType t);

    // 빈 칸 좌표 목록 반환 (아이템 스폰용)
    std::vector<std::pair<int,int>> getEmptyCells() const;

    // 일반 벽 좌표 목록 반환 (게이트 생성용; IMMUNE_WALL 제외)
    std::vector<std::pair<int,int>> getWallCells() const;

private:
    // 2D 맵 데이터
    std::vector<std::vector<CellType>> grid_;

    // 파일로부터 맵 로드 (Stage1 용)
    void loadFromFile(const std::string &filePath);

    // 하드코딩 맵 초기화 (테스트용)
    void initDefault();
};

#endif // MAP_H
