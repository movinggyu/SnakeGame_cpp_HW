#include "ScoreBoard.h"
#include "Snake.h"
#include <ncurses.h>
extern int frameDelayMs;

// 생성자: 초기 뱀 길이와 미션 목표 초기화
ScoreBoard::ScoreBoard(int initLength)
    : length_(initLength), maxLength_(initLength),
      growCount_(0), poisonCount_(0), gateCount_(0),
      targetLength_(5), targetGrow_(3), targetPoison_(2), targetGate_(1)
{}

// 성장 아이템 사용 시: 성장 카운트 증가 및 길이 증가
void ScoreBoard::updateGrow() {
    growCount_++;
    length_++;
    if (length_ > maxLength_) maxLength_ = length_;
}

// 독 아이템 사용 시: 독 카운트 증가, 길이 감소 및 3 미만 시 게임 오버
void ScoreBoard::updatePoison() {
    poisonCount_++;
    if (length_ > 0) length_--;
    if (length_ < 3) gameOver_ = true;
}

// 게이트 통과 시: 게이트 카운트 증가
void ScoreBoard::updateGate() {
    gateCount_++;
}

// 점수판 및 미션 상태를 화면에 출력
void ScoreBoard::draw(int elapsedSec) const {
    int x = Map::WIDTH + 2;
    int y = 1;

    mvprintw(y++, x, "=== Score Board ===");
    mvprintw(y++, x, "B: %d / %d", length_, maxLength_);
    mvprintw(y++, x, "+: %d", growCount_);
    mvprintw(y++, x, "-: %d", poisonCount_);
    mvprintw(y++, x, "G: %d", gateCount_);
    mvprintw(y++, x, "Time: %d s", elapsedSec);
    mvprintw(y++, x, "Frame Delay: %d ms", frameDelayMs); // 과제 구현사항) 프레임 딜레이

    y++;
    mvprintw(y++, x, "=== Mission ===");
    mvprintw(y++, x, "Length >= %d   [%c]", targetLength_, length_ >= targetLength_ ? 'v' : ' ');
    mvprintw(y++, x, "Grow   >= %d   [%c]", targetGrow_, growCount_ >= targetGrow_ ? 'v' : ' ');
    mvprintw(y++, x, "Poison <= %d   [%c]", targetPoison_, poisonCount_ >= targetPoison_ ? 'v' : ' ');
    mvprintw(y++, x, "Gate   >= %d   [%c]", targetGate_, gateCount_ >= targetGate_ ? 'v' : ' ');
    mvprintw(y++, x, "Delay  <= 100 [%c]", frameDelayMs <= 100 ? 'v' : ' '); // 과제 구현사항) 최대 지연 제한
}

// 미션 달성 여부 반환
bool ScoreBoard::checkMission() const {
    return length_    >= targetLength_
        && growCount_ >= targetGrow_
        && poisonCount_>= targetPoison_
        && gateCount_ >= targetGate_
        && frameDelayMs <= 100;
}
