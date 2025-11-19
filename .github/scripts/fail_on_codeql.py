#!/usr/bin/env python3
"""
CodeQL Findings Checker
Fails the CI job if critical or high severity findings are detected.
"""
import sys
import os
from pathlib import Path
from typing import Dict

SUMMARY_PATH = 'artifacts/codeql/codeql-summary.txt'
FAIL_ON_SEVERITIES = ['error', 'warning']  # Fail on these severity levels


def count_findings_by_severity(summary_path: str) -> Dict[str, int]:
    """
    Count findings by severity level.

    Returns:
        Dictionary mapping severity to count
    """
    if not os.path.exists(summary_path):
        print(f"No summary found at {summary_path}")
        return {}

    severity_counts = {}

    with open(summary_path, 'r', encoding='utf-8') as f:
        # Skip header
        next(f, None)

        for line in f:
            parts = line.strip().split('\t')
            if not parts:
                continue

            severity = parts[0].strip().lower()
            severity_counts[severity] = severity_counts.get(severity, 0) + 1

    return severity_counts


def print_summary(severity_counts: Dict[str, int]):
    """Print a formatted summary of findings."""
    print("\n" + "="*60)
    print("CodeQL Findings Summary")
    print("="*60)

    total = sum(severity_counts.values())

    if not severity_counts:
        print("✅ No findings detected")
        return

    # Print each severity
    severity_order = ['error', 'warning', 'note', 'information']
    for severity in severity_order:
        count = severity_counts.get(severity, 0)
        if count > 0:
            emoji = {
                'error': '🔴',
                'warning': '🟡',
                'note': '🔵',
                'information': 'ℹ️'
            }.get(severity, '⚪')
            print(f"{emoji} {severity.upper()}: {count}")

    # Print any unknown severities
    for severity, count in severity_counts.items():
        if severity not in severity_order and count > 0:
            print(f"⚪ {severity.upper()}: {count}")

    print(f"\n📈 Total findings: {total}")
    print("="*60 + "\n")


def should_fail(severity_counts: Dict[str, int]) -> bool:
    """
    Determine if CI should fail based on severity counts.

    Returns:
        True if job should fail, False otherwise
    """
    critical_count = 0

    for severity in FAIL_ON_SEVERITIES:
        count = severity_counts.get(severity, 0)
        critical_count += count

    return critical_count > 0


def main():
    """Main entry point."""
    print("Checking CodeQL findings...")

    # Count findings
    severity_counts = count_findings_by_severity(SUMMARY_PATH)

    # Print summary
    print_summary(severity_counts)

    # Determine if we should fail
    if should_fail(severity_counts):
        error_count = severity_counts.get('error', 0)
        warning_count = severity_counts.get('warning', 0)

        print(f"❌ FAILING: Found {error_count} error(s) and {warning_count} warning(s)")
        print(f"   Failed severity levels: {', '.join(FAIL_ON_SEVERITIES)}")
        print("\n💡 Review the CodeQL summary and SARIF artifacts for details")
        sys.exit(1)

    print("✅ No critical or high severity findings detected")
    print("   Job will continue")
    sys.exit(0)


if __name__ == '__main__':
    main()