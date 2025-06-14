#include "Item.h"
#include <cstdlib>
#include <algorithm>
#include <ctime>

// 생성자: 맵, 스코어보드 참조 저장 및 스폰
Item::Item(Map &map, ScoreBoard &board)
  : map_(map), board_(board)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    spawn();
}

// 아이템 스폰: 이전 위치 지우고 빈 칸에 랜덤 배치
void Item::spawn() {
    map_.setCell(position_.x, position_.y, Map::EMPTY);
    auto empties = map_.getEmptyCells();
    if (empties.empty()) return;

    auto [x, y] = empties[std::rand() % empties.size()];
    position_ = {x, y};

    int r = std::rand() % 3;
    if (r == 0)      type_ = Map::GROWTH_ITEM;
    else if (r == 1) type_ = Map::POISON_ITEM;
    else             type_ = Map::SPEED_ITEM;

    map_.setCell(x, y, type_);
    lastSpawnTime_ = std::time(nullptr);
}

// 주기적 리스폰: 마지막 스폰 후 5초 경과 시 spawn()
void Item::updateRespawn() {
    if (std::time(nullptr) - lastSpawnTime_ >= RESPAWN_DELAY) {
        spawn();
    }
}

// 충돌 처리: 성장, 독, 스피드 효과 적용 후 리스폰
bool Item::checkCollision(Snake &snake) {
    const Vec2 &head = snake.getHead();
    if (head.x != position_.x || head.y != position_.y)
        return false;

    bool died = false;
    if (type_ == Map::GROWTH_ITEM) {
        if (snake.getBody().size() < Snake::MAX_LENGTH) {
            snake.grow();
            board_.updateGrow();
        }
    } else if (type_ == Map::POISON_ITEM) {
        died = snake.shrink();
        board_.updatePoison();
    } else { // SPEED_ITEM
        extern int frameDelayMs;
        if (frameDelayMs >= 50) frameDelayMs -= 10;
    }

    spawn();
    return died;
}

// 위치, 타입 반환
Vec2 Item::getPosition() const { return position_; }
Map::CellType Item::getType()   const { return type_; }
