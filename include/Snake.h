#ifndef SNAKE_H
#define SNAKE_H

#include <deque>
#include "Map.h"

// Stage2: 뱀 이동 및 충돌 처리
// Game Rule #1: 진행 방향의 반대 방향 입력 시 실패 (Game Over)  
//              자신의 몸 또는 벽과 충돌 시 실패  
// (Rule #1) :contentReference[oaicite:0]{index=0}
  
// 2D 좌표 구조체
struct Vec2 {
    int x, y;
};

// 이동 방향 열거형
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class Snake {
public:
    // 최대길이: 20으로 초기화
    static constexpr size_t MAX_LENGTH = 20;

    // 생성자: 맵에서 머리와 몸통 위치를 찾아 body_ 초기화
    // 시작 방향은 머리에서 두 번째 세그먼트로 향하는 반대 방향
    Snake(const Map &map);

    // 키 입력으로 다음 방향 설정
    // 진행 방향과 반대 방향으로 설정 시 무시하거나 Game Over 처리
    void setDirection(Direction d);

    // 게이트 워프 직후 반대 방향도 허용하는 강제 변경용
    void forceDirection(Direction d);

    // 한 틱마다 이동 수행
    // grow == true면 머리 쪽으로 길이 +1 (Growth Item 획득)  
    void move(bool grow);

    // 뱀의 머리를 (x, y) 좌표로 순간 이동시킵니다.
    void warpTo(int x, int y);

    // Poison Item 적용: 꼬리 쪽으로 길이 -1  
    // 길이 < 3 이면 true 반환 (Game Over)  
    bool shrink();

    // 꼬리 1 증가
    void grow();

    // 이동 직전, 다음 칸이 벽 또는 면역벽인지 검사  
    bool willHitWall(const Map &map) const;

    // 이동 직전, 다음 칸이 자기 몸과 겹치는지 검사  
    bool willHitSelf() const;

    // 이동 후 현재 머리가 벽 또는 자기 몸과 충돌했는지 검사  
    bool isCollision(const Map &map) const;

    // 현재 머리 위치 반환
    const Vec2 &getHead() const;

    // 몸통 전체 좌표 반환
    const std::deque<Vec2> &getBody() const;

    // 현재 진행 방향 반환
    Direction getDirection() const;

private:
    std::deque<Vec2> body_;   // front = head, back = tail
    Direction dir_;           // 실제 이동에 사용되는 방향
    Direction nextDir_;       // setDirection() 으로 입력된 다음 방향
};

#endif // SNAKE_H
