#!/usr/bin/env python3
"""
Generate GitHub Actions summary for tests and coverage.
"""
import os
import sys
import json
from pathlib import Path


def generate_test_summary(test_results: dict) -> str:
    """Generate markdown summary for test results."""
    total = test_results.get('total', 0)
    passed = test_results.get('passed', 0)
    failed = test_results.get('failed', 0)
    skipped = test_results.get('skipped', 0)
    duration = test_results.get('duration', 0.0)

    # Determine status
    if failed > 0:
        status_emoji = '❌'
        status = 'FAILED'
    elif passed == total and total > 0:
        status_emoji = '✅'
        status = 'PASSED'
    else:
        status_emoji = '⚠️'
        status = 'COMPLETED'

    summary = f"## {status_emoji} Unit Test Results\n\n"
    summary += f"**Status:** {status}\n\n"

    # Summary table
    summary += "| Metric | Value |\n"
    summary += "|--------|-------|\n"
    summary += f"| Total Tests | {total} |\n"
    summary += f"| ✅ Passed | {passed} |\n"
    summary += f"| ❌ Failed | {failed} |\n"

    if skipped > 0:
        summary += f"| ⏭️ Skipped | {skipped} |\n"

    summary += f"| ⏱️ Duration | {duration:.2f}s |\n"

    if total > 0:
        pass_rate = (passed / total) * 100
        summary += f"| 📊 Pass Rate | {pass_rate:.1f}% |\n"

    # Failed tests details
    if failed > 0 and 'failed_tests' in test_results:
        summary += "\n### ❌ Failed Tests\n\n"
        for test_name in test_results['failed_tests'][:10]:
            summary += f"- `{test_name}`\n"

        if len(test_results['failed_tests']) > 10:
            summary += f"\n...and {len(test_results['failed_tests']) - 10} more\n"

    return summary


def generate_coverage_summary(coverage_data: dict) -> str:
    """Generate markdown summary for coverage results."""
    line_coverage = coverage_data.get('line_coverage', 0.0)
    function_coverage = coverage_data.get('function_coverage', 0.0)
    lines_covered = coverage_data.get('lines_covered', 0)
    lines_total = coverage_data.get('lines_total', 0)
    functions_covered = coverage_data.get('functions_covered', 0)
    functions_total = coverage_data.get('functions_total', 0)

    # Determine status
    if line_coverage >= 80:
        status_emoji = '✅'
        status = 'GOOD'
    elif line_coverage >= 60:
        status_emoji = '⚠️'
        status = 'MODERATE'
    else:
        status_emoji = '❌'
        status = 'LOW'

    summary = f"## {status_emoji} Code Coverage\n\n"
    summary += f"**Status:** {status} ({line_coverage:.1f}%)\n\n"

    # Coverage table
    summary += "| Metric | Coverage | Covered/Total |\n"
    summary += "|--------|----------|---------------|\n"
    summary += f"| 📈 Lines | {line_coverage:.1f}% | {lines_covered}/{lines_total} |\n"
    summary += f"| 🔧 Functions | {function_coverage:.1f}% | {functions_covered}/{functions_total} |\n"

    # Visual coverage bar
    def get_coverage_bar(percentage: float, width: int = 20) -> str:
        filled = int(percentage / 100 * width)
        empty = width - filled
        return f"{'█' * filled}{'░' * empty}"

    summary += f"\n### Line Coverage\n\n"
    summary += f"```\n{get_coverage_bar(line_coverage)} {line_coverage:.1f}%\n```\n"

    # Top uncovered files
    if 'files' in coverage_data:
        files = sorted(coverage_data['files'], key=lambda f: f.get('line_coverage', 0))
        uncovered_files = [f for f in files if f.get('line_coverage', 0) < 80][:5]

        if uncovered_files:
            summary += "\n### 📉 Files Needing Attention (< 80% coverage)\n\n"
            summary += "| File | Coverage |\n"
            summary += "|------|----------|\n"
            for file_data in uncovered_files:
                path = file_data['path']
                # Shorten path
                if len(path) > 60:
                    path = '...' + path[-57:]
                cov = file_data.get('line_coverage', 0)
                summary += f"| `{path}` | {cov:.1f}% |\n"

    return summary


def generate_combined_summary(test_results: dict, coverage_data: dict) -> str:
    """Generate combined markdown summary."""
    summary = "# 🧪 Tests & Coverage Report\n\n"

    # Overall status
    test_failed = test_results.get('failed', 0) > 0
    low_coverage = coverage_data.get('line_coverage', 0) < 60

    if test_failed or low_coverage:
        summary += "**Overall Status:** ❌ Issues Detected\n\n"
    elif coverage_data.get('line_coverage', 0) >= 80 and test_results.get('passed', 0) == test_results.get('total', 0):
        summary += "**Overall Status:** ✅ All Good\n\n"
    else:
        summary += "**Overall Status:** ⚠️ Needs Improvement\n\n"

    # Quick stats
    summary += "## Quick Stats\n\n"
    summary += "| Category | Metric | Value |\n"
    summary += "|----------|--------|-------|\n"
    summary += f"| 🧪 Tests | Passed/Total | {test_results.get('passed', 0)}/{test_results.get('total', 0)} |\n"
    summary += f"| 🧪 Tests | Failed | {test_results.get('failed', 0)} |\n"
    summary += f"| 📊 Coverage | Line | {coverage_data.get('line_coverage', 0):.1f}% |\n"
    summary += f"| 📊 Coverage | Function | {coverage_data.get('function_coverage', 0):.1f}% |\n"
    summary += f"| ⏱️ Duration | Test Runtime | {test_results.get('duration', 0):.2f}s |\n"

    summary += "\n---\n\n"

    # Detailed test summary
    summary += generate_test_summary(test_results)

    summary += "\n---\n\n"

    # Detailed coverage summary
    summary += generate_coverage_summary(coverage_data)

    return summary


def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: generate_summary.py <type> [test_json] [coverage_json]")
        print("Types: tests, coverage, combined")
        return 1

    summary_type = sys.argv[1]

    summary = ""

    try:
        if summary_type == 'tests':
            if len(sys.argv) < 3:
                print("❌ Test results JSON file required")
                return 1

            with open(sys.argv[2], 'r') as f:
                test_results = json.load(f)

            summary = generate_test_summary(test_results)

        elif summary_type == 'coverage':
            if len(sys.argv) < 3:
                print("❌ Coverage data JSON file required")
                return 1

            with open(sys.argv[2], 'r') as f:
                coverage_data = json.load(f)

            summary = generate_coverage_summary(coverage_data)

        elif summary_type == 'combined':
            if len(sys.argv) < 4:
                print("❌ Both test results and coverage data JSON files required")
                return 1

            with open(sys.argv[2], 'r') as f:
                test_results = json.load(f)

            with open(sys.argv[3], 'r') as f:
                coverage_data = json.load(f)

            summary = generate_combined_summary(test_results, coverage_data)

        else:
            print(f"❌ Unknown summary type: {summary_type}")
            return 1

        # Write to GitHub Actions summary
        github_step_summary = os.environ.get('GITHUB_STEP_SUMMARY')
        if github_step_summary:
            with open(github_step_summary, 'a') as f:
                f.write(summary)
                f.write('\n')
            print(f"✅ Summary written to GitHub Actions")
        else:
            # Print to stdout for local testing
            print(summary)

        return 0

    except Exception as e:
        print(f"❌ Error generating summary: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())

