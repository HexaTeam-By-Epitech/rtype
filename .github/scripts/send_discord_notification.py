#!/usr/bin/env python3
"""
Discord Webhook Notification for CodeQL Analysis
Sends a rich embed with analysis results to Discord.
"""
import os
import sys
import json
import urllib.request
import urllib.error
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from datetime import datetime, timezone

SUMMARY_PATH = 'artifacts/codeql/codeql-summary.txt'
DISCORD_WEBHOOK_ENV = 'DISCORD_WEBHOOK'
MAX_EMBED_FIELDS = 10
MAX_FIELD_VALUE_LENGTH = 1024


def load_github_context() -> Dict:
    """Load GitHub Actions context from environment."""
    context_json = os.environ.get('GITHUB_CONTEXT', '{}')
    try:
        return json.loads(context_json)
    except json.JSONDecodeError:
        return {}


def load_findings() -> List[Tuple[str, str, str, str]]:
    """Load findings from summary file."""
    if not os.path.exists(SUMMARY_PATH):
        return []

    findings = []
    with open(SUMMARY_PATH, 'r', encoding='utf-8') as f:
        next(f, None)  # Skip header
        for line in f:
            parts = line.strip().split('\t', 3)
            while len(parts) < 4:
                parts.append('')
            findings.append(tuple(parts[:4]))

    return findings


def count_by_severity(findings: List[Tuple[str, str, str, str]]) -> Dict[str, int]:
    """Count findings by severity level."""
    counts = {}
    for severity, _, _, _ in findings:
        sev_lower = severity.lower()
        counts[sev_lower] = counts.get(sev_lower, 0) + 1
    return counts


def get_severity_color(severity_counts: Dict[str, int]) -> int:
    """
    Determine embed color based on severity.

    Returns:
        Discord color as integer (0xRRGGBB)
    """
    if severity_counts.get('error', 0) > 0:
        return 0xED4245  # Red
    elif severity_counts.get('warning', 0) > 0:
        return 0xFEE75C  # Yellow
    elif severity_counts.get('note', 0) > 0:
        return 0x5865F2  # Blurple
    else:
        return 0x57F287  # Green


def get_status_emoji(severity_counts: Dict[str, int]) -> str:
    """Get status emoji based on findings."""
    if severity_counts.get('error', 0) > 0:
        return '🔴'
    elif severity_counts.get('warning', 0) > 0:
        return '🟡'
    elif severity_counts.get('note', 0) > 0:
        return '🔵'
    else:
        return '✅'


def create_summary_field(severity_counts: Dict[str, int], total: int) -> Dict:
    """Create summary field for embed."""
    if not severity_counts:
        return {
            "name": "📊 Summary",
            "value": "✅ No findings detected",
            "inline": False
        }

    lines = []
    if severity_counts.get('error', 0) > 0:
        lines.append(f"🔴 Errors: **{severity_counts['error']}**")
    if severity_counts.get('warning', 0) > 0:
        lines.append(f"🟡 Warnings: **{severity_counts['warning']}**")
    if severity_counts.get('note', 0) > 0:
        lines.append(f"🔵 Notes: **{severity_counts['note']}**")
    if severity_counts.get('information', 0) > 0:
        lines.append(f"ℹ️ Info: **{severity_counts['information']}**")

    lines.append(f"\n📈 **Total:** {total}")

    return {
        "name": "📊 Analysis Summary",
        "value": "\n".join(lines),
        "inline": False
    }


def create_top_findings_field(findings: List[Tuple[str, str, str, str]]) -> Optional[Dict]:
    """Create field with top findings."""
    if not findings:
        return None

    top_findings = findings[:5]
    lines = []

    for severity, rule, location, message in top_findings:
        emoji = {
            'error': '🔴',
            'warning': '🟡',
            'note': '🔵',
            'information': 'ℹ️'
        }.get(severity.lower(), '⚪')

        # Truncate message if too long
        display_msg = message[:60] + '...' if len(message) > 60 else message

        # Truncate location
        display_loc = location.split('/')[-1] if '/' in location else location
        if len(display_loc) > 40:
            display_loc = '...' + display_loc[-37:]

        lines.append(f"{emoji} `{rule}`")
        lines.append(f"└ {display_loc}")
        if display_msg:
            lines.append(f"   _{display_msg}_")
        lines.append("")  # Empty line for spacing

    value = "\n".join(lines)

    # Ensure we don't exceed Discord's field value limit
    if len(value) > MAX_FIELD_VALUE_LENGTH:
        value = value[:MAX_FIELD_VALUE_LENGTH-3] + "..."

    return {
        "name": "🔍 Top Findings",
        "value": value or "No details available",
        "inline": False
    }


