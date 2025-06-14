#ifndef ITEM_H
#define ITEM_H

#include <ctime>
#include <vector>
#include "Map.h"
#include "Snake.h"
#include "ScoreBoard.h"

// Item 클래스: 성장 및 독 아이템 생성 및 처리
class Item {
public:
    // 생성자: 맵·스코어보드 참조 저장, 즉시 아이템 스폰
    Item(Map &map, ScoreBoard &board);

    // 빈 칸에 무작위로 아이템 배치
    void spawn();

    // 스폰 후 일정 시간(5초) 지나면 재생성
    void updateRespawn();

    // 뱀과 충돌 검사 및 효과 적용
    // 효과 후 즉시 스폰, 사망 시 true 반환
    bool checkCollision(Snake &snake);

    // 아이템 위치 및 종류 조회
    Vec2            getPosition() const;
    Map::CellType   getType()     const;

private:
    Map           &map_;            // 게임 맵 참조
    ScoreBoard    &board_;          // 스코어보드 참조
    Vec2           position_;       // 현재 위치
    Map::CellType  type_;           // GROWTH_ITEM 또는 POISON_ITEM 또는 SPEED_ITEM
    std::time_t    lastSpawnTime_;  // 마지막 스폰 시각
    static constexpr int RESPAWN_DELAY = 5;  // 재생성 대기 시간(초)
};

#endif // ITEM_H
