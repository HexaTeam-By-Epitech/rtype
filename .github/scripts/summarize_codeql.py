#!/usr/bin/env python3
"""
CodeQL SARIF Summary Generator
Extracts and normalizes findings from CodeQL SARIF files.
"""
import json
import glob
import os
import sys
from pathlib import Path
from typing import List, Dict, Tuple, Optional

# Configuration
OUTPUT_DIR = 'artifacts/codeql'
SKIP_PATTERNS = ['vcpkg_installed', '/vcpkg_installed/', '/vcpkg/']
SEVERITY_ORDER = {'error': 0, 'warning': 1, 'note': 2, 'information': 3, 'recommendation': 4, '': 5}


def normalize_severity(value) -> str:
    """
    Normalize various severity representations to standardized levels.

    Returns one of: 'error', 'warning', 'note', or 'information'
    """
    if not value:
        return 'note'

    val = str(value).strip().lower()

    # Handle numeric priorities
    if val.isdigit():
        priority = int(val)
        if priority <= 1:
            return 'error'
        elif priority == 2:
            return 'warning'
        return 'note'

    # Map common severity synonyms
    severity_map = {
        'critical': 'error',
        'high': 'error',
        'error': 'error',
        'failure': 'error',
        'warning': 'warning',
        'warn': 'warning',
        'medium': 'warning',
        'note': 'note',
        'info': 'note',
        'informational': 'note',
        'information': 'information',
        'low': 'note',
        'recommendation': 'note',
    }

    return severity_map.get(val, 'note')


def extract_severity_from_result(result: Dict, rules_meta: Dict, rules_meta_by_index: Dict) -> str:
    """
    Extract severity from a SARIF result using multiple fallback strategies.
    """
    # Strategy 1: Direct fields
    level = result.get('level') or result.get('severity') or result.get('kind') or ''

    # Strategy 2: Properties object
    if not level:
        props = result.get('properties') or {}
        level = (props.get('severity') or props.get('priority') or
                props.get('level') or props.get('severityLabel') or '')

    # Strategy 3: Rule object attached to result
    if not level:
        rule_obj = result.get('rule') or result.get('ruleObject') or {}
        if isinstance(rule_obj, dict):
            default_config = rule_obj.get('defaultConfiguration') or {}
            level = default_config.get('level', '')
            if not level:
                rule_props = rule_obj.get('properties') or {}
                level = rule_props.get('severity', '')

    # Strategy 4: Lookup by ruleIndex
    if not level and 'ruleIndex' in result:
        try:
            idx = int(result['ruleIndex'])
            rule_meta = rules_meta_by_index.get(idx) or rules_meta_by_index.get((0, idx))
            if rule_meta:
                level = extract_severity_from_rule_meta(rule_meta)
        except (ValueError, TypeError):
            pass

    # Strategy 5: Lookup by ruleId
    if not level:
        rule_id = result.get('ruleId', '')
        if rule_id in rules_meta:
            level = extract_severity_from_rule_meta(rules_meta[rule_id])

    return level


def extract_severity_from_rule_meta(rule_meta: Dict) -> str:
    """Extract severity from rule metadata."""
    if not rule_meta:
        return ''

    default_config = rule_meta.get('defaultConfiguration') or {}
    level = default_config.get('level', '')

    if not level:
        props = rule_meta.get('properties') or {}
        level = props.get('severity', '')

    return level


def build_rules_metadata(run: Dict) -> Tuple[Dict, Dict]:
    """
    Build lookup tables for rule metadata from SARIF run.

    Returns:
        (rules_meta, rules_meta_by_index) - Two dicts for different lookup strategies
    """
    rules_meta = {}
    rules_meta_by_index = {}

    try:
        tool = run.get('tool', {})
        driver = tool.get('driver', {})
        rules_list = driver.get('rules') or []

        # Index driver rules
        for idx, rule in enumerate(rules_list):
            rule_id = rule.get('id') or rule.get('name')
            if rule_id:
                rules_meta[rule_id] = rule
            rules_meta_by_index[(0, idx)] = rule
            rules_meta_by_index[idx] = rule

        # Index extension rules
        extensions = tool.get('extensions') or []
        for comp_index, ext in enumerate(extensions, start=1):
            ext_driver = ext.get('driver') or {}
            ext_rules = ext_driver.get('rules') or []
            for idx, rule in enumerate(ext_rules):
                rule_id = rule.get('id') or rule.get('name')
                if rule_id and rule_id not in rules_meta:
                    rules_meta[rule_id] = rule
                rules_meta_by_index[(comp_index, idx)] = rule

    except Exception as e:
        print(f"Warning: Error building rules metadata: {e}", file=sys.stderr)

    return rules_meta, rules_meta_by_index


