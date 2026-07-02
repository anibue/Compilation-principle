# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

A PL/0 compiler and interpreter built with Borland C++Builder 6 (VCL). It compiles PL/0 source code into P-Code (stack-based VM instructions) and interprets it, all within a Windows GUI application. This is a university compiler course project (Guangdong University of Technology, 2026).

This project extended the original PL/0 compiler with 7 new language features: ELSE clause, compound assignment (+=, -=, *=, /=), increment/decrement (++/---), FOR-TO/FOR-DOWNTO loops, and RETURN statement.

## Build

**BCB6 IDE:** Open `PL01.bpr` (root directory) in Borland C++Builder 6. Build to produce `project/PL01.exe`.

**Console driver** (for automated regression testing without GUI):
```bash
cd test_console
bcc32 -c -tWM -I"E:/BCB/include" -o pl0_test.obj pl0_test.cpp
ilink32 -Tpe -x -ap c0x32.obj pl0_test.obj,pl0_test.exe,,import32.lib cw32mti.lib
```

**Command line** (for GUI executable, requires BCB6 installed, e.g. at `E:\\BCB`):
```bash
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp
cmd //c "E:\\BCB\\Bin\\ilink32 -aa -Tpe -x -Gn -jE:\\BCB\\Lib\\Obj -jE:\\BCB\\Lib -jE:\\BCB\\Lib\\Release E:\\BCB\\Lib\\c0w32.obj E:\\BCB\\Lib\\Release\\rtl.bpi E:\\BCB\\Lib\\Release\\vcl.bpi E:\\BCB\\Lib\\Release\\vclx.bpi E:\\BCB\\Lib\\memmgr.lib E:\\BCB\\Lib\\Obj\\sysinit.obj project\\PL01.obj project\\Unit1.obj,project\\PL01.exe,,E:\\BCB\\Lib\\import32.lib E:\\BCB\\Lib\\cp32mti.lib,,project\\PL01.res"
```

Note: `ilink32` must be invoked via `cmd //c "..."` wrapper when running from bash to avoid path parsing issues.

Do not assume a modern build pipeline, CI, or automated test harness exists. If you introduce one, keep it additive.

## Usage

### GUI Mode
1. Run `project/PL01.exe`.
2. The dropdown (ComboCase) auto-populates with `.PL0` file names found in the exe directory and `test_cases\\`.
3. Select or type a test case name (e.g., `E01`), then click **RUN**.
4. Output appears in the Memo panel and a `.COD` file is generated alongside the source.
5. **刷新列表** rescans directories. **清空输出** clears the Memo.

### Console Mode
```bash
cd test_console
bash run_tests.sh
```
Runs 12 positive + 4 negative test cases, outputs PASS/FAIL status. 16/16 expected to pass (100%).

## Architecture

The entire compiler is a **single-file implementation** in `src/Unit1.cpp` (~980 lines). All compiler state is global. There is no modular separation between lexer, parser, code generator, and interpreter.

**File roles:**
- `src/PL01.cpp` — VCL entry point (WinMain), 21 lines.
- `src/Unit1.h` — Form class `TForm1` with VCL controls (`ComboCase`, `ButtonRun`, `ButtonRefresh`, `ButtonClear`, `Memo1`, `ListSwitch`) and helper output methods (`printfs`, `printcs`, `printls`, `printrs`, `prinths`).
- `src/Unit1.cpp` — Everything: lexer (`GetSym`), recursive-descent parser (`Block`, `STATEMENT`, `EXPRESSION`, `TERM`, `FACTOR`, `CONDITION`), code generator (`GEN`), symbol table (`ENTER`, `POSITION`), stack-based interpreter (`Interpret`), and UI logic (`LoadTestCases`, `FormCreate`, event handlers).
- `src/Unit1.dfm` — VCL form layout.
- `test_console/pl0_test.cpp` — Console driver duplicating compiler logic without VCL (~772 lines). Synchronizes with Unit1.cpp manually.
- `test_console/run_tests.sh` — Bash regression script.
- `pascal_reference/PL0.PAS` — Original Pascal reference implementation this was ported from.

**Key data structures (all file-scope globals in Unit1.cpp):**
- `SYMBOL` enum — all token types (base PL/0 + extensions: ELSESYM, FORSYM, TOSYM, DOWNTOSYM, RETURNSYM, PLUSEQ, MINUSEQ, TIMESBECOMES, SLASHBECOMES, PLUSPLUS, MINUSMINUS, NEQ, LEQ, GEQ)
- `FCT` enum — P-Code opcodes (LIT, OPR, LOD, STO, CAL, INI, JMP, JPC)
- `INSTRUCTION` struct — P-Code instruction: F (function), L (level), A (address)
- `TABLE[100]` — symbol table entries with name, kind (CONSTANT/VARIABLE/PROCEDUR), value/level/ADR/size
- `CODE[200]` — generated P-Code program

