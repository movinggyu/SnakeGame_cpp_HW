#include "Gate.h"

// 생성자: 맵 참조 저장, 난수 초기화
Gate::Gate(Map &map) : map_(map) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    gateA_ = {-1, -1};
    gateB_ = {-1, -1};
}

// 기존 게이트를 벽으로 복원
void Gate::clearPair() {
    if (gateA_.x >= 0) map_.setCell(gateA_.x, gateA_.y, Map::WALL);
    if (gateB_.x >= 0) map_.setCell(gateB_.x, gateB_.y, Map::WALL);
    gateA_ = {-1, -1};
    gateB_ = {-1, -1};
}

// 벽 셀 중 유효한 두 지점을 무작위 선택해 게이트 설정
void Gate::generatePair() {
    clearPair();
    auto walls = map_.getWallCells();

    struct Pair { std::pair<int,int> a, b; };
    std::vector<Pair> valid;
    for (size_t i = 0; i < walls.size(); ++i) {
        for (size_t j = i + 1; j < walls.size(); ++j) {
            auto [x1, y1] = walls[i];
            auto [x2, y2] = walls[j];
            if (std::abs(x1 - x2) + std::abs(y1 - y2) > 1) {
                valid.push_back({walls[i], walls[j]});
            }
        }
    }
    if (valid.empty()) return;

    auto choice = valid[std::rand() % valid.size()];
    gateA_ = {choice.a.first, choice.a.second};
    gateB_ = {choice.b.first, choice.b.second};
    map_.setCell(gateA_.x, gateA_.y, Map::GATE);
    map_.setCell(gateB_.x, gateB_.y, Map::GATE);
}

// 해당 위치가 게이트인지 여부 반환
bool Gate::isGate(const Vec2 &pos) const {
    return (pos.x == gateA_.x && pos.y == gateA_.y)
        || (pos.x == gateB_.x && pos.y == gateB_.y);
}

// 반대편 게이트 좌표 반환
Vec2 Gate::getOtherGate(const Vec2 &pos) const {
    return (pos.x == gateA_.x && pos.y == gateA_.y)
        ? gateB_ : gateA_;
}

// 입장 방향과 위치에 따라 유효한 출구 방향 결정
Direction Gate::computeExitDirection(const Vec2 &pos, Direction inDir) const {
    // 가장자리 게이트: 항상 안쪽 방향
    if (pos.y == 0)               return Direction::Down;
    if (pos.y == Map::HEIGHT-1)   return Direction::Up;
    if (pos.x == 0)               return Direction::Right;
    if (pos.x == Map::WIDTH-1)    return Direction::Left;

    // 내부 게이트: 직진 → 시계 → 반시계 → 후진 순으로 시도
    Direction cw, ccw, opp;
    switch (inDir) {
        case Direction::Up:    cw = Direction::Right; ccw = Direction::Left;  opp = Direction::Down;  break;
        case Direction::Right: cw = Direction::Down;  ccw = Direction::Up;    opp = Direction::Left;  break;
        case Direction::Down:  cw = Direction::Left;  ccw = Direction::Right; opp = Direction::Up;    break;
        case Direction::Left:  cw = Direction::Up;    ccw = Direction::Down;  opp = Direction::Right; break;
    }
    std::vector<Direction> order = {inDir, cw, ccw, opp};

    for (auto d : order) {
        Vec2 next{ pos.x + (d == Direction::Right) - (d == Direction::Left),
                   pos.y + (d == Direction::Down)  - (d == Direction::Up) };
        if (map_.getCell(next.x, next.y) == Map::EMPTY)
            return d;
    }
    // 모든 방향이 막힌 경우 원래 방향 유지
    return inDir;
}
