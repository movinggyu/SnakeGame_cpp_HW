#ifndef GATE_H
#define GATE_H

#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Map.h"
#include "Snake.h"

// Stage4: Gate 생성 및 통과 로직
class Gate {
public:
    // 생성자: Map 참조 보관
    Gate(Map &map);

    // 기존 Gate 제거 후, 맵의 WALL 중 2곳을 무작위로 골라 페어 생성
    void generatePair();

    // 생성된 Gate를 맵에서 모두 제거
    void clearPair();

    // pos가 Gate 위치인지 검사
    bool isGate(const Vec2 &pos) const;

    // 진입한 Gate의 반대편 Gate 좌표 반환
    Vec2 getOtherGate(const Vec2 &pos) const;

    // 진입 방향(inDir)과 Gate 위치(pos)에 따라
    // 올바른 진출 방향을 계산
    Direction computeExitDirection(const Vec2 &pos, Direction inDir) const;

private:
    Map &map_;
    Vec2 gateA_, gateB_;  // 두 Gate 좌표
};

#endif // GATE_H
