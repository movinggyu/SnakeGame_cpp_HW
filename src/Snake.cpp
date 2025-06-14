#include "Snake.h"
#include <algorithm>
#include <stdexcept>

// 생성자: 맵에서 뱀 머리와 몸통을 찾아 body_ 초기화
// 이어진 세그먼트를 따라 추가하고, 머리→두 번째 세그먼트 방향의 반대로 dir_ 설정
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
    if (headPos.x < 0) throw std::runtime_error("맵에 뱀 머리가 없습니다.");

    // 첫 세그먼트 추가
    body_.push_back(headPos);
    Vec2 prev = headPos, curr;
    std::vector<Vec2> dirs = {{0,-1},{0,1},{-1,0},{1,0}};
    for (auto &d : dirs) {
        Vec2 p = {headPos.x + d.x, headPos.y + d.y};
        if (map.getCell(p.x, p.y) == Map::SNAKE_BODY) {
            curr = p; body_.push_back(curr);
            break;
        }
    }
    if (body_.size() < 2) throw std::runtime_error("맵에 뱀 몸통이 없습니다.");

    // 추가 몸통 이어 붙이기
    while (true) {
        bool found = false;
        for (auto &d : dirs) {
            Vec2 next = {curr.x + d.x, curr.y + d.y};
            if (next.x==prev.x && next.y==prev.y) continue;
            if (map.getCell(next.x, next.y) == Map::SNAKE_BODY) {
                body_.push_back(next);
                prev = curr; curr = next; found = true;
                break;
            }
        }
        if (!found) break;
    }

    // 초기 방향 설정
    Vec2 sec = body_[1];
    Vec2 vec = {headPos.x - sec.x, headPos.y - sec.y};
    if (vec.x == 1)      dir_ = nextDir_ = Direction::Right;
    else if (vec.x == -1) dir_ = nextDir_ = Direction::Left;
    else if (vec.y == 1)  dir_ = nextDir_ = Direction::Down;
    else                  dir_ = nextDir_ = Direction::Up;
}

// setDirection: 반대 방향 입력 시 무시
void Snake::setDirection(Direction d) {
    if ((dir_==Direction::Up && d==Direction::Down) ||
        (dir_==Direction::Down && d==Direction::Up) ||
        (dir_==Direction::Left && d==Direction::Right) ||
        (dir_==Direction::Right && d==Direction::Left)) return;
    nextDir_ = d;
}

// forceDirection: 강제 방향 변경 (게이트 워프 후 사용)
void Snake::forceDirection(Direction d) {
    dir_ = nextDir_ = d;
}

// move: nextDir_로 머리 위치 업데이트
// grow=true 시 꼬리 제거 없이 길이 증가
void Snake::move(bool grow) {
    dir_ = nextDir_;
    Vec2 newHead = body_.front();
    switch (dir_) {
        case Direction::Up:    --newHead.y; break;
        case Direction::Down:  ++newHead.y; break;
        case Direction::Left:  --newHead.x; break;
        case Direction::Right: ++newHead.x; break;
    }
    body_.push_front(newHead);
    if (!grow) body_.pop_back();
}

// warpTo: 머리를 지정 좌표로 이동
void Snake::warpTo(int x, int y) {
    if (!body_.empty()) {
        body_.front().x = x;
        body_.front().y = y;
    }
}

// grow: 꼬리 끝 복제해 길이 증가 (MAX_LENGTH 제한)
void Snake::grow() {
    if (body_.size() < MAX_LENGTH) body_.push_back(body_.back());
}

// shrink: 꼬리 하나 제거, 길이 <3 시 true 반환
bool Snake::shrink() {
    if (!body_.empty()) body_.pop_back();
    return body_.size() < 3;
}

// willHitWall: nextDir_ 기준 벽 또는 면역벽 충돌 예상 시 true
bool Snake::willHitWall(const Map &map) const {
    Vec2 next = body_.front();
    switch (nextDir_) {
        case Direction::Up:    --next.y; break;
        case Direction::Down:  ++next.y; break;
        case Direction::Left:  --next.x; break;
        case Direction::Right: ++next.x; break;
    }
    auto cell = map.getCell(next.x, next.y);
    return cell==Map::WALL || cell==Map::IMMUNE_WALL;
}

// willHitSelf: nextDir_ 기준 몸통과 겹칠 경우 true
bool Snake::willHitSelf() const {
    Vec2 next = body_.front();
    switch (nextDir_) {
        case Direction::Up:    --next.y; break;
        case Direction::Down:  ++next.y; break;
        case Direction::Left:  --next.x; break;
        case Direction::Right: ++next.x; break;
    }
    return std::any_of(body_.begin(), body_.end(),
        [&next](const Vec2 &seg){ return seg.x==next.x && seg.y==next.y; });
}

// isCollision: 현재 머리 위치 충돌 검사 (벽 또는 자기 몸)
bool Snake::isCollision(const Map &map) const {
    const Vec2 &head = body_.front();
    auto cell = map.getCell(head.x, head.y);
    if (cell==Map::WALL || cell==Map::IMMUNE_WALL) return true;
    for (size_t i=1; i<body_.size(); ++i) {
        if (body_[i].x==head.x && body_[i].y==head.y) return true;
    }
    return false;
}

// 상태 조회: 머리 좌표, 몸통 전체, 현재 방향
const Vec2 &Snake::getHead() const { return body_.front(); }
const std::deque<Vec2> &Snake::getBody() const { return body_; }
Direction Snake::getDirection() const { return dir_; }
