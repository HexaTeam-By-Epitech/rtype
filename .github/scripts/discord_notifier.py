#!/usr/bin/env python3
"""
Generic Discord Webhook Notifier
Sends rich embeds to Discord for various CI/CD events.
"""
import os
import sys
import json
import urllib.request
import urllib.error
from typing import Dict, List, Optional
from datetime import datetime, timezone


class DiscordNotifier:
    """Generic Discord notification handler."""

    def __init__(self, webhook_url: str):
        """Initialize notifier with webhook URL."""
        self.webhook_url = webhook_url
        self.context = self._load_github_context()

    @staticmethod
    def _load_github_context() -> Dict:
        """Load GitHub Actions context from environment."""
        context_json = os.environ.get('GITHUB_CONTEXT', '{}')
        try:
            return json.loads(context_json)
        except json.JSONDecodeError:
            return {}

    def _get_base_embed(self, title: str, description: str, color: int) -> Dict:
        """Create base embed with common fields."""
        repo = self.context.get('repository', 'unknown/repo')
        ref = self.context.get('ref', 'unknown').replace('refs/heads/', '')
        sha = self.context.get('sha', 'unknown')[:7]
        actor = self.context.get('actor', 'unknown')
        run_id = self.context.get('run_id', '')
        server_url = os.environ.get('GITHUB_SERVER_URL', 'https://github.com')

        workflow_url = f"{server_url}/{repo}/actions/runs/{run_id}" if run_id else None

        embed = {
            "title": title,
            "description": description,
            "color": color,
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "fields": [],
            "footer": {
                "text": f"Branch: {ref} • Commit: {sha} • By: {actor}"
            }
        }

        if workflow_url:
            embed["url"] = workflow_url

        return embed

    def send(self, embeds: List[Dict], username: str = "CI/CD Bot",
             avatar_url: str = "https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png") -> bool:
        """
        Send embeds to Discord webhook.

        Args:
            embeds: List of embed dictionaries
            username: Bot username
            avatar_url: Bot avatar URL

        Returns:
            True if successful, False otherwise
        """
        payload = {
            "embeds": embeds,
            "username": username,
            "avatar_url": avatar_url
        }

        data = json.dumps(payload).encode('utf-8')

        try:
            headers = {
                'Content-Type': 'application/json',
                'Accept': 'application/json',
                'User-Agent': 'Mozilla/5.0 (compatible; GitHub-Actions/1.0)'
            }

            req = urllib.request.Request(
                self.webhook_url,
                data=data,
                headers=headers,
                method='POST'
            )

            with urllib.request.urlopen(req, timeout=10) as response:
                return response.status == 204

        except urllib.error.HTTPError as e:
            print(f"❌ HTTP error {e.code}: {e.reason}", file=sys.stderr)
            if e.code == 403:
                print("⚠️  Cloudflare or Discord blocking request", file=sys.stderr)
            return False

        except urllib.error.URLError as e:
            print(f"❌ URL error: {e.reason}", file=sys.stderr)
            return False

        except Exception as e:
            print(f"❌ Unexpected error: {e}", file=sys.stderr)
            return False

    def create_test_embed(self, test_results: Dict) -> Dict:
        """Create embed for unit test results."""
        total = test_results.get('total', 0)
        passed = test_results.get('passed', 0)
        failed = test_results.get('failed', 0)
        skipped = test_results.get('skipped', 0)
        duration = test_results.get('duration', 0.0)

        # Determine color and status
        if failed > 0:
            color = 0xED4245  # Red
            status_emoji = '❌'
            status_text = "Tests Failed"
        elif passed == total and total > 0:
            color = 0x57F287  # Green
            status_emoji = '✅'
            status_text = "All Tests Passed"
        else:
            color = 0xFEE75C  # Yellow
            status_emoji = '⚠️'
            status_text = "Tests Completed"

        repo = self.context.get('repository', 'unknown/repo')
        embed = self._get_base_embed(
            f"{status_emoji} Unit Tests",
            f"**{status_text}** for `{repo}`",
            color
        )

        # Add summary field
        lines = [
            f"✅ Passed: **{passed}**",
            f"❌ Failed: **{failed}**",
        ]
        if skipped > 0:
            lines.append(f"⏭️ Skipped: **{skipped}**")
        lines.append(f"\n📊 **Total:** {total}")
        lines.append(f"⏱️ **Duration:** {duration:.2f}s")

        embed["fields"].append({
            "name": "📊 Test Summary",
            "value": "\n".join(lines),
            "inline": False
        })

        # Add failed tests details if any
        if failed > 0 and 'failed_tests' in test_results:
            failed_tests = test_results['failed_tests'][:5]
            lines = []
            for test_name in failed_tests:
                lines.append(f"❌ `{test_name}`")

            if len(test_results['failed_tests']) > 5:
                lines.append(f"\n...and {len(test_results['failed_tests']) - 5} more")

            embed["fields"].append({
                "name": "❌ Failed Tests",
                "value": "\n".join(lines) if lines else "No details available",
                "inline": False
            })

        return embed

    def create_coverage_embed(self, coverage_data: Dict) -> Dict:
        """Create embed for code coverage results."""
        line_coverage = coverage_data.get('line_coverage', 0.0)
        function_coverage = coverage_data.get('function_coverage', 0.0)
        lines_covered = coverage_data.get('lines_covered', 0)
        lines_total = coverage_data.get('lines_total', 0)
        functions_covered = coverage_data.get('functions_covered', 0)
        functions_total = coverage_data.get('functions_total', 0)

        # Determine color and status based on line coverage
        if line_coverage >= 80:
            color = 0x57F287  # Green
            status_emoji = '✅'
            status_text = "Good Coverage"
        elif line_coverage >= 60:
            color = 0xFEE75C  # Yellow
            status_emoji = '⚠️'
            status_text = "Moderate Coverage"
        else:
            color = 0xED4245  # Red
            status_emoji = '❌'
            status_text = "Low Coverage"

        repo = self.context.get('repository', 'unknown/repo')
        embed = self._get_base_embed(
            f"{status_emoji} Code Coverage",
            f"**{status_text}** for `{repo}`",
            color
        )

        # Add coverage summary
        lines = [
            f"📈 **Line Coverage:** {line_coverage:.1f}%",
            f"   └ {lines_covered}/{lines_total} lines covered",
            f"🔧 **Function Coverage:** {function_coverage:.1f}%",
            f"   └ {functions_covered}/{functions_total} functions covered"
        ]

        embed["fields"].append({
            "name": "📊 Coverage Summary",
            "value": "\n".join(lines),
            "inline": False
        })

        # Add coverage bar visualization
        def get_coverage_bar(percentage: float) -> str:
            filled = int(percentage / 10)
            empty = 10 - filled
            return f"[{'█' * filled}{'░' * empty}] {percentage:.1f}%"

        embed["fields"].append({
            "name": "📊 Line Coverage",
            "value": f"`{get_coverage_bar(line_coverage)}`",
            "inline": False
        })

        return embed

    def create_combined_embed(self, test_results: Dict, coverage_data: Dict) -> Dict:
        """Create combined embed for tests and coverage."""
        total = test_results.get('total', 0)
        passed = test_results.get('passed', 0)
        failed = test_results.get('failed', 0)
        line_coverage = coverage_data.get('line_coverage', 0.0)

        # Determine overall status
        if failed > 0 or line_coverage < 60:
            color = 0xED4245  # Red
            status_emoji = '❌'
            status_text = "Issues Detected"
        elif line_coverage >= 80 and passed == total:
            color = 0x57F287  # Green
            status_emoji = '✅'
            status_text = "All Good"
        else:
            color = 0xFEE75C  # Yellow
            status_emoji = '⚠️'
            status_text = "Needs Improvement"

        repo = self.context.get('repository', 'unknown/repo')
        embed = self._get_base_embed(
            f"{status_emoji} Tests & Coverage",
            f"**{status_text}** for `{repo}`",
            color
        )

        # Add test summary
        test_lines = [
            f"✅ Passed: **{passed}**",
            f"❌ Failed: **{failed}**",
            f"📊 Total: **{total}**",
            f"⏱️ Duration: **{test_results.get('duration', 0.0):.2f}s**"
        ]

        embed["fields"].append({
            "name": "🧪 Test Results",
            "value": "\n".join(test_lines),
            "inline": True
        })

        # Add coverage summary
        coverage_lines = [
            f"📈 Line: **{line_coverage:.1f}%**",
            f"🔧 Function: **{coverage_data.get('function_coverage', 0.0):.1f}%**",
            f"📄 Lines: **{coverage_data.get('lines_covered', 0)}/{coverage_data.get('lines_total', 0)}**"
        ]

        embed["fields"].append({
            "name": "📊 Coverage",
            "value": "\n".join(coverage_lines),
            "inline": True
        })

        return embed


