#ifndef ITEM_H
#define ITEM_H

#include <ctime>
#include <vector>
#include "Map.h"
#include "Snake.h"
#include "ScoreBoard.h"

// Stage3: 성장 및 독 아이템 로직
// – 최대 3개 동시 관리는 main()에서 std::vector<Item> items; 로 처리
// – 5초마다 리스폰, 뱀과 충돌 시 즉시 다른 위치로 이동
class Item {
public:
    // 생성자: Map 참조를 저장한 뒤 즉시 spawn()
    Item(Map &map, ScoreBoard &board);

    // 빈 칸 중 무작위 위치에 아이템 생성
    void spawn();

    // 매 프레임(틱)마다 호출 → 마지막 spawn 이후 5초 넘으면 재생성
    void updateRespawn();

    // 뱀과 충돌 검사
    //   충돌 시:
    //    – 성장(Item::GROWTH_ITEM)  → snake.move(true)  
    //    – 독(Item::POISON_ITEM)    → died = snake.shrink()  
    //   효과 적용 후 즉시 spawn()
    // 반환값: 독에 의해 뱀 길이가 3 미만이 되어 사망했다면 true
    bool checkCollision(Snake &snake);

    // 현재 아이템 위치 확인 (main에서 그리기/지우기용)
    Vec2  getPosition() const;
    Map::CellType getType()    const;

private:
    Map            &map_;               // 맵에 직접 cell 쓰기/읽기 위해 참조 저장
    ScoreBoard     &board_;
    Vec2           position_;          // 현재 아이템 좌표
    Map::CellType  type_;              // GROWTH_ITEM 또는 POISON_ITEM
    std::time_t    lastSpawnTime_;     // 마지막 생성 시각
    static constexpr int RESPAWN_DELAY = 5; // 초 단위
};

#endif // ITEM_H
