#ifndef GATE_H
#define GATE_H

#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include "Map.h"
#include "Snake.h"

// Gate: 맵 벽 셀 중 두 곳을 선택해 순간이동 게이트로 설정하고,
//       뱀의 입·출구 방향을 계산해 통과를 처리하는 클래스
class Gate {
public:
    // 생성자: 맵 참조 저장 및 랜덤 시드 초기화
    Gate(Map &map);

    // 기존 게이트 제거 후 새로 두 개 생성
    void generatePair();

    // 맵에서 게이트 제거 및 원래 벽으로 복원
    void clearPair();

    // 해당 좌표가 게이트인지 확인
    bool isGate(const Vec2 &pos) const;

    // 진입한 게이트의 반대편 좌표 반환
    Vec2 getOtherGate(const Vec2 &pos) const;

    // 진입 방향과 위치에 따라 올바른 출구 방향 계산
    Direction computeExitDirection(const Vec2 &pos, Direction inDir) const;

private:
    Map &map_;           // 게임 맵 참조
    Vec2 gateA_, gateB_; // 두 게이트 위치
};

#endif // GATE_H
