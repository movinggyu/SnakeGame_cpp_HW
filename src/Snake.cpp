#include "Snake.h"
#include <algorithm>
#include <stdexcept>

// 생성자: Map에서 SNAKE_HEAD, SNAKE_BODY 찾아 초기 위치 설정
Snake::Snake(const Map &map) {
    Vec2 headPos{-1,-1};
    // 머리 위치 탐색
    for (int y = 0; y < Map::HEIGHT; ++y) {
        for (int x = 0; x < Map::WIDTH; ++x) {
            if (map.getCell(x, y) == Map::SNAKE_HEAD) {
                headPos = {x, y};
                break;
            }
        }
        if (headPos.x >= 0) break;
    }
    if (headPos.x < 0)
        throw std::runtime_error("맵에 뱀 머리가 없습니다.");

    // 몸통 세그먼트 탐색 및 순서대로 body_에 추가
    body_.push_back(headPos);
    Vec2 prev = headPos;
    Vec2 curr;
    // 방향 배열 (상, 하, 좌, 우)
    std::vector<Vec2> dirs = {{0,-1},{0,1},{-1,0},{1,0}};

    // 첫 번째 몸통 세그먼트 찾기
    for (auto &d : dirs) {
        Vec2 p = { headPos.x + d.x, headPos.y + d.y };
        if (map.getCell(p.x, p.y) == Map::SNAKE_BODY) {
            curr = p;
            body_.push_back(curr);
            break;
        }
    }
    if (body_.size() < 2)
        throw std::runtime_error("맵에 뱀 몸통이 없습니다.");

    // 추가 몸통을 순서대로 찾기
    while (true) {
        bool found = false;
        for (auto &d : dirs) {
            Vec2 next = { curr.x + d.x, curr.y + d.y };
            if (next.x == prev.x && next.y == prev.y) continue;
            if (map.getCell(next.x, next.y) == Map::SNAKE_BODY) {
                body_.push_back(next);
                prev = curr; curr = next;
                found = true;
                break;
            }
        }
        if (!found) break;
    }

    // 초기 이동 방향 = 머리에서 두 번째 세그먼트를 향하는 반대 방향
    Vec2 second = body_[1];
    Vec2 vec = { headPos.x - second.x, headPos.y - second.y };
    if (vec.x == 1) dir_ = nextDir_ = Direction::Right;
    else if (vec.x == -1) dir_ = nextDir_ = Direction::Left;
    else if (vec.y == 1) dir_ = nextDir_ = Direction::Down;
    else dir_ = nextDir_ = Direction::Up;
}

// 키 입력으로 다음 방향 설정
// 진행 방향의 정반대 입력은 무시
void Snake::setDirection(Direction d) {
    if ((dir_ == Direction::Up    && d == Direction::Down)  ||
        (dir_ == Direction::Down  && d == Direction::Up)    ||
        (dir_ == Direction::Left  && d == Direction::Right) ||
        (dir_ == Direction::Right && d == Direction::Left))
    {
        return;
    }
    nextDir_ = d;
}

// 게이트 워프 이후 진행방향 180도 반대 방향으로도 강제로 바꿔주는 역할
void Snake::forceDirection(Direction d) {
    dir_     = d;
    nextDir_ = d;
}

// 한 틱마다 이동 수행
// grow == true 면 머리 쪽으로 길이 +1, 아니면 꼬리 하나 제거
void Snake::move(bool grow) {
    dir_ = nextDir_;               // 실제 이동 방향 갱신
    Vec2 newHead = body_.front();  // 현재 머리 위치
    switch (dir_) {
        case Direction::Up:    --newHead.y; break;
        case Direction::Down:  ++newHead.y; break;
        case Direction::Left:  --newHead.x; break;
        case Direction::Right: ++newHead.x; break;
    }
    body_.push_front(newHead);     // 새 머리 추가
    body_.pop_back();          // 꼬리 제거
}

// 뱀 몸통(deque)의 front 요소(머리)의 좌표를
// 전달받은 (x, y)로 변경하여 게이트 반대편으로 이동시킨다.
void Snake::warpTo(int x, int y) {
    if (!body_.empty()) {
        body_.front().x = x;
        body_.front().y = y;
    }
}

void Snake::grow() {
    if (body_.size() >= MAX_LENGTH) return;
    // 꼬리 끝을 그대로 복제해서 길이 증가
    body_.push_back(body_.back());
}

// Poison 아이템 적용: 꼬리 쪽으로 길이 -1
// 길이 < 3 이면 Game Over (true 반환)
bool Snake::shrink() {
    if (!body_.empty())
        body_.pop_back();
    return (body_.size() < 3);
}

// 이동 전, 다음 칸이 벽 또는 면역벽인지 검사
bool Snake::willHitWall(const Map &map) const {
    Vec2 next = body_.front();
    switch (nextDir_) {
        case Direction::Up:    --next.y; break;
        case Direction::Down:  ++next.y; break;
        case Direction::Left:  --next.x; break;
        case Direction::Right: ++next.x; break;
    }
    auto cell = map.getCell(next.x, next.y);
    return (cell == Map::WALL || cell == Map::IMMUNE_WALL);
}

// 이동 전, 다음 칸이 자신의 몸과 겹치는지 검사
bool Snake::willHitSelf() const {
    Vec2 next = body_.front();
    switch (nextDir_) {
        case Direction::Up:    --next.y; break;
        case Direction::Down:  ++next.y; break;
        case Direction::Left:  --next.x; break;
        case Direction::Right: ++next.x; break;
    }
    // 몸통 전체 검사
    return std::any_of(body_.begin(), body_.end(),
        [&next](const Vec2 &seg){
            return seg.x == next.x && seg.y == next.y;
        });
}

// 이동 후, 현재 머리가 벽 또는 자기 몸과 충돌했는지 검사
bool Snake::isCollision(const Map &map) const {
    const Vec2 &head = body_.front();
    auto cell = map.getCell(head.x, head.y);
    if (cell == Map::WALL || cell == Map::IMMUNE_WALL)
        return true;
    // 자기 몸(머리 제외)과의 충돌 검사
    for (size_t i = 1; i < body_.size(); ++i) {
        if (body_[i].x == head.x && body_[i].y == head.y)
            return true;
    }
    return false;
}

// 현재 머리 위치 반환
const Vec2 &Snake::getHead() const {
    return body_.front();
}

// 몸통 전체 좌표 반환
const std::deque<Vec2> &Snake::getBody() const {
    return body_;
}

// 현재 이동 방향 반환
Direction Snake::getDirection() const {
    return dir_;
}
