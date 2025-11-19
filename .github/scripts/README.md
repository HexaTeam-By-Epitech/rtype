# GitHub Actions Scripts

This directory contains Python scripts used by GitHub Actions workflows for CI/CD automation.

## Scripts Overview

### Core Scripts

#### `discord_notifier.py` 🔔
**Generic Discord webhook notifier for all CI/CD events.**

**Usage:**
```bash
# CodeQL notifications
./discord_notifier.py codeql [codeql-summary.txt]

# Test results notifications
./discord_notifier.py tests <test-results.json>

# Coverage notifications
./discord_notifier.py coverage <coverage.json>

# Combined tests + coverage
./discord_notifier.py combined <combined.json>
```

**Environment Variables:**
- `DISCORD_WEBHOOK`: Discord webhook URL (required)
- `GITHUB_CONTEXT`: GitHub Actions context JSON (optional, auto-loaded)

**Features:**
- Rich embeds with color-coded status
- Automatic GitHub context extraction (repo, branch, commit, actor)
- Links to workflow runs
- Customizable bot name and avatar

---

#### `parse_test_results.py` 🧪
**Parse CTest/GTest results and generate JSON summary.**

**Usage:**
```bash
./parse_test_results.py <build_dir> [output.json]
```

**Output Format:**
```json
{
  "total": 10,
  "passed": 8,
  "failed": 2,
  "skipped": 0,
  "duration": 12.5,
  "failed_tests": ["TestCase.FailedTest1", "TestCase.FailedTest2"]
}
```

**Features:**
- Parses CTest XML output
- Supports GTest XML reports
- Merges multiple test suite results
- Extracts failed test names

---

#### `parse_coverage.py` 📊
**Parse LCOV coverage data and generate JSON summary.**

**Usage:**
```bash
# From LCOV file
./parse_coverage.py <coverage.info> [output.json] [--include client,server]

# From build directory
./parse_coverage.py <build_dir> [output.json] [--include client,server]
```

**Output Format:**
```json
{
  "line_coverage": 75.5,
  "function_coverage": 80.2,
  "branch_coverage": 65.3,
  "lines_covered": 1500,
  "lines_total": 2000,
  "functions_covered": 120,
  "functions_total": 150,
  "files": [
    {
      "path": "client/src/Client.cpp",
      "lines_covered": 100,
      "lines_total": 150,
      "line_coverage": 66.7
    }
  ]
}
```

**Features:**
- Parses LCOV info files
- Filters by directory (include only client/server)
- Excludes vcpkg and system files
- Fallback to gcov parsing
- Per-file coverage metrics

---

#### `generate_summary.py` 📝
**Generate GitHub Actions summary markdown for tests and coverage.**

**Usage:**
```bash
# Test summary only
./generate_summary.py tests <test-results.json>

# Coverage summary only
./generate_summary.py coverage <coverage.json>

# Combined summary
./generate_summary.py combined <test-results.json> <coverage.json>
```

**Features:**
- Writes to `$GITHUB_STEP_SUMMARY`
- Color-coded status (✅ ⚠️ ❌)
- Visual coverage bars
- Top failed tests
- Files needing attention (< 80% coverage)
- Overall pass rates

---

### CodeQL Scripts (Legacy)

#### `summarize_codeql.py`
Parse CodeQL SARIF files and generate text summary.

#### `publish_codeql_summary.py`
Write CodeQL summary to GitHub Actions summary.

#### `fail_on_codeql.py`
Exit with error code if CodeQL findings exceed threshold.

#### `send_discord_notification.py` (Deprecated)
**⚠️ Deprecated:** Use `discord_notifier.py` instead.

---

## Workflow Integration

### Build and Tests Workflow

```yaml
- name: Parse test results
  run: |
    chmod +x .github/scripts/parse_test_results.py
    .github/scripts/parse_test_results.py build artifacts/tests/test-results.json

- name: Generate coverage data
  run: |
    lcov --capture --directory build --output-file coverage.info
    lcov --extract coverage.info '*/client/*' '*/server/*' --output-file coverage-filtered.info

- name: Parse coverage data
  run: |
    chmod +x .github/scripts/parse_coverage.py
    .github/scripts/parse_coverage.py coverage-filtered.info artifacts/tests/coverage.json --include client,server

- name: Generate GitHub summary
  run: |
    chmod +x .github/scripts/generate_summary.py
    .github/scripts/generate_summary.py combined \
      artifacts/tests/test-results.json \
      artifacts/tests/coverage.json

- name: Send Discord notification
  env:
    DISCORD_WEBHOOK: ${{ secrets.DISCORD_WEBHOOK }}
    GITHUB_CONTEXT: ${{ toJson(github) }}
  run: |
    # Create combined JSON
    jq -n \
      --slurpfile tests artifacts/tests/test-results.json \
      --slurpfile coverage artifacts/tests/coverage.json \
      '{tests: $tests[0], coverage: $coverage[0]}' \
      > combined.json
    
    # Send notification
    chmod +x .github/scripts/discord_notifier.py
    .github/scripts/discord_notifier.py combined combined.json
```

### Code Analysis Workflow

```yaml
- name: Send Discord notification
  env:
    DISCORD_WEBHOOK: ${{ secrets.DISCORD_WEBHOOK }}
    GITHUB_CONTEXT: ${{ toJson(github) }}
  run: |
    chmod +x .github/scripts/discord_notifier.py
    .github/scripts/discord_notifier.py codeql artifacts/codeql/codeql-summary.txt
```

---

## Configuration

### Required Secrets

Add these secrets to your GitHub repository:

- `DISCORD_WEBHOOK`: Discord webhook URL for notifications

### Directory Structure

Expected artifacts structure:
```
artifacts/
├── tests/
│   ├── test-results.json
│   ├── coverage.json
│   ├── coverage.info
│   ├── combined.json
│   └── coverage-html/
└── codeql/
    ├── codeql-summary.txt
    └── results.sarif
```

---

## Local Testing

### Test Summary Generation
```bash
# Create sample test results
cat > test-results.json << EOF
{
  "total": 10,
  "passed": 8,
  "failed": 2,
  "skipped": 0,
  "duration": 5.5,
  "failed_tests": ["Test1", "Test2"]
}
EOF

# Generate summary
./generate_summary.py tests test-results.json
```

### Test Discord Notification
```bash
export DISCORD_WEBHOOK="https://discord.com/api/webhooks/..."
export GITHUB_CONTEXT='{"repository":"owner/repo","ref":"refs/heads/main"}'

./discord_notifier.py tests test-results.json
```

---

## Dependencies

All scripts use Python 3 standard library only:
- `os`, `sys`, `json`
- `urllib.request`, `urllib.error`
- `pathlib`, `xml.etree.ElementTree`
- `re`, `datetime`, `typing`

No external packages required! ✅

---

## Error Handling

All scripts:
- Exit with code `0` on success
- Exit with code `1` on error
- Print errors to `stderr`
- Gracefully handle missing files
- Support `--help` flag (TODO)

---

## Contributing

When modifying scripts:
1. Maintain backward compatibility
2. Add type hints
3. Write docstrings
4. Test locally before committing
5. Update this README

---

## License

Same as parent project (R-Type).

