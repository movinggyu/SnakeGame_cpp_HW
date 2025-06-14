#ifndef SNAKE_H
#define SNAKE_H

#include <deque>
#include "Map.h"

// Vec2: 2D 좌표 구조체
struct Vec2 {
    int x, y;
};

// Direction: 이동 방향
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

// Snake: 뱀 이동 및 충돌 처리 클래스
class Snake {
public:
    static constexpr size_t MAX_LENGTH = 20;  // 최대 길이

    // 생성자: 맵에서 머리·몸통 위치 탐색 후 body_ 초기화
    Snake(const Map &map);

    // setDirection: 반대 방향 입력은 무시
    void setDirection(Direction d);

    // forceDirection: 강제 방향 설정 (게이트 워프 직후)
    void forceDirection(Direction d);

    // move: 이동 수행; grow=true면 길이 증가
    void move(bool grow);

    // warpTo: 머리를 지정 좌표로 순간 이동
    void warpTo(int x, int y);

    // shrink: 길이 감소; 길이 <3이면 true 반환
    bool shrink();

    // grow: 길이 증가
    void grow();

    // willHitWall: 이동 전 벽 충돌 여부 반환
    bool willHitWall(const Map &map) const;

    // willHitSelf: 이동 전 자기 몸과 충돌 여부 반환
    bool willHitSelf() const;

    // isCollision: 이동 후 벽 또는 자기 몸 충돌 여부 반환
    bool isCollision(const Map &map) const;

    // 상태 조회자들
    const Vec2 &getHead() const;
    const std::deque<Vec2> &getBody() const;
    Direction getDirection() const;

private:
    std::deque<Vec2> body_;   // 뱀 몸통: front=머리, back=꼬리
    Direction dir_;           // 현재 이동 방향
    Direction nextDir_;       // 다음 이동 방향
};

#endif // SNAKE_H
