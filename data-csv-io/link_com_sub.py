#!/usr/bin/env python3
"""
Join Pushshift submissions and comments dumps into a single CSV by linking
comment.link_id (t3_<submission_id>) to submission.id.

Credit to Watchful1 PushshiftDumps: filter_file.py for original file->csv

Usage examples:
  # Single files
  python data-csv-io/link_com_sub.py subs.zst comments.zst out/joined.csv

  # Folders (process all .zst/.jsonl/.txt inside each folder)
  python data-csv-io/link_com_sub.py --subs-dir path/to/subs_dir --comms-dir path/to/comments_dir out/joined.csv

  # Chosen Columns
  edit the write row and w.writerow to change the columns that are output into the csv file (KEEP TEXT BODY)

Dependencies:
  pip install zstandard
"""

import argparse
import csv
import json
import os
import sys
from datetime import datetime
from typing import Dict, Any, Iterable, List

try:
    import zstandard
except ModuleNotFoundError:
    print("Missing dependency: zstandard. Install with: python -m pip install zstandard", file=sys.stderr)
    sys.exit(1)


# -------- IO helpers --------

def is_zst(path: str) -> bool:
    return path.lower().endswith(".zst")


def is_textish(path: str) -> bool:
    low = path.lower()
    return low.endswith(".jsonl") or low.endswith(".ndjson") or low.endswith(".txt")


def list_inputs(path: str) -> List[str]:
    if os.path.isdir(path):
        out = []
        for name in sorted(os.listdir(path)):
            full = os.path.join(path, name)
            if os.path.isfile(full) and (is_zst(full) or is_textish(full)):
                out.append(full)
        return out
    return [path]


def read_lines(path: str) -> Iterable[str]:
    """Yield NDJSON lines from .zst or plaintext file."""
    if is_zst(path):
        with open(path, "rb") as fh:
            reader = zstandard.ZstdDecompressor(max_window_size=2**31).stream_reader(fh)
            buf = ""
            while True:
                chunk = reader.read(2**20)
                if not chunk:
                    break
                buf += chunk.decode("utf-8", errors="strict")
                while True:
                    nl = buf.find("\n")
                    if nl == -1:
                        break
                    line = buf[:nl]
                    buf = buf[nl + 1:]
                    if line:
                        yield line
            reader.close()
            if buf:
                yield buf
    else:
        with open(path, "r", encoding="utf-8") as fh:
            for line in fh:
                line = line.rstrip("\n")
                if line:
                    yield line


# -------- Join logic --------

def normalize_link_id(link_id: Any) -> str:
    s = link_id if isinstance(link_id, str) else ""
    return s[3:] if s.startswith("t3_") else s


def utc_to_iso(ts) -> str:
    try:
        return datetime.utcfromtimestamp(int(ts)).strftime("%Y-%m-%d %H:%M:%S")
    except Exception:
        return ""


def ensure_submission_permalink(sub: Dict[str, Any]) -> str:
    if not sub:
        return ""
    pl = sub.get("permalink")
    if pl:
        return "https://www.reddit.com" + pl
    sid = sub.get("submission_id")
    sr = sub.get("subreddit")
    if sid and sr:
        return f"https://www.reddit.com/r/{sr}/comments/{sid}/"
    return ""


def load_submissions(paths: List[str]) -> Dict[str, Dict[str, Any]]:
    """
    Build id -> minimal submission record.
    Only keep fields we actually write to reduce memory.
    """
    subs: Dict[str, Dict[str, Any]] = {}
    for p in paths:
        for line in read_lines(p):
            try:
                obj = json.loads(line)
            except json.JSONDecodeError:
                continue
            sid = obj.get("id")
            if not sid:
                continue
            subs[sid] = {
                "submission_id": sid,
                "subreddit": obj.get("subreddit"),
                "author": obj.get("author"),
                "title": obj.get("title"),
                "is_self": obj.get("is_self"),
                "selftext": obj.get("selftext"),
                "url": obj.get("url"),
                "score": obj.get("score"),
                "created_utc": obj.get("created_utc"),
                "permalink": obj.get("permalink"),
            }
    return subs


