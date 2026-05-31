import glob
from pathlib import Path

root = Path(__file__).resolve().parent
pattern = root / 'ios_src' / 'esp' / '*.h'

for f in glob.glob(str(pattern)):
    with open(f, 'r', encoding='utf-8') as file:
        content = file.read()
    content = content.replace('Memory::', 'InternalMemory::')
    content = content.replace('"memory.h"', '"../memory_internal.h"')
    with open(f, 'w', encoding='utf-8') as file:
        file.write(content)
print("Done")
