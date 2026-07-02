# Repository Guidelines

## Project Structure & Module Organization

This repository contains a Borland C++Builder 6 VCL implementation of a PL/0 compiler with 7 extended language features.

- `src/` contains the application source. `PL01.cpp` is the VCL entry point, while `Unit1.cpp`, `Unit1.h`, and `Unit1.dfm` define the main form and compiler logic (~980 lines).
- `project/` contains C++Builder project artifacts (`PL01.res`, compiled `.obj` files, `PL01.exe`). The `.bpr` project file lives at the repository root.
- `test_cases/` contains PL/0 sample programs (`*.PL0`) and expected/generated code output (`*.COD`), including 11 new test cases + 11 edge cases.
- `test_console/` contains the console version compiler driver (`pl0_test.cpp`, ~772 lines), regression script (`run_tests.sh`), and built executable (`pl0_test.exe`).
- `pascal_reference/PL0.PAS` is the Pascal reference implementation.
- `docs/` contains experiment documentation, report generation scripts, and 13 report figures.
- `PL0编译器扩充实验报告.docx` is the compiler extension experiment report (~45 pages, root directory).

Most compiler behavior is concentrated in `src/Unit1.cpp` (~980 lines). Treat it as the single source of truth. The console driver in `test_console/pl0_test.cpp` must be manually synchronized with any changes to Unit1.cpp.

**GUI controls** (defined in `Unit1.h` / `Unit1.dfm`):
- `ComboCase` (TComboBox) — dropdown for selecting test cases, auto-populated on startup via `LoadTestCases()`.
- `ButtonRun` — triggers compilation and interpretation.
- `ButtonRefresh` — rescans directories for `.PL0` files.
- `ButtonClear` — clears the output Memo.
- `Memo1` — displays compilation/execution output.
- `ListSwitch` (TRadioGroup) — toggles P-Code listing display.

## Build, Test, and Development Commands

This is a legacy Windows IDE project with an optional console driver for automated testing.

### GUI Build
- Open `PL01.bpr` in Borland C++Builder 6. Build to produce `project/PL01.exe`.

Command-line GUI build:
```bash
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp
cmd //c "E:\BCB\Bin\ilink32 -aa -Tpe -x -Gn -jE:\BCB\Lib\Obj -jE:\BCB\Lib -jE:\BCB\Lib\Release E:\BCB\Lib\c0w32.obj E:\BCB\Lib\Release\rtl.bpi E:\BCB\Lib\Release\vcl.bpi E:\BCB\Lib\Release\vclx.bpi E:\BCB\Lib\memmgr.lib E:\BCB\Lib\Obj\sysinit.obj project\PL01.obj project\Unit1.obj,project\PL01.exe,,E:\BCB\Lib\import32.lib E:\BCB\Lib\cp32mti.lib,,project\PL01.res"
```

### Console Driver Build (for automated regression)
```bash
cd test_console
bcc32 -c -tWM -I"E:/BCB/include" -o pl0_test.obj pl0_test.cpp
ilink32 -Tpe -x -ap c0x32.obj pl0_test.obj,pl0_test.exe,,import32.lib cw32mti.lib
bash run_tests.sh
```

Note: `ilink32` requires a `cmd //c "..."` wrapper when invoked from bash. Use `cw32mti.lib` (not `cp32mti.lib`) for the console driver to avoid VCL dependency.

### Report Generation
```bash
D:/anaconda/python.exe docs/make_report_v6.py
D:/anaconda/python.exe docs/finalize_report.py
```

## Coding Style & Naming Conventions

Preserve the existing C++Builder/VCL style. Use the current brace and indentation patterns in `src/Unit1.cpp`. Keep VCL form declarations and event handlers aligned with the `.dfm` file. Existing compiler symbols use uppercase names such as `SYM`, `CODE`, `TABLE`, and enum values like `IDENT` or `WHILESYM`.

DFM files use `#NNNNN` format for Unicode characters (decimal code points). For Chinese characters, ANSI_CHARSET with 仿宋 font renders correctly. GB2312_CHARSET with 宋体 may cause garbled text — verify encoding when modifying labels.

All extended tokens (`ELSE`, `FOR/TO/DOWNTO`, `RETURN`, `+=`, `-=`, `*=`, `/=`, `++`, `--`, `<>`, `!=`, `<=`, `>=`) are fully implemented through lexer, parser, code generation, and interpreter — paired positive regression tests exist for each. Do not describe a feature as supported unless lexer + parser + codegen + test coverage are all in place.

## Testing Guidelines

Two test drivers exist:

1. **GUI driver** (`project/PL01.exe`): Select test case from dropdown, click RUN, compare with `.COD`.
2. **Console driver** (`test_console/pl0_test.exe` + `run_tests.sh`): Automated regression. 16/16 expected to pass.

**Test cases (16 total, 100% pass rate):**
| Name | Type | Description |
|------|------|-------------|
| E01, E0101 | Positive | Basic arithmetic + WRITE |
| FORT01, FORT02, FORT03 | Positive | FOR-TO/DOWNTO loops |
| COMP01, INC01, RET01 | Positive | Compound assignment, ++/--, RETURN |
| P9101, P9102, P9104, T1 | Positive | Nested procedures, WHILE |
| E110 | Positive | IF-THEN-ELSE |
| EX000, EX01, EX10, EX11 | Negative | Pseudo-keywords, READ errors |

Also includes `EDGE01-11.PL0` for boundary testing (empty FOR range, same-bounds FOR, nested FOR, procedure calls with new syntax, etc.).

Do not treat every file in `test_cases/` as a successful compilation sample. Some inputs are intentionally negative cases. When reporting test results, explicitly distinguish positive execution cases from negative error-handling cases.

## Commit & Pull Request Guidelines

The current history uses short, descriptive commit subjects such as `Add README`. Keep subjects imperative and concise, for example `Add FOR loop code generation tests`. Pull requests should include a summary, affected compiler areas, manual test cases run, and screenshots only when UI behavior changes.

## Agent-Specific Instructions

Do not delete or regenerate C++Builder project files unless required. Treat `*.dfm`, `*.bpr`, and `*.res` as IDE-managed artifacts and keep edits minimal. When editing `.dfm` files, preserve exact indentation and VCL property formatting.

**Console driver sync:** When extending compiler features, you must update both `src/Unit1.cpp` (GUI) and `test_console/pl0_test.cpp` (console). The console driver uses `#define` VCL types to C equivalents and requires `__InitVCL`/`__ExitVCL` stubs. Both drivers should produce identical `.COD` output for the same `.PL0` input.

**Build from command line notes:** (1) copy `src\Unit1.dfm` to the root before linking (the `.rc` resource script references it), (2) use `cmd //c` wrapper for `ilink32`, (3) kill any running `PL01.exe` before relinking.

The repository contains text files with legacy Chinese encoding issues. Prefer adding clean UTF-8 documentation rather than rewriting legacy files unless specifically asked.

## Experiment Report

The report (`PL0编译器扩充实验报告.docx`, ~45 pages) is generated from Python scripts:
- `docs/make_report_v6.py` — generates the docx with all content
- `docs/finalize_report.py` — adds section break before Chapter 1 + PAGE field in footer

Figures are in `docs/report_figures/` (13 PNGs: 5 mermaid flowcharts + 8 matplotlib plots from real execution data). All figures are embedded with 宋体五号 captions below.
