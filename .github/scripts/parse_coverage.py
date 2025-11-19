#!/usr/bin/env python3
"""
Parse LCOV coverage data and generate JSON summary.
"""
import os
import sys
import json
import re
from pathlib import Path
from typing import Dict, List, Tuple


def parse_lcov_info(lcov_file: str, include_dirs: List[str] = None) -> Dict:
    """
    Parse LCOV .info file and extract coverage metrics.

    Args:
        lcov_file: Path to LCOV info file
        include_dirs: List of directory patterns to include (e.g., ['client/', 'server/'])

    Returns:
        Dictionary with coverage metrics
    """
    coverage = {
        'line_coverage': 0.0,
        'function_coverage': 0.0,
        'branch_coverage': 0.0,
        'lines_covered': 0,
        'lines_total': 0,
        'functions_covered': 0,
        'functions_total': 0,
        'branches_covered': 0,
        'branches_total': 0,
        'files': []
    }

    if not os.path.exists(lcov_file):
        print(f"⚠️  LCOV file not found: {lcov_file}", file=sys.stderr)
        return coverage

    current_file = None
    file_data = {}

    try:
        with open(lcov_file, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                line = line.strip()

                if line.startswith('SF:'):
                    # Start of new file
                    current_file = line[3:]
                    file_data = {
                        'path': current_file,
                        'lines_covered': 0,
                        'lines_total': 0,
                        'functions_covered': 0,
                        'functions_total': 0,
                        'line_coverage': 0.0
                    }

                elif line.startswith('FNF:'):
                    # Functions found
                    file_data['functions_total'] = int(line[4:])

                elif line.startswith('FNH:'):
                    # Functions hit
                    file_data['functions_covered'] = int(line[4:])

                elif line.startswith('LF:'):
                    # Lines found
                    file_data['lines_total'] = int(line[3:])

                elif line.startswith('LH:'):
                    # Lines hit
                    file_data['lines_covered'] = int(line[3:])

                elif line.startswith('BRF:'):
                    # Branches found
                    if 'branches_total' not in file_data:
                        file_data['branches_total'] = 0
                    file_data['branches_total'] += int(line[4:])

                elif line.startswith('BRH:'):
                    # Branches hit
                    if 'branches_covered' not in file_data:
                        file_data['branches_covered'] = 0
                    file_data['branches_covered'] += int(line[4:])

                elif line == 'end_of_record':
                    # End of file record
                    if current_file and file_data:
                        # Check if file should be included
                        should_include = True
                        if include_dirs:
                            should_include = any(dir_pattern in current_file for dir_pattern in include_dirs)

                        # Exclude vcpkg and system files
                        if 'vcpkg_installed' in current_file or '/usr/' in current_file:
                            should_include = False

                        if should_include:
                            # Calculate file coverage
                            if file_data['lines_total'] > 0:
                                file_data['line_coverage'] = (file_data['lines_covered'] / file_data['lines_total']) * 100

                            coverage['files'].append(file_data)
                            coverage['lines_covered'] += file_data['lines_covered']
                            coverage['lines_total'] += file_data['lines_total']
                            coverage['functions_covered'] += file_data['functions_covered']
                            coverage['functions_total'] += file_data['functions_total']

                            if 'branches_covered' in file_data:
                                coverage['branches_covered'] += file_data.get('branches_covered', 0)
                                coverage['branches_total'] += file_data.get('branches_total', 0)

                    current_file = None
                    file_data = {}

        # Calculate overall coverage percentages
        if coverage['lines_total'] > 0:
            coverage['line_coverage'] = (coverage['lines_covered'] / coverage['lines_total']) * 100

        if coverage['functions_total'] > 0:
            coverage['function_coverage'] = (coverage['functions_covered'] / coverage['functions_total']) * 100

        if coverage['branches_total'] > 0:
            coverage['branch_coverage'] = (coverage['branches_covered'] / coverage['branches_total']) * 100

    except Exception as e:
        print(f"⚠️  Error parsing LCOV file: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()

    return coverage


def parse_gcov_summary(build_dir: str) -> Dict:
    """Parse gcov summary from build directory."""
    # This is a fallback if LCOV is not available
    coverage = {
        'line_coverage': 0.0,
        'function_coverage': 0.0,
        'lines_covered': 0,
        'lines_total': 0,
        'functions_covered': 0,
        'functions_total': 0,
        'files': []
    }

    # Look for .gcov files
    gcov_files = list(Path(build_dir).glob('**/*.gcov'))

    for gcov_file in gcov_files:
        try:
            with open(gcov_file, 'r') as f:
                content = f.read()

                # Parse basic metrics from gcov output
                # This is a simplified parser
                lines = content.split('\n')
                lines_covered = 0
                lines_total = 0

                for line in lines:
                    if line.strip() and not line.startswith('//'):
                        parts = line.split(':', 2)
                        if len(parts) >= 2:
                            exec_count = parts[0].strip()
                            if exec_count.isdigit():
                                lines_total += 1
                                if int(exec_count) > 0:
                                    lines_covered += 1
                            elif exec_count != '-' and exec_count != '#####':
                                lines_total += 1

                if lines_total > 0:
                    file_coverage = (lines_covered / lines_total) * 100
                    coverage['files'].append({
                        'path': str(gcov_file),
                        'lines_covered': lines_covered,
                        'lines_total': lines_total,
                        'line_coverage': file_coverage
                    })
                    coverage['lines_covered'] += lines_covered
                    coverage['lines_total'] += lines_total

        except Exception as e:
            print(f"⚠️  Error parsing {gcov_file}: {e}", file=sys.stderr)

    if coverage['lines_total'] > 0:
        coverage['line_coverage'] = (coverage['lines_covered'] / coverage['lines_total']) * 100

    return coverage


def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: parse_coverage.py <lcov_file|build_dir> [output_json] [--include client,server]")
        return 1

    input_path = sys.argv[1]
    output_file = 'coverage.json'
    include_dirs = ['client/', 'server/']

    # Parse additional arguments
    i = 2
    while i < len(sys.argv):
        if sys.argv[i] == '--include' and i + 1 < len(sys.argv):
            include_dirs = [d.strip() + '/' for d in sys.argv[i + 1].split(',')]
            i += 2
        else:
            output_file = sys.argv[i]
            i += 1

    print(f"📊 Parsing coverage data from {input_path}...")
    print(f"   Including directories: {', '.join(include_dirs)}")

    # Determine if input is LCOV file or build directory
    if os.path.isfile(input_path) and input_path.endswith('.info'):
        coverage = parse_lcov_info(input_path, include_dirs)
    elif os.path.isdir(input_path):
        # Look for coverage.info or lcov.info
        lcov_files = list(Path(input_path).glob('**/coverage.info'))
        lcov_files.extend(Path(input_path).glob('**/lcov.info'))

        if lcov_files:
            print(f"   Found LCOV file: {lcov_files[0]}")
            coverage = parse_lcov_info(str(lcov_files[0]), include_dirs)
        else:
            print("   No LCOV file found, trying gcov...")
            coverage = parse_gcov_summary(input_path)
    else:
        print(f"❌ Invalid input: {input_path}", file=sys.stderr)
        return 1

    # Ensure output directory exists
    output_path = Path(output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    # Write results
    with open(output_file, 'w') as f:
        json.dump(coverage, f, indent=2)

    print(f"✅ Coverage data written to {output_file}")
    print(f"   Line Coverage: {coverage['line_coverage']:.1f}%")
    print(f"   Function Coverage: {coverage['function_coverage']:.1f}%")
    print(f"   Lines: {coverage['lines_covered']}/{coverage['lines_total']}")
    print(f"   Functions: {coverage['functions_covered']}/{coverage['functions_total']}")
    print(f"   Files analyzed: {len(coverage['files'])}")

    return 0


if __name__ == '__main__':
    sys.exit(main())