def extract_location(result: Dict) -> str:
    """Extract file location from SARIF result."""
    locations = result.get('locations', [])
    if not locations:
        return '?'

    try:
        phys_loc = locations[0]['physicalLocation']
        uri = phys_loc.get('artifactLocation', {}).get('uri', '')
        start_line = phys_loc.get('region', {}).get('startLine', '?')
        return f"{uri}:{start_line}"
    except (KeyError, IndexError):
        return '?'


def should_skip_location(location: str) -> bool:
    """Check if location should be skipped (e.g., third-party code)."""
    return any(pattern in location for pattern in SKIP_PATTERNS)


def extract_message(result: Dict) -> str:
    """Extract and clean message from SARIF result."""
    msg_obj = result.get('message', {})
    msg = msg_obj.get('text', '') or msg_obj.get('markdown', '')
    return msg.strip().splitlines()[0] if msg else ''


def process_sarif_file(filepath: Path) -> List[Tuple[str, str, str, str]]:
    """
    Process a single SARIF file and extract findings.

    Returns:
        List of (severity, rule_id, location, message) tuples
    """
    findings = []

    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            sarif_data = json.load(f)
    except Exception as e:
        print(f"Error: Failed to parse {filepath}: {e}", file=sys.stderr)
        return findings

    runs = sarif_data.get('runs', [])
    for run in runs:
        rules_meta, rules_meta_by_index = build_rules_metadata(run)
        results = run.get('results', [])

        for result in results:
            rule_id = result.get('ruleId', 'N/A')

            # Extract severity
            raw_severity = extract_severity_from_result(result, rules_meta, rules_meta_by_index)
            severity = normalize_severity(raw_severity)

            # Extract location
            location = extract_location(result)

            # Skip third-party code
            if should_skip_location(location):
                continue

            # Extract message
            message = extract_message(result)

            findings.append((severity, rule_id, location, message))

    return findings


def main():
    """Main entry point."""
    # Create output directory
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Find SARIF files
    sarif_files = glob.glob(os.path.join(OUTPUT_DIR, '*.sarif'))

    if not sarif_files:
        print(f"No SARIF files found in {OUTPUT_DIR}")
        sys.exit(0)

    print(f"Processing {len(sarif_files)} SARIF file(s)...")

    # Process all SARIF files
    all_findings = []
    for filepath in sarif_files:
        findings = process_sarif_file(Path(filepath))
        all_findings.extend(findings)
        print(f"  {Path(filepath).name}: {len(findings)} finding(s)")

    if not all_findings:
        print("No results found in SARIF files")
        sys.exit(0)

    # Sort findings by severity
    all_findings.sort(key=lambda x: (SEVERITY_ORDER.get(x[0].lower(), 9), x[1]))

    # Write summary
    output_path = os.path.join(OUTPUT_DIR, 'codeql-summary.txt')
    with open(output_path, 'w', encoding='utf-8') as out:
        out.write('severity\trule\tlocation\tmessage\n')
        for severity, rule_id, location, message in all_findings:
            line = f"{severity}\t{rule_id}\t{location}\t{message}"
            print(line)
            out.write(line + '\n')

    # Print statistics
    severity_counts = {}
    for severity, _, _, _ in all_findings:
        severity_counts[severity] = severity_counts.get(severity, 0) + 1

    print(f"\n{'='*60}")
    print("Summary Statistics:")
    for sev in ['error', 'warning', 'note', 'information']:
        count = severity_counts.get(sev, 0)
        if count > 0:
            print(f"  {sev.capitalize()}: {count}")
    print(f"  Total: {len(all_findings)}")
    print(f"{'='*60}")
    print(f"\nWrote summary to {output_path}")


if __name__ == '__main__':
    main()