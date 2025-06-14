#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <string>
#include "Map.h"

// Stage5: 점수판 및 미션 시스템
class ScoreBoard {
    bool gameOver_ = false;
public:
    // 생성자: 초기 길이(length), 최대 길이(maxLength), 아이템·게이트 카운트(grow, poison, gate) 초기화
    ScoreBoard(int initLength = 3);

    // 성장 아이템 획득 시 호출: 길이 증가 및 카운트 업데이트
    void updateGrow();

    // 독 아이템 획득 시 호출: 길이 감소 및 카운트 업데이트
    void updatePoison();

    // 게이트 통과 시 호출: 게이트 카운트 증가
    void updateGate();

    // 경과 시간(elapsedSec)과 함께 UI에 점수판 및 미션 표시
    void draw(int elapsedSec) const;

    // 미션 달성 여부 반환: 모든 목표치 초과·충족 시 true
    bool checkMission() const;

    // 게임오버 조회자
    bool isGameOver() const{
        return gameOver_;
    }

private:
    int length_;       // 현재 뱀 길이
    int maxLength_;    // 현재까지 가장 길었던 길이
    int growCount_;    // 획득한 성장 아이템 수
    int poisonCount_;  // 획득한 독 아이템 수
    int gateCount_;    // 통과한 게이트 수
    
    // 미션 목표치
    int targetLength_;  // 목표 길이
    int targetGrow_;    // 목표 성장 아이템 수
    int targetPoison_;  // 목표 독 아이템 허용 수
    int targetGate_;    // 목표 게이트 통과 수
};

#endif // SCOREBOARD_H