def create_embed(findings: List[Tuple[str, str, str, str]], context: Dict) -> Dict:
    """Create Discord embed payload."""
    severity_counts = count_by_severity(findings)
    total = len(findings)

    # Extract GitHub context
    repo = context.get('repository', 'unknown/repo')
    ref = context.get('ref', 'unknown').replace('refs/heads/', '')
    sha = context.get('sha', 'unknown')[:7]
    actor = context.get('actor', 'unknown')
    run_id = context.get('run_id', '')
    server_url = os.environ.get('GITHUB_SERVER_URL', 'https://github.com')

    # Build workflow URL
    workflow_url = f"{server_url}/{repo}/actions/runs/{run_id}" if run_id else None

    # Determine status
    status_emoji = get_status_emoji(severity_counts)
    status_text = "Analysis Complete"
    if severity_counts.get('error', 0) > 0:
        status_text = "Issues Found"
    elif severity_counts.get('warning', 0) > 0:
        status_text = "Warnings Detected"

    # Build embed
    embed = {
        "title": f"{status_emoji} CodeQL Static Analysis",
        "description": f"**{status_text}** for `{repo}`",
        "color": get_severity_color(severity_counts),
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "fields": [],
        "footer": {
            "text": f"Branch: {ref} • Commit: {sha} • By: {actor}"
        }
    }

    # Add workflow URL if available
    if workflow_url:
        embed["url"] = workflow_url

    # Add summary field
    summary_field = create_summary_field(severity_counts, total)
    embed["fields"].append(summary_field)

    # Add top findings if any
    if findings:
        top_findings_field = create_top_findings_field(findings)
        if top_findings_field:
            embed["fields"].append(top_findings_field)

    # Add action link
    if workflow_url:
        embed["fields"].append({
            "name": "🔗 Actions",
            "value": f"[View Full Report]({workflow_url})",
            "inline": False
        })

    return embed


def send_to_discord(webhook_url: str, embed: Dict) -> bool:
    """
    Send embed to Discord webhook.

    Returns:
        True if successful, False otherwise
    """
    payload = {
        "embeds": [embed],
        "username": "CodeQL Bot",
        "avatar_url": "https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png"
    }

    data = json.dumps(payload).encode('utf-8')

    try:
        # Using a more common User-Agent and Accept header helps avoid Cloudflare blocks
        # which can return HTTP 403 with error code 1010 when the default python-urllib user agent
        # is used from some environments.
        headers = {
            'Content-Type': 'application/json',
            'Accept': 'application/json',
            'User-Agent': 'Mozilla/5.0 (compatible; GitHub-Actions/1.0)'
        }

        req = urllib.request.Request(
            webhook_url,
            data=data,
            headers=headers,
            method='POST'
        )

        with urllib.request.urlopen(req, timeout=10) as response:
            if response.status == 204:
                return True
            else:
                print(f"Unexpected response status: {response.status}", file=sys.stderr)
                return False

    except urllib.error.HTTPError as e:
        print(f"HTTP Error {e.code}: {e.reason}", file=sys.stderr)
        try:
            error_body = e.read().decode('utf-8')
            print(f"Response: {error_body}", file=sys.stderr)
        except:
            pass
        return False

    except urllib.error.URLError as e:
        print(f"URL Error: {e.reason}", file=sys.stderr)
        return False

    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return False


def main():
    """Main entry point."""
    # Check for webhook URL
    webhook_url = os.environ.get(DISCORD_WEBHOOK_ENV)

    if not webhook_url:
        print(f"⚠️  {DISCORD_WEBHOOK_ENV} not set, skipping Discord notification")
        sys.exit(0)

    # Accept both discord.com and legacy discordapp.com webhook domains
    if not webhook_url.startswith(('https://discord.com/api/webhooks/', 'https://discordapp.com/api/webhooks/')):
        print(f"⚠️  Invalid Discord webhook URL format", file=sys.stderr)
        sys.exit(0)

    print("📤 Preparing Discord notification...")

    # Load data
    findings = load_findings()
    context = load_github_context()

    print(f"   Found {len(findings)} finding(s)")

    # Create embed
    embed = create_embed(findings, context)

    # Send to Discord
    print("   Sending to Discord...")
    success = send_to_discord(webhook_url, embed)

    if success:
        print("✅ Discord notification sent successfully")
    else:
        print("❌ Failed to send Discord notification", file=sys.stderr)
        # Don't fail the job if Discord notification fails
        sys.exit(0)


if __name__ == '__main__':
    main()