def main():
    """Main entry point."""
    webhook_url = os.environ.get('DISCORD_WEBHOOK')

    if not webhook_url:
        print("⚠️  DISCORD_WEBHOOK not set, skipping notification")
        return 0

    # Determine notification type from command line args
    if len(sys.argv) < 2:
        print("Usage: discord_notifier.py <type> [data_file]")
        print("Types: tests, coverage, combined")
        return 1

    notification_type = sys.argv[1]
    data_file = sys.argv[2] if len(sys.argv) > 2 else None

    notifier = DiscordNotifier(webhook_url)

    try:
        if notification_type == 'tests':
            # Load test results
            if not data_file:
                print("❌ Test results file required")
                return 1

            with open(data_file, 'r') as f:
                test_results = json.load(f)

            embed = notifier.create_test_embed(test_results)

        elif notification_type == 'coverage':
            # Load coverage data
            if not data_file:
                print("❌ Coverage data file required")
                return 1

            with open(data_file, 'r') as f:
                coverage_data = json.load(f)

            embed = notifier.create_coverage_embed(coverage_data)

        elif notification_type == 'combined':
            # Load both test and coverage data
            if not data_file:
                print("❌ Combined data file required")
                return 1

            with open(data_file, 'r') as f:
                combined_data = json.load(f)

            test_results = combined_data.get('tests', {})
            coverage_data = combined_data.get('coverage', {})

            embed = notifier.create_combined_embed(test_results, coverage_data)

        else:
            print(f"❌ Unknown notification type: {notification_type}")
            return 1

        # Send notification
        if notifier.send([embed]):
            print(f"✅ Successfully sent {notification_type} notification to Discord")
            return 0
        else:
            print(f"❌ Failed to send {notification_type} notification")
            return 1

    except Exception as e:
        print(f"❌ Error creating notification: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
