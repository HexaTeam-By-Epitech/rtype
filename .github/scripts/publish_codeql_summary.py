#!/usr/bin/env python3
import os
import html

summary_path = 'artifacts/codeql/codeql-summary.txt'
if not os.path.exists(summary_path):
    print('No summary to publish')
    raise SystemExit(0)

lines = open(summary_path, 'r', encoding='utf-8').read().splitlines()
if len(lines) <= 1:
    print('No findings to publish')
    raise SystemExit(0)

# split each line into at most 4 parts to allow tabs in the message column
rows = [l.split('\t', 3) for l in lines[1:101]]  # up to 100 rows
# Build a simple HTML table
content = []
content.append('<h2>CodeQL scan summary (top {})</h2>'.format(len(rows)))
content.append('<div style="max-height:400px;overflow:auto;">')
content.append('<table style="border-collapse:collapse; width:100%;">')
content.append('<thead><tr>')
content.append('<th style="border:1px solid #ddd;padding:6px;background:#f6f8fa;text-align:left">Severity</th>')
content.append('<th style="border:1px solid #ddd;padding:6px;background:#f6f8fa;text-align:left">Rule</th>')
content.append('<th style="border:1px solid #ddd;padding:6px;background:#f6f8fa;text-align:left">Location</th>')
content.append('<th style="border:1px solid #ddd;padding:6px;background:#f6f8fa;text-align:left">Message</th>')
content.append('</tr></thead><tbody>')
for parts in rows:
    while len(parts) < 4:
        parts.append('')
    sev, rule, loc, msg = [html.escape(p) for p in parts[:4]]
    content.append('<tr>')
    content.append(f'<td style="border:1px solid #ddd;padding:6px">{sev}</td>')
    content.append(f'<td style="border:1px solid #ddd;padding:6px">{rule}</td>')
    content.append(f'<td style="border:1px solid #ddd;padding:6px">{loc}</td>')
    content.append(f'<td style="border:1px solid #ddd;padding:6px">{msg}</td>')
    content.append('</tr>')
content.append('</tbody></table>')
content.append('</div>')

# Write into GITHUB_STEP_SUMMARY
summary_file = os.environ.get('GITHUB_STEP_SUMMARY')
if summary_file:
    with open(summary_file, 'a', encoding='utf-8') as out:
        out.write('\n')
        out.write('\n'.join(content))
    print('Published summary to', summary_file)
else:
    print('\n'.join(content))
    print('No GITHUB_STEP_SUMMARY env var set; printed summary to stdout')

# Emit top 10 annotations as GitHub workflow command lines to stdout
for parts in rows[:10]:
    while len(parts) < 4:
        parts.append('')
    sev, rule, loc, msg = parts[:4]
    safe_msg = msg.replace('\n', ' ').replace('\r', ' ').replace('\t', ' ')
    sev_norm = (sev or '').strip().lower()
    if loc and loc != '?':
        # try to split path:line
        if ':' in loc:
            file, line = loc.rsplit(':', 1)
            line = line if line.isdigit() else '1'
            if sev_norm == 'error':
                print(f"::error file={file},line={line}::{rule}: {safe_msg}")
            else:
                print(f"::warning file={file},line={line}::{rule}: {safe_msg}")
        else:
            if sev_norm == 'error':
                print(f"::error ::{rule}: {safe_msg}")
            else:
                print(f"::warning ::{rule}: {safe_msg}")
    else:
        if sev_norm == 'error':
            print(f"::error ::{rule}: {safe_msg}")
        else:
            print(f"::warning ::{rule}: {safe_msg}")
