## 2단계에서 Tick에 대한 입력처리방법 (추가사항)

(main.cpp)

매 프레임마다 getch()를 한 번 호출해 키 입력을 즉시 확인합니다. nodelay(stdscr, TRUE) 모드를 켜 두면, 키가 눌리지 않은 순간에도 getch()가 곧바로 ERR를 반환하므로 입력을 기다리지 않고 바로 다음 로직으로 넘어갈 수 있습니다.

게임 루프는 크게 “입력 → 이동·충돌 처리 → 화면 갱신” 순서로 이뤄집니다.

1. int ch = getch(); 로 키를 읽고, 방향키 입력이면 snake.setDirection()에 전달

2. 입력이 없으면(혹은 다른 키면) 아무것도 바꾸지 않고 기존 방향 유지

3. snake.move() 등 게임 객체를 업데이트

마지막에 napms(FRAME_DELAY_MS)를 호출해 프레임당 고정된 시간(ms)만큼 일시 정지합니다. 이동안에도 터미널은 누른 키를 내부 큐에 쌓아 두었다가, 다음 루프의 getch()에서 순서대로 꺼내 쓰기 때문에 키 입력이 절대 누락되지 않습니다.

그래서 여러입력을 동시에 입력해도 한 프레임당 한 입력씩 순서대로 처리하게 됩니다.

## 3단계에서 새로운 아이템 추가 (추가 사항)

