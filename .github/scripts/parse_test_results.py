#!/usr/bin/env python3
"""
Parse CTest/GTest results and generate JSON summary.
"""
import os
import sys
import json
import re
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Dict, List, Optional


def parse_ctest_output(build_dir: str) -> Dict:
    """Parse CTest output from Testing directory."""
    results = {
        'total': 0,
        'passed': 0,
        'failed': 0,
        'skipped': 0,
        'duration': 0.0,
        'failed_tests': []
    }

    testing_dir = Path(build_dir) / 'Testing'
    print(f"🔍 Looking for test results in: {testing_dir}")

    if not testing_dir.exists():
        print(f"⚠️  Testing directory does not exist: {testing_dir}")
        return results

    # Find the most recent Test.xml file
    test_xml_files = sorted(list(testing_dir.glob('*/Test.xml')), reverse=True)
    print(f"📁 Found {len(test_xml_files)} Test.xml files")

    if test_xml_files:
        xml_file = test_xml_files[0]
        print(f"📄 Parsing: {xml_file}")
        try:
            tree = ET.parse(xml_file)
            root = tree.getroot()

            # CTest XML format
            testing = root.find('Testing')
            if testing:
                for test in testing.findall('.//Test'):
                    status = test.get('Status')
                    results['total'] += 1

                    name_elem = test.find('.//Name')
                    test_name = name_elem.text if name_elem is not None else 'Unknown'

                    if status == 'passed':
                        results['passed'] += 1
                    elif status == 'failed':
                        results['failed'] += 1
                        results['failed_tests'].append(test_name)
                    else:
                        results['skipped'] += 1

                    # Get duration
                    time_elem = test.find('.//Results/NamedMeasurement[@name="Execution Time"]/Value')
                    if time_elem is not None and time_elem.text:
                        try:
                            results['duration'] += float(time_elem.text)
                        except ValueError:
                            pass

                print(f"✅ Parsed {results['total']} tests from XML")
                return results

        except Exception as e:
            print(f"⚠️  Error parsing {xml_file}: {e}", file=sys.stderr)

    # Fallback: parse LastTest.log
    print("📋 Trying LastTest.log fallback...")
    last_test_log = testing_dir / 'Temporary' / 'LastTest.log'
    if last_test_log.exists():
        try:
            with open(last_test_log, 'r') as f:
                content = f.read()

                # FIX: Use finditer instead of search to get ALL matches
                passed_matches = re.finditer(r'(\d+)/(\d+) Test\s+#\d+:\s+(\S+).*\s+Passed', content, re.MULTILINE)
                for match in passed_matches:
                    test_name = match.group(3)
                    results['passed'] += 1
                    results['total'] += 1
                    print(f"  ✅ Found passed test: {test_name}")

                failed_matches = re.finditer(r'(\d+)/(\d+) Test\s+#\d+:\s+(\S+).*\*\*\*Failed', content, re.MULTILINE)
                for match in failed_matches:
                    test_name = match.group(3)
                    results['failed'] += 1
                    results['total'] += 1
                    results['failed_tests'].append(test_name)
                    print(f"  ❌ Found failed test: {test_name}")

                print(f"✅ Parsed {results['total']} tests from LastTest.log")

        except Exception as e:
            print(f"⚠️  Error parsing LastTest.log: {e}", file=sys.stderr)
    else:
        print(f"⚠️  LastTest.log not found: {last_test_log}")

    return results


def parse_gtest_xml(xml_path: str) -> Dict:
    """Parse GTest XML output."""
    results = {
        'total': 0,
        'passed': 0,
        'failed': 0,
        'skipped': 0,
        'duration': 0.0,
        'failed_tests': []
    }

    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()

        # GTest XML format
        for testsuite in root.findall('.//testsuite'):
            tests = int(testsuite.get('tests', 0))
            failures = int(testsuite.get('failures', 0))
            errors = int(testsuite.get('errors', 0))
            skipped = int(testsuite.get('skipped', 0))
            time = float(testsuite.get('time', 0))

            results['total'] += tests
            results['failed'] += failures + errors
            results['passed'] += tests - failures - errors - skipped
            results['skipped'] += skipped
            results['duration'] += time

            # Get failed test names
            for testcase in testsuite.findall('.//testcase'):
                if testcase.find('failure') is not None or testcase.find('error') is not None:
                    test_name = f"{testsuite.get('name')}.{testcase.get('name')}"
                    results['failed_tests'].append(test_name)

        print(f"✅ Parsed {results['total']} tests from GTest XML")

    except Exception as e:
        print(f"⚠️  Error parsing GTest XML: {e}", file=sys.stderr)

    return results


def merge_results(results_list: List[Dict]) -> Dict:
    """Merge multiple test result dictionaries."""
    merged = {
        'total': 0,
        'passed': 0,
        'failed': 0,
        'skipped': 0,
        'duration': 0.0,
        'failed_tests': []
    }

    for result in results_list:
        merged['total'] += result['total']
        merged['passed'] += result['passed']
        merged['failed'] += result['failed']
        merged['skipped'] += result['skipped']
        merged['duration'] += result['duration']
        merged['failed_tests'].extend(result['failed_tests'])

    return merged


def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: parse_test_results.py <build_dir> [output_json]")
        return 1

    build_dir = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else 'test-results.json'

    if not os.path.exists(build_dir):
        print(f"❌ Build directory not found: {build_dir}", file=sys.stderr)
        return 1

    # Try to parse CTest results
    print("📊 Parsing test results...")
    results = parse_ctest_output(build_dir)

    # Also look for GTest XML files
    print("🔍 Looking for GTest XML files...")
    gtest_xmls = list(Path(build_dir).glob('**/test_detail.xml'))
    gtest_xmls.extend(Path(build_dir).glob('**/*_results.xml'))
    gtest_xmls.extend(Path(build_dir).glob('**/gtest_*.xml'))

    print(f"📁 Found {len(gtest_xmls)} GTest XML files")

    gtest_results = []
    for xml_file in gtest_xmls:
        print(f"  📄 Parsing: {xml_file}")
        gtest_results.append(parse_gtest_xml(str(xml_file)))

    if gtest_results:
        # Merge GTest results with CTest results
        all_results = [results] + gtest_results
        results = merge_results(all_results)
        print(f"🔄 Merged results from multiple sources")

    # Ensure output directory exists
    output_path = Path(output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    # Write results
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2)

    print(f"\n{'='*60}")
    print(f"✅ Test results written to {output_file}")
    print(f"{'='*60}")
    print(f"   Total:    {results['total']}")
    print(f"   Passed:   {results['passed']}")
    print(f"   Failed:   {results['failed']}")
    print(f"   Skipped:  {results['skipped']}")
    print(f"   Duration: {results['duration']:.2f}s")
    print(f"{'='*60}")

    if results['total'] == 0:
        print("\n⚠️  WARNING: No tests were found!")
        print("   Possible causes:")
        print("   - Tests didn't run successfully")
        print("   - CTest output files are missing")
        print("   - Test XML files are in unexpected locations")

    return 0


if __name__ == '__main__':
    sys.exit(main())