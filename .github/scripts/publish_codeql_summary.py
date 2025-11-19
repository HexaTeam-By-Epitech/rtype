#!/usr/bin/env python3
"""
GitHub Actions CodeQL Summary Publisher
Publishes CodeQL results to GitHub Actions summary and creates annotations.
"""
import os
import sys
import html
from pathlib import Path
from typing import List, Tuple

SUMMARY_PATH = 'artifacts/codeql/codeql-summary.txt'
MAX_TABLE_ROWS = 100
MAX_ANNOTATIONS = 10


def load_summary() -> List[Tuple[str, str, str, str]]:
    """
    Load and parse the summary file.

    Returns:
        List of (severity, rule, location, message) tuples
    """
    if not os.path.exists(SUMMARY_PATH):
        print(f"No summary found at {SUMMARY_PATH}")
        return []

    with open(SUMMARY_PATH, 'r', encoding='utf-8') as f:
        lines = f.read().splitlines()

    if len(lines) <= 1:
        print("Summary file is empty or has only header")
        return []

    # Parse lines, split into max 4 parts (to allow tabs in message)
    findings = []
    for line in lines[1:]:  # Skip header
        parts = line.split('\t', 3)
        while len(parts) < 4:
            parts.append('')
        findings.append(tuple(parts[:4]))

    return findings


def get_severity_emoji(severity: str) -> str:
    """Get emoji for severity level."""
    severity_lower = severity.lower()
    emoji_map = {
        'error': '🔴',
        'warning': '🟡',
        'note': '🔵',
        'information': 'ℹ️',
    }
    return emoji_map.get(severity_lower, '⚪')


def generate_html_table(findings: List[Tuple[str, str, str, str]]) -> str:
    """Generate HTML table for GitHub Actions summary."""
    if not findings:
        return '<p>✅ No findings to display</p>'

    rows_to_display = findings[:MAX_TABLE_ROWS]

    html_parts = [
        f'<h2>🔍 CodeQL Scan Summary (Top {len(rows_to_display)} of {len(findings)})</h2>',
        '<div style="max-height:500px;overflow:auto;border:1px solid #e1e4e8;border-radius:6px;">',
        '<table style="border-collapse:collapse;width:100%;font-size:13px;">',
        '<thead><tr style="background:#f6f8fa;">',
        '<th style="border:1px solid #d0d7de;padding:8px;text-align:left;font-weight:600;">Severity</th>',
        '<th style="border:1px solid #d0d7de;padding:8px;text-align:left;font-weight:600;">Rule</th>',
        '<th style="border:1px solid #d0d7de;padding:8px;text-align:left;font-weight:600;">Location</th>',
        '<th style="border:1px solid #d0d7de;padding:8px;text-align:left;font-weight:600;">Message</th>',
        '</tr></thead>',
        '<tbody>',
    ]

    for severity, rule, location, message in rows_to_display:
        emoji = get_severity_emoji(severity)
        severity_display = f"{emoji} {html.escape(severity)}" if severity else "⚪ unknown"

        # Apply color based on severity
        severity_lower = severity.lower()
        row_style = ""
        if severity_lower == 'error':
            row_style = "background:#ffeef0;"
        elif severity_lower == 'warning':
            row_style = "background:#fff8c5;"

        html_parts.extend([
            f'<tr style="{row_style}">',
            f'<td style="border:1px solid #d0d7de;padding:8px;white-space:nowrap;">{severity_display}</td>',
            f'<td style="border:1px solid #d0d7de;padding:8px;font-family:monospace;font-size:12px;">{html.escape(rule)}</td>',
            f'<td style="border:1px solid #d0d7de;padding:8px;font-family:monospace;font-size:11px;">{html.escape(location)}</td>',
            f'<td style="border:1px solid #d0d7de;padding:8px;">{html.escape(message)}</td>',
            '</tr>',
        ])

    html_parts.extend([
        '</tbody>',
        '</table>',
        '</div>',
    ])

    # Add statistics
    stats = {}
    for severity, _, _, _ in findings:
        sev_lower = severity.lower()
        stats[sev_lower] = stats.get(sev_lower, 0) + 1

    html_parts.append('<h3>📊 Statistics</h3>')
    html_parts.append('<ul style="list-style:none;padding-left:0;">')
    for sev in ['error', 'warning', 'note', 'information']:
        count = stats.get(sev, 0)
        if count > 0:
            emoji = get_severity_emoji(sev)
            html_parts.append(f'<li>{emoji} <strong>{sev.capitalize()}:</strong> {count}</li>')
    html_parts.append(f'<li>📈 <strong>Total:</strong> {len(findings)}</li>')
    html_parts.append('</ul>')

    return '\n'.join(html_parts)


def publish_to_github_summary(html_content: str):
    """Publish HTML content to GitHub Actions summary."""
    summary_file = os.environ.get('GITHUB_STEP_SUMMARY')

    if not summary_file:
        print("GITHUB_STEP_SUMMARY not set, printing to stdout:")
        print(html_content)
        return

    try:
        with open(summary_file, 'a', encoding='utf-8') as f:
            f.write('\n')
            f.write(html_content)
        print(f"✓ Published summary to {summary_file}")
    except Exception as e:
        print(f"Error writing to summary file: {e}", file=sys.stderr)


def create_github_annotations(findings: List[Tuple[str, str, str, str]]):
    """Create GitHub Actions annotations for top findings."""
    annotations_created = 0

    for severity, rule, location, message in findings[:MAX_ANNOTATIONS]:
        # Clean message for annotation
        safe_message = message.replace('\n', ' ').replace('\r', ' ').replace('\t', ' ')
        severity_lower = severity.lower()

        # Parse location
        if location and location != '?':
            if ':' in location:
                parts = location.rsplit(':', 1)
                file = parts[0]
                line = parts[1] if len(parts) > 1 and parts[1].isdigit() else '1'

                # Choose annotation type
                if severity_lower == 'error':
                    print(f"::error file={file},line={line}::{rule}: {safe_message}")
                elif severity_lower == 'warning':
                    print(f"::warning file={file},line={line}::{rule}: {safe_message}")
                else:
                    print(f"::notice file={file},line={line}::{rule}: {safe_message}")

                annotations_created += 1
            else:
                # Location without line number
                if severity_lower == 'error':
                    print(f"::error::{rule}: {safe_message}")
                elif severity_lower == 'warning':
                    print(f"::warning::{rule}: {safe_message}")
                else:
                    print(f"::notice::{rule}: {safe_message}")
                annotations_created += 1
        else:
            # No location specified
            if severity_lower == 'error':
                print(f"::error::{rule}: {safe_message}")
            elif severity_lower == 'warning':
                print(f"::warning::{rule}: {safe_message}")
            annotations_created += 1

    if annotations_created > 0:
        print(f"✓ Created {annotations_created} annotation(s)")


def main():
    """Main entry point."""
    print("Loading CodeQL summary...")
    findings = load_summary()

    if not findings:
        print("No findings to publish")
        sys.exit(0)

    print(f"Loaded {len(findings)} finding(s)")

    # Generate and publish HTML table
    print("Generating HTML summary...")
    html_content = generate_html_table(findings)
    publish_to_github_summary(html_content)

    # Create GitHub annotations
    print(f"Creating annotations (max {MAX_ANNOTATIONS})...")
    create_github_annotations(findings)

    print("✓ Summary publication complete")


if __name__ == '__main__':
    main()