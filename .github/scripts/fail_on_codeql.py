#!/usr/bin/env python3
import sys
import os

summary_path = 'artifacts/codeql/codeql-summary.txt'
if not os.path.exists(summary_path):
    print('No codeql summary found at', summary_path)
    sys.exit(0)

errors = 0
warnings = 0
with open(summary_path, 'r', encoding='utf-8') as f:
    next(f, None)  # skip header
    for line in f:
        parts = line.strip().split('\t')
        if not parts:
            continue
        sev = parts[0].strip().lower()
        if sev == 'error':
            errors += 1
        elif sev == 'warning':
            warnings += 1

print(f'CodeQL summary: {errors} error(s), {warnings} warning(s)')
# Treat both errors and warnings as fail conditions (critiques ou hautes)
if errors > 0 or warnings > 0:
    print('Failing job due to CodeQL high/critical findings')
    sys.exit(1)

print('No high/critical CodeQL findings')
sys.exit(0)

