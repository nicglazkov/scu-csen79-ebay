#!/usr/bin/env python3

"""
Minimal HTTP server that serves the existing frontend files
and exposes /data/listings.json as JSON derived from listings_backup.txt.
Starts the C++ auction backend (auction_app) automatically so the site works with one command.
"""

import json
import os
import re
import signal
import subprocess
import sys
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import urlparse

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
BACKUP_PATH = os.path.join(ROOT_DIR, "listings_backup.txt")
STATIC_FALLBACK_JSON = os.path.join(ROOT_DIR, "data", "listings.json")
AUCTION_APP = os.path.join(ROOT_DIR, "auction_app")

_auction_process = None


def _start_auction_backend():
    """Start the C++ auction app in the background. Keeps running until we stop the server."""
    global _auction_process
    if not os.path.isfile(AUCTION_APP):
        print("Warning: auction_app not found. Run: g++ -std=c++17 runner.cpp main.c++ User.cpp Listing.c++ Listings.c++ Bid.cpp -pthread -o auction_app", file=sys.stderr)
        return
    _auction_process = subprocess.Popen(
        [AUCTION_APP],
        cwd=ROOT_DIR,
        stdin=subprocess.PIPE,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def _stop_auction_backend():
    """Stop the C++ auction app when the server exits."""
    global _auction_process
    if _auction_process is not None:
        _auction_process.terminate()
        _auction_process.wait(timeout=3)
        _auction_process = None


def parse_backup_file(path: str):
    """
    Parse listings_backup.txt into a list of listing dicts.
    The format is the one produced by Listings::saveToFile().
    """
    if not os.path.exists(path):
        return []

    with open(path, "r", encoding="utf-8") as f:
        text = f.read()

    # Split on separator lines
    blocks = [b.strip() for b in text.split("---------------------------------") if b.strip()]
    listings = []

    item_re = re.compile(r"^Item:\s*(.*)$")
    desc_re = re.compile(r"^Description:\s*(.*)$")
    curr_re = re.compile(r"^Current Price:\s*\$(.*)$")
    buy_re = re.compile(r"^Buy Outright Price:\s*\$(.*)$")

    for idx, block in enumerate(blocks, start=1):
        lines = [line.strip() for line in block.splitlines() if line.strip()]
        title = ""
        description = ""
        current = 0.0
        buyout = 0.0
        bids = 0

        in_bid_history = False
        for line in lines:
            if line.startswith("Bid History:"):
                in_bid_history = True
                continue

            if not in_bid_history:
                m = item_re.match(line)
                if m:
                    title = m.group(1)
                    continue
                m = desc_re.match(line)
                if m:
                    description = m.group(1)
                    continue
                m = curr_re.match(line)
                if m:
                    try:
                        current = float(m.group(1))
                    except ValueError:
                        current = 0.0
                    continue
                m = buy_re.match(line)
                if m:
                    try:
                        buyout = float(m.group(1))
                    except ValueError:
                        buyout = 0.0
                    continue
            else:
                # Each non-empty line in bid history corresponds to a bid
                bids += 1

        listings.append(
            {
                "id": idx,
                "title": title or f"Item {idx}",
                "description": description or "",
                "currentBid": current,
                "buyout": buyout if buyout > 0 else max(current, 0.0),
                # Simple heuristic increment so the UI works
                "bidIncrement": max(round(max(current, 1.0) * 0.05, 2), 0.25),
                "bids": bids,
                "seller": "Unknown",
                "condition": "Used",
                "timeLeft": "N/A",
                "image": "../assets/images/placeholder.png",
            }
        )

    return listings


class EbayLiteHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed = urlparse(self.path)
        if parsed.path == "/data/listings.json":
            self.handle_listings_json()
        else:
            # Serve static files from the project root
            super().do_GET()

    def handle_listings_json(self):
        # Prefer C++ backend output so the frontend shows real auction data.
        listings = parse_backup_file(BACKUP_PATH)
        if not listings and os.path.exists(STATIC_FALLBACK_JSON):
            try:
                with open(STATIC_FALLBACK_JSON, "r", encoding="utf-8") as f:
                    listings = json.load(f)
            except Exception:
                listings = []

        data = json.dumps(listings or [], indent=2)
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(data.encode("utf-8"))))
        self.end_headers()
        self.wfile.write(data.encode("utf-8"))


def run(host="0.0.0.0", port=8000):
    os.chdir(ROOT_DIR)

    def _on_exit(*_):
        _stop_auction_backend()

    signal.signal(signal.SIGINT, lambda s, f: (_on_exit(), sys.exit(0)))
    signal.signal(signal.SIGTERM, lambda s, f: (_on_exit(), sys.exit(0)))
    try:
        _start_auction_backend()
        with ThreadingHTTPServer((host, port), EbayLiteHandler) as httpd:
            print("eBay Lite: backend + web server started.")
            print(f"Open in browser: http://localhost:{port}/index.html")
            print("Press Ctrl+C to stop.")
            httpd.serve_forever()
    finally:
        _on_exit()


if __name__ == "__main__":
    run()