def join_to_csv(sub_paths: List[str], comm_paths: List[str], out_csv: str, keep_unmatched: bool) -> None:
    subs = load_submissions(sub_paths)

    os.makedirs(os.path.dirname(out_csv) or ".", exist_ok=True)
    with open(out_csv, "w", encoding="utf-8", newline="") as fh:
        w = csv.writer(fh)
        w.writerow([
            # Comment side
            "comment_id","comment_author","comment_subreddit","comment_score",
            "comment_created_utc","comment_created_iso","comment_body",
            "comment_parent_id","comment_link_id","comment_permalink",
            # Submission side
            "submission_id","submission_author","submission_subreddit","submission_score",
            "submission_created_utc","submission_created_iso","submission_title",
            "submission_is_self","submission_selftext_or_url","submission_permalink",
        ])

        total = matched = skipped = 0
        for p in comm_paths:
            for line in read_lines(p):
                total += 1
                try:
                    c = json.loads(line)
                except json.JSONDecodeError:
                    skipped += 1
                    continue

                sid = normalize_link_id(c.get("link_id"))
                sub = subs.get(sid)

                if not sub and not keep_unmatched:
                    skipped += 1
                    continue

                c_id = c.get("id")
                c_author = c.get("author")
                c_sr = c.get("subreddit")
                c_score = c.get("score")
                c_utc = c.get("created_utc")
                c_iso = utc_to_iso(c_utc)
                c_body = c.get("body")
                c_parent = c.get("parent_id")
                c_pl = c.get("permalink")
                if c_pl:
                    c_pl = "https://www.reddit.com" + c_pl
                else:
                    if c_id and sid and c_sr:
                        c_pl = f"https://www.reddit.com/r/{c_sr}/comments/{sid}/_/{c_id}"
                    else:
                        c_pl = ""

                s_id = sub.get("submission_id") if sub else ""
                s_author = sub.get("author") if sub else ""
                s_sr = sub.get("subreddit") if sub else ""
                s_score = sub.get("score") if sub else ""
                s_utc = sub.get("created_utc") if sub else ""
                s_iso = utc_to_iso(s_utc) if sub else ""
                s_title = sub.get("title") if sub else ""
                s_is_self = sub.get("is_self") if sub else ""
                if sub:
                    s_self_or_url = sub.get("selftext") if sub.get("is_self") else sub.get("url")
                else:
                    s_self_or_url = ""
                s_pl = ensure_submission_permalink(sub)

                w.writerow([
                    c_id,c_author,c_sr,c_score,c_utc,c_iso,c_body,c_parent,c.get("link_id"),c_pl,
                    s_id,s_author,s_sr,s_score,s_utc,s_iso,s_title,s_is_self,s_self_or_url,s_pl
                ])
                if sub:
                    matched += 1

    print(f"Done. Processed comments: {total:,} | matched: {matched:,} | skipped: {skipped:,}")


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="Join subreddit comments to submissions by link_id/id.")
    ap.add_argument("submissions", nargs="?", help="Submissions file (.zst/.jsonl/.txt) or directory")
    ap.add_argument("comments", nargs="?", help="Comments file (.zst/.jsonl/.txt) or directory")
    ap.add_argument("output_csv", nargs="?", help="Output CSV path")
    ap.add_argument("--subs-dir", help="Alternative: directory of submissions files")
    ap.add_argument("--comms-dir", help="Alternative: directory of comments files")
    ap.add_argument("--keep-unmatched", action="store_true", help="Keep comments whose submissions are missing")
    return ap.parse_args()


def main():
    args = parse_args()

    # Allow either positional path(s) or --subs-dir/--comms-dir
    if args.subs_dir and args.comms_dir and args.output_csv:
        sub_paths = list_inputs(args.subs_dir)
        comm_paths = list_inputs(args.comms_dir)
        out_csv = args.output_csv
    else:
        if not (args.submissions and args.comments and args.output_csv):
            print("Usage:\n  python scripts/join_comments_to_submissions.py <submissions> <comments> <output.csv>\n"
                  "or\n  python scripts/join_comments_to_submissions.py --subs-dir <dir> --comms-dir <dir> <output.csv>", file=sys.stderr)
            sys.exit(2)
        sub_paths = list_inputs(args.submissions)
        comm_paths = list_inputs(args.comments)
        out_csv = args.output_csv

    print(f"Submissions inputs: {len(sub_paths)} file(s)")
    print(f"Comments inputs:    {len(comm_paths)} file(s)")
    join_to_csv(sub_paths, comm_paths, out_csv, args.keep_unmatched)


if __name__ == "__main__":
    main()