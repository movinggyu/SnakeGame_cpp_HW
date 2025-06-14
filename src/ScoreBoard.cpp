#include "ScoreBoard.h"
#include "Snake.h"
#include <ncurses.h>
extern int frameDelayMs;

// 기본 미션: 길이>=10, 성장>=5, 독<=2, 게이트>=1
ScoreBoard::ScoreBoard(int initLength)
    : length_(initLength), maxLength_(initLength),
      growCount_(0), poisonCount_(0), gateCount_(0),
      targetLength_(4), targetGrow_(0), targetPoison_(0), targetGate_(0)
{}
// 5,3,2,1

void ScoreBoard::updateGrow() {
    growCount_++;
    length_++;
    if (length_ > maxLength_) maxLength_ = length_;
}

void ScoreBoard::updatePoison() {
    poisonCount_++;
    if (length_ > 0) length_--;

    if (length_ < 3) gameOver_ = true;
}

void ScoreBoard::updateGate() {
    gateCount_++;
}

void ScoreBoard::draw(int elapsedSec) const {
    int x = Map::WIDTH + 2;
    int y = 1;

    mvprintw(y++, x, "=== Score Board ===");
    mvprintw(y++, x, "B: %d / %d", length_, maxLength_);
    mvprintw(y++, x, "+: %d", growCount_);
    mvprintw(y++, x, "-: %d", poisonCount_);
    mvprintw(y++, x, "G: %d", gateCount_);
    mvprintw(y++, x, "Time: %d s", elapsedSec);
    mvprintw(y++, x, "Frame Delay: %d s", frameDelayMs); // 5단계 Score Board 변경 : 추가적인 정보 (추가사항)

    y++;
    mvprintw(y++, x, "=== Mission ===");
    mvprintw(y++, x, "Length >= %d   [%c]", targetLength_, length_>= targetLength_ ? 'v' : ' ');
    mvprintw(y++, x, "Grow   >= %d   [%c]", targetGrow_, growCount_ >= targetGrow_ ? 'v' : ' ');
    mvprintw(y++, x, "Poison <= %d   [%c]", targetPoison_, poisonCount_ >= targetPoison_ ? 'v' : ' ');
    mvprintw(y++, x, "Gate   >= %d   [%c]", targetGate_, gateCount_ >= targetGate_ ? 'v' : ' ');
    mvprintw(y++, x, "Delay  <= 100 [%c]", frameDelayMs <= 100 ? 'v' : ' ');
}

bool ScoreBoard::checkMission() const {
    return length_   >= targetLength_
        && growCount_>= targetGrow_
        && poisonCount_>= targetPoison_
        && gateCount_ >= targetGate_
        && frameDelayMs <= 100;
}
