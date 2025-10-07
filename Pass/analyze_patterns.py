# analyze_patterns.py
import re
import sys
from collections import Counter, deque

pat = re.compile(r'^\[I\]\s+[^:]+::\s*[^:]*::\s*([a-zA-Z0-9_]+)\s*\{')
ops = []
with open(sys.argv[1], 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        m = pat.match(line)
        if m:
            ops.append(m.group(1))

def ngrams(seq, n):
    dq = deque(maxlen=n)
    for x in seq:
        dq.append(x)
        if len(dq) == n:
            yield tuple(dq)

for n in range(1, 6):
    cnt = Counter(ngrams(ops, n))
    print(f"# top patterns n={n}")
    for pat, c in cnt.most_common(30):
        print(f"{n}\t{c}\t{' '.join(pat)}")
    print()
