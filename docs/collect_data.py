# -*- coding: utf-8 -*-
"""Step 1: actually run the PL/0 compiler and collect real outputs.
The output data is saved to JSON so the figure scripts can reuse it."""
import os, json, re, subprocess

ROOT = r"C:\Users\xwmsu\Desktop\c_builder_b"
EXE = os.path.join(ROOT, "test_console", "pl0_test.exe")
TC_DIR = os.path.join(ROOT, "test_cases")
OUT_DIR = os.path.join(ROOT, "docs", "report_figures")
TMP_DIR = os.path.join(ROOT, "docs", "report_figures", "_run")
os.makedirs(TMP_DIR, exist_ok=True)

# (case_name, stdin or "")
RUN_TABLE = [
    ("E01", ""),
    ("E0101", ""),
    ("E110", "0"),
    ("EX10", "456"),
    ("EX11", "123"),
    ("FORT01", ""),
    ("FORT02", ""),
    ("FORT03", ""),
    ("COMP01", ""),
    ("INC01", ""),
    ("RET01", ""),
    ("P9101", "2"),
    ("P9102", "2"),
    ("P9104", "2"),
    ("T1", ""),
    ("T2", ""),
    ("EX01", ""),
    ("EX000", ""),
]

results = {}
for (case, stdin) in RUN_TABLE:
    src = os.path.join(TC_DIR, case + ".PL0")
    out = os.path.join(TMP_DIR, case + ".cod")
    if not os.path.isfile(src):
        print("MISS:", src); continue
    try:
        p = subprocess.run([EXE, src, out],
                           input=(stdin + "\n").encode() if stdin else b"",
                           timeout=10, capture_output=True)
    except subprocess.TimeoutExpired:
        print("TIMEOUT:", case); continue
    if not os.path.isfile(out): continue
    text = open(out, encoding="utf-8", errors="replace").read()
    # parse P-Code instructions
    pcode = []
    run_output = []
    in_run = False
    err_marks = []
    for line in text.splitlines():
        m = re.match(r"^\s*(\d+)\s+(LIT|LOD|STO|OPR|CAL|INI|JMP|JPC)\s+(\d+)\s+(\d+)\s*$",
                     line)
        if m:
            pcode.append((int(m.group(1)), m.group(2),
                          int(m.group(3)), int(m.group(4))))
            continue
        if "RUN PL0" in line:
            in_run = True; continue
        if "END PL0" in line:
            in_run = False; continue
        if in_run and line.strip() and not line.startswith("?"):
            run_output.append(line.strip())
        m2 = re.match(r"^\*+ +(\^\d+)\s*$", line)
        if m2:
            err_marks.append(m2.group(1))
    results[case] = {
        "pcode_len": len(pcode),
        "pcode": pcode,
        "run_output": run_output,
        "err_marks": err_marks,
        "is_negative": bool(err_marks) or case in ("EX01", "EX000", "T2"),
    }

# Sort by category for charts
positive = [c for c, r in results.items() if not r["is_negative"]]
negative = [c for c, r in results.items() if r["is_negative"]]

with open(os.path.join(OUT_DIR, "run_results.json"), "w", encoding="utf-8") as f:
    json.dump(results, f, ensure_ascii=False, indent=2)

# Print summary
print("Positive cases:")
for c in positive:
    print(f"  {c}: pcode_len={results[c]['pcode_len']}, "
          f"run_output={results[c]['run_output']}")
print("Negative cases:")
for c in negative:
    print(f"  {c}: err_marks={results[c]['err_marks']}")
print("Saved to", os.path.join(OUT_DIR, "run_results.json"))