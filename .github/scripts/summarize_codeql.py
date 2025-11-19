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

# Normalisation centralisée de la sévérité
def normalize_sev(val):
    """Normalize various severity representations to one of: 'error', 'warning', 'note' or ''.
    Handles numeric priorities, common strings (high/critical -> error), and returns
    a cleaned lowercase token otherwise.
    """
    if not val:
        return ''
    v = str(val).strip().lower()
    # numeric priority mapping if present
    if v.isdigit():
        n = int(v)
        if n <= 1:
            return 'error'
        if n == 2:
            return 'warning'
        return 'note'
    # map common synonyms
    if v in ('critical', 'high'):
        return 'error'
    if v in ('error', 'failure'):
        return 'error'
    if v in ('warning', 'warn', 'medium'):
        return 'warning'
    if v in ('note', 'info', 'informational', 'information', 'low'):
        return 'note'
    return v

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
        # build a map of rules metadata for fallback severity
        rules_meta = {}
        try:
            tool = run.get('tool', {}).get('driver', {})
            for rmeta in tool.get('rules', []) or []:
                rid = rmeta.get('id') or rmeta.get('name')
                rules_meta[rid] = rmeta
        except Exception:
            rules_meta = {}

        results = run.get('results', [])
        for r in results:
            rid = r.get('ruleId', 'N/A')

            # Attempt to determine severity from multiple possible SARIF fields
            level = r.get('level') or r.get('severity') or r.get('kind') or ''
            # some SARIF put severity in properties
            props = r.get('properties', {}) or {}
            if not level and props:
                level = props.get('severity') or props.get('priority') or props.get('level') or props.get('severityLabel') or ''

            # try to get severity from a rule object attached directly to the result
            rule_obj = r.get('rule') or r.get('ruleObject') or {}
            if not level and isinstance(rule_obj, dict):
                level = (rule_obj.get('properties') or {}).get('severity') or rule_obj.get('defaultConfiguration', {}).get('level', '')

            # fallback to rule metadata defaultConfiguration or properties
            if not level and rid in rules_meta:
                try:
                    level = rules_meta[rid].get('defaultConfiguration', {}).get('level', '') or (rules_meta[rid].get('properties') or {}).get('severity', '')
                except Exception:
                    level = ''

            # sometimes the rule id in results doesn't match the key used in rules_meta; try to find a matching rule
            if not level:
                for k, rm in rules_meta.items():
                    try:
                        if k == rid or rm.get('id') == rid or rm.get('name') == rid or (isinstance(rm.get('id'), str) and rm.get('id').endswith(rid)):
                            level = (rm.get('defaultConfiguration', {}) or {}).get('level', '') or (rm.get('properties') or {}).get('severity', '')
                            if level:
                                break
                    except Exception:
                        continue

            level = normalize_sev(level)

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
