#include "Gate.h"

// 생성자: 맵 참조 저장, 난수 시드 초기화
Gate::Gate(Map &map) : map_(map) {
    std::srand((unsigned)std::time(nullptr));
    gateA_ = {-1,-1};
    gateB_ = {-1,-1};
}

// 기존 Gate를 맵에서 WALL로 되돌림
void Gate::clearPair() {
    if (gateA_.x >= 0) map_.setCell(gateA_.x, gateA_.y, Map::WALL);
    if (gateB_.x >= 0) map_.setCell(gateB_.x, gateB_.y, Map::WALL);
    gateA_ = {-1,-1};
    gateB_ = {-1,-1};
}

// WALL 셀 중 두 곳을 무작위로 선택해 Gate로 설정
void Gate::generatePair() {
    clearPair();

    // 1) 모든 WALL 셀을 가져와서
    auto walls = map_.getWallCells();
    std::vector<std::pair<int,int>> validA, validB;

    // 2) i<j 쌍을 만들어 두 게이트가 맨해튼 거리가 >1 인 경우만 유효 목록에 추가
    struct Pair { std::pair<int,int> a, b; };
    std::vector<Pair> validPairs;
    for (size_t i = 0; i < walls.size(); ++i) {
        for (size_t j = i + 1; j < walls.size(); ++j) {
            auto [x1,y1] = walls[i];
            auto [x2,y2] = walls[j];
            int manh = std::abs(x1 - x2) + std::abs(y1 - y2);
            if (manh > 1) {
                validPairs.push_back({walls[i], walls[j]});
            }
        }
    }
    if (validPairs.empty()) return;

    // 3) 그중 하나를 랜덤 선택
    auto choice = validPairs[std::rand() % validPairs.size()];
    gateA_ = { choice.a.first, choice.a.second };
    gateB_ = { choice.b.first, choice.b.second };

    // 4) 맵에 반영
    map_.setCell(gateA_.x, gateA_.y, Map::GATE);
    map_.setCell(gateB_.x, gateB_.y, Map::GATE);
}

// 좌표가 Gate인지 검사
bool Gate::isGate(const Vec2 &pos) const {
    return (pos.x == gateA_.x && pos.y == gateA_.y)
        || (pos.x == gateB_.x && pos.y == gateB_.y);
}

// 반대편 Gate 좌표 반환
Vec2 Gate::getOtherGate(const Vec2 &pos) const {
    if (pos.x == gateA_.x && pos.y == gateA_.y) return gateB_;
    else                                       return gateA_;
}

// 진입 방향과 Gate 위치에 따라 올바른 진출 방향 결정
Direction Gate::computeExitDirection(const Vec2 &pos, Direction inDir) const {
    // 1) 가장자리 Gate → 항상 안쪽 방향 (고정) :contentReference[oaicite:3]{index=3}
    if (pos.y == 0)                     return Direction::Down;
    if (pos.y == Map::HEIGHT-1)       return Direction::Up;
    if (pos.x == 0)                     return Direction::Right;
    if (pos.x == Map::WIDTH-1)        return Direction::Left;

    // 2) 중앙 Gate → inDir, 시계, 반시계, 반대 순 시도 :contentReference[oaicite:4]{index=4}
    Direction cw, ccw, opp;
    switch (inDir) {
        case Direction::Up:    cw = Direction::Right; ccw = Direction::Left;  opp = Direction::Down;  break;
        case Direction::Right: cw = Direction::Down;  ccw = Direction::Up;    opp = Direction::Left;  break;
        case Direction::Down:  cw = Direction::Left;  ccw = Direction::Right; opp = Direction::Up;    break;
        case Direction::Left:  cw = Direction::Up;    ccw = Direction::Down;  opp = Direction::Right; break;
    }
    std::vector<Direction> order = { inDir, cw, ccw, opp };

    // 각 방향으로 한 칸 이동한 뒤 EMPTY인지 검사 후 첫 번째 유효한 방향 반환
    for (auto d : order) {
        int dx = 0, dy = 0;
        if      (d == Direction::Up)    dy = -1;
        else if (d == Direction::Down)  dy =  1;
        else if (d == Direction::Left)  dx = -1;
        else if (d == Direction::Right) dx =  1;

        Vec2 next{ pos.x + dx, pos.y + dy };
        if (map_.getCell(next.x, next.y) == Map::EMPTY)
            return d;
    }
    // 혹시 전부 막혀 있으면 원래 진행 방향 유지
    return inDir;
}
