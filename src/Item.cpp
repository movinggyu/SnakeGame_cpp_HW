#include "Item.h"
#include <cstdlib>
#include <algorithm>
#include <ctime>

Item::Item(Map &map, ScoreBoard &board)
  : map_(map), board_(board)
{
    std::srand((unsigned)std::time(nullptr));
    spawn();
}

void Item::spawn() {
    // 이전 위치 지우기
    map_.setCell(position_.x, position_.y, Map::EMPTY);

    // 빈 칸 목록 조회
    auto empties = map_.getEmptyCells();
    if (empties.empty()) return;

    // 무작위 위치 선택
    auto [x, y] = empties[std::rand() % empties.size()];
    position_ = { x, y };

    // 타입 결정: 33% 확률로 성장/독/스피드
    int r = std::rand() % 3;
    if(r==0) type_ = Map::GROWTH_ITEM;
    else if(r==1) type_ = Map::POISON_ITEM;
    else type_ = Map::SPEED_ITEM; // 과제 구현사항) 새로운 아이템

    // 맵에 반영
    map_.setCell(x, y, type_);
    lastSpawnTime_ = std::time(nullptr);
}

void Item::updateRespawn() {
    std::time_t now = std::time(nullptr);
    if (now - lastSpawnTime_ >= RESPAWN_DELAY) {
        spawn();
    }
}

bool Item::checkCollision(Snake &snake) {
    // 충돌은 뱀 머리 위치와 아이템 위치가 같을 때
    const Vec2 &head = snake.getHead();
    if (head.x != position_.x || head.y != position_.y)
        return false;

    bool died = false;
    if (type_ == Map::GROWTH_ITEM) {
            // 성장 아이템: 길이 < MAX_LENGTH일 때만
            if (snake.getBody().size() < Snake::MAX_LENGTH) {
                snake.grow(); // move의 grow 값에 True를 인자로 넣어줌
                board_.updateGrow();
            }
    } else if (type_ == Map::POISON_ITEM) {
        // 독: shrink()가 true면 길이 < 3 → 사망
        died = snake.shrink();
        board_.updatePoison();
    } else if (type_ == Map::SPEED_ITEM){ // 과제 구현사항) 새로운 아이템
        extern int frameDelayMs;
        if(frameDelayMs >= 50)
        frameDelayMs -= 10;
    }

    // 점수판 업데이트는 main에서 board.updateGrow()/updatePoison() 호출

    // 충돌 후 즉시 리스폰
    spawn();
    return died;
}

Vec2 Item::getPosition() const {
    return position_;
}

Map::CellType Item::getType() const {
    return type_;
}
