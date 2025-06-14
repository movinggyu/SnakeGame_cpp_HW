import os

def list_directory(path: str, prefix: str = ''):
    """경로(path) 아래의 디렉토리/파일을 트리 구조로 출력."""
    try:
        entries = sorted(os.listdir(path))
    except PermissionError:
        print(prefix + '└── [권한 없음]')
        return

    entries_count = len(entries)
    for idx, entry in enumerate(entries):
        full_path = os.path.join(path, entry)
        # 마지막 항목인지 판단해서 connector 결정
        connector = '└── ' if idx == entries_count - 1 else '├── '
        print(prefix + connector + entry)

        # 디렉토리면 재귀 호출
        if os.path.isdir(full_path):
            # 다음 레벨 prefix: 마지막이면 공백, 아니면 │
            extension = '    ' if idx == entries_count - 1 else '│   '
            list_directory(full_path, prefix + extension)

if __name__ == '__main__':
    root = input("트리 출력할 디렉토리 경로를 입력하세요 (기본: 현재 폴더): ") or '.'
    if os.path.isdir(root):
        print(root)
        list_directory(root)
    else:
        print(f"유효하지 않은 경로입니다: {root}")