**Compilation pipeline** (orchestrated by `ButtonRunClick`):
1. Initialize reserved words, symbol sets, and operator mappings.
2. Locate input `.PL0` file: try exe dir, then `test_cases\\`, then parent dir's `test_cases\\`, then parent dir itself.
3. `GetSym` -> parse `PROGRAM` header -> `Block()` -> expect `.` -> `Interpret()` if no errors.
4. Output goes to both the GUI memo and a `.COD` file.

**UI initialization** (`FormCreate` -> `LoadTestCases`):
- Uses Win32 `FindFirstFile`/`FindNextFile` to scan for `*.PL0` in exe dir, `test_cases\\` (exe and parent level), and parent dir.
- Deduplicates entries and populates the ComboCase dropdown.

## Language Features

**Working:** PROGRAM, CONST, VAR, PROCEDURE declarations; BEGIN/END blocks; assignment (`:=`); arithmetic (`+`, `-`, `*`, `/`); comparisons (`=`, `<>`, `!=`, `<`, `<=`, `>`, `>=`); ODD; IF/THEN/ELSE; WHILE/DO; FOR-TO / FOR-DOWNTO loops; CALL; READ/WRITE; compound assignment (`+=`, `-=`, `*=`, `/=`); increment/decrement (`A++`, `A--`); RETURN statement; nested procedures with lexical scoping (up to 3 levels).

**Lexed but NOT implemented in codegen/interpreter:** *(none — all extended tokens are now end-to-end)*

All 16 new tokens (ELSE, FOR, TO, DOWNTO, RETURN; +=, -=, *=, /=, ++, --; !=, <>; <=, >=) are fully implemented through lexer, parser, code generation, and interpreter. Do not describe a feature as supported unless lexer + parser + codegen + test coverage are all in place.

## Testing

Two test drivers exist:

1. **GUI driver** (`project/PL01.exe`): Select test case from dropdown, click RUN, compare with `.COD`.
2. **Console driver** (`test_console/pl0_test.exe` + `run_tests.sh`): Automated regression without GUI.

Some test cases are intentionally **negative** (trigger parser/syntax errors). When reporting results, explicitly distinguish positive (regression) cases from negative (error-handling) cases.

**Test cases (16 total, 100% pass rate):**
| Name | Type | Description |
|------|------|-------------|
| E01, E0101 | Positive | Basic arithmetic + WRITE |
| FORT01, FORT02, FORT03 | Positive | FOR-TO/DOWNTO loops |
| COMP01, INC01, RET01 | Positive | Compound assignment, ++/--, RETURN |
| P9101, P9102, P9104, T1 | Positive | Nested procedures, WHILE |
| E110 | Positive | IF-THEN-ELSE |
| EX000, EX01, EX10, EX11 | Negative | Pseudo-keywords, READ errors |

## Coding Style

- Preserve the existing C++Builder/VCL style in `Unit1.cpp`. Avoid broad formatting churn.
- Compiler symbols use uppercase: `SYM`, `CODE`, `TABLE`, enum values like `IDENT`, `WHILESYM`. Follow that convention when extending internals.
- Keep VCL form declarations and event handlers aligned with the `.dfm` file.
- Do not delete or regenerate C++Builder project files (`*.dfm`, `*.bpr`, `*.res`). Treat them as IDE-managed artifacts.
- The `.bpr` file is at the project root (not `project/`), because its relative paths (`src\\`, `project\\`) are resolved from the `.bpr`'s directory.

## Console Driver Notes

When extending compiler features, you must update **both** files:
- `src/Unit1.cpp` (GUI version)
- `test_console/pl0_test.cpp` (console version)

The console driver uses `#define` VCL types to C equivalents (e.g., `TMemo` -> `FILE*`). It requires `__InitVCL`/`__ExitVCL` stubs from `pl0_test.cpp`. The linker uses `cw32mti.lib` (not `cp32mti.lib`) to avoid VCL dependency.

**Important:** The console driver must be rebuilt separately. Its `.obj` is **not** part of the GUI build. Both drivers should produce identical `.COD` output for the same `.PL0` input.

## Report Generation

The experiment report is generated by Python:
```bash
D:/anaconda/python.exe docs/make_report_v6.py
D:/anaconda/python.exe docs/finalize_report.py
```

Output: `PL0编译器扩充实验报告.docx` (~45 pages, 11 images, 16 tables). The `finalize_report.py` script adds section breaks and page numbers (starting from 1 at Chapter 1).

Figures are in `docs/report_figures/` (13 PNGs: 5 mermaid diagrams + 8 matplotlib plots from real execution data).

## Commit Style

Short, imperative subjects (e.g., `Add FOR loop code generation tests`, `Fix console driver link error`).
