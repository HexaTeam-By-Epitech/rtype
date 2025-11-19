#!/usr/bin/env python3
import json
import glob
import os
import sys

out_dir = 'artifacts/codeql'
os.makedirs(out_dir, exist_ok=True)
files = glob.glob(os.path.join(out_dir, '*.sarif'))
if not files:
    print('No SARIF files found in', out_dir)
    sys.exit(0)

summary = []
# patterns to skip (third-party libs / vcpkg)
skip_patterns = ['vcpkg_installed', '/vcpkg_installed/', '/vcpkg/']
for f in files:
    try:
        with open(f, 'r', encoding='utf-8') as fh:
            j = json.load(fh)
    except Exception as e:
        print('Failed to parse', f, e)
        continue
    runs = j.get('runs', [])
    for run in runs:
        results = run.get('results', [])
        for r in results:
            rid = r.get('ruleId', 'N/A')
            level = r.get('level', r.get('kind', '')) or ''
            msg = r.get('message', {}).get('text', '')
            if not msg:
                msg = r.get('message', {}).get('markdown', '')
            msg = msg.strip().splitlines()[0] if msg else ''
            loc = '?'
            locs = r.get('locations', [])
            if locs:
                try:
                    pl = locs[0]['physicalLocation']
                    uri = pl.get('artifactLocation', {}).get('uri', '')
                    start = pl.get('region', {}).get('startLine', '?')
                    loc = f"{uri}:{start}"
                except Exception:
                    loc = '?'

            # Skip findings that are inside vendored / package-managed libraries (vcpkg, etc.)
            if loc and any(pat in loc for pat in skip_patterns):
                # Debug: uncomment to print skipped locations
                # print('Skipping third-party finding:', loc, rid)
                continue

            summary.append((level, rid, loc, msg))

if not summary:
    print('No results found in SARIF files')
    sys.exit(0)

# Sort severity: errors first, then warnings, then others
order = {'error': 0, 'warning': 1, 'note': 2, 'information': 3, 'recommendation': 4, '': 5}
summary.sort(key=lambda x: (order.get(x[0].lower(), 9), x[1]))

out_path = os.path.join(out_dir, 'codeql-summary.txt')
with open(out_path, 'w', encoding='utf-8') as outf:
    outf.write('severity\trule\tlocation\tmessage\n')
    for level, rid, loc, msg in summary:
        line = f"{level}\t{rid}\t{loc}\t{msg}"
        print(line)
        outf.write(line + '\n')

print('\nWrote summary to', out_path)
