#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <string>
#include "Map.h"

// ScoreBoard: 뱀 길이, 아이템·게이트 횟수 및 미션 목표 관리
class ScoreBoard {
    bool gameOver_ = false;

public:
    // 생성자: 초기 길이 설정 및 내부 카운트 초기화
    ScoreBoard(int initLength = 3);

    // 성장 아이템 획득 시 호출 (길이 증가 및 카운트)
    void updateGrow();

    // 독 아이템 획득 시 호출 (길이 감소 및 카운트)
    void updatePoison();

    // 게이트 통과 시 호출 (카운트 증가)
    void updateGate();

    // 경과 시간과 함께 점수판·미션 UI 출력
    void draw(int elapsedSec) const;

    // 모든 미션 목표 달성 여부 반환
    bool checkMission() const;

    // 게임 오버 상태 반환
    bool isGameOver() const { return gameOver_; }

private:
    int length_;       // 현재 뱀 길이
    int maxLength_;    // 기록된 최대 길이
    int growCount_;    // 성장 아이템 획득 수
    int poisonCount_;  // 독 아이템 획득 수
    int gateCount_;    // 게이트 통과 수

    // 미션 목표치
    int targetLength_;  // 목표 길이
    int targetGrow_;    // 목표 성장 아이템 수
    int targetPoison_;  // 허용 독 아이템 수
    int targetGate_;    // 목표 게이트 통과 수
};

#endif // SCOREBOARD_H
