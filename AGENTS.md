# Repository Guidelines

## Project Structure & Module Organization

This repository contains a Borland C++Builder 6 VCL implementation of a PL/0 compiler.

- `src/` contains the application source. `PL01.cpp` is the VCL entry point, while `Unit1.cpp`, `Unit1.h`, and `Unit1.dfm` define the main form and compiler logic.
- `project/` contains C++Builder project artifacts (`PL01.res`, compiled `.obj` files, `PL01.exe`). The `.bpr` project file lives at the repository root.
- `test_cases/` contains PL/0 sample programs (`*.PL0`) and expected/generated code output (`*.COD`).
- `pascal_reference/PL0.PAS` is the Pascal reference implementation.
- `docs/` contains experiment documentation and the Python script `generate_repo_summary_doc.py` for generating the Chinese repository description docx.
- `PL0编译器扩充实验报告.docx` is the compiler extension experiment report (root directory).

Most compiler behavior is concentrated in `src/Unit1.cpp` (~900 lines). Treat it as the single source of truth for lexer, parser, code generation, and interpreter behavior. Avoid assuming the project is modularized just because functionality is conceptually separated.

**GUI controls** (defined in `Unit1.h` / `Unit1.dfm`):
- `ComboCase` (TComboBox) — dropdown for selecting test cases, auto-populated on startup via `LoadTestCases()`.
- `ButtonRun` — triggers compilation and interpretation.
- `ButtonRefresh` — rescans directories for `.PL0` files.
- `ButtonClear` — clears the output Memo.
- `Memo1` — displays compilation/execution output.
- `ListSwitch` (TRadioGroup) — toggles P-Code listing display.

## Build, Test, and Development Commands

This is a legacy Windows IDE project, not a command-line build by default.

- Open `PL01.bpr` (at repository root) in Borland C++Builder 6.
- Build from the IDE to produce `project/PL01.exe`.

Command-line build requires BCB6 (e.g. at `E:\BCB`):
```bash
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp
ilink32 -aa -Tpe -x -Gn -jE:\BCB\Lib\Obj -jE:\BCB\Lib -jE:\BCB\Lib\Release E:\BCB\Lib\c0w32.obj E:\BCB\Lib\Release\rtl.bpi E:\BCB\Lib\Release\vcl.bpi E:\BCB\Lib\Release\vclx.bpi E:\BCB\Lib\memmgr.lib E:\BCB\Lib\Obj\sysinit.obj project\PL01.obj project\Unit1.obj,project\PL01.exe,,E:\BCB\Lib\import32.lib E:\BCB\Lib\cp32mti.lib,,project\PL01.res
```
Note: `ilink32` requires a `cmd //c "..."` wrapper when invoked from bash.

Do not assume a modern build pipeline, CI job, or automated regression harness exists. If you introduce one, keep it additive and document how it coexists with the IDE-driven workflow.

## Coding Style & Naming Conventions

Preserve the existing C++Builder/VCL style. Use the current brace and indentation patterns in `src/Unit1.cpp`; avoid broad formatting churn. Keep VCL form declarations and event handlers aligned with the `.dfm` file. Existing compiler symbols use uppercase names such as `SYM`, `CODE`, `TABLE`, and enum values like `IDENT` or `WHILESYM`; follow that convention when extending compiler internals.

DFM files use `#NNNNN` format for Unicode characters (decimal code points). For Chinese characters, ANSI_CHARSET with 仿宋 font renders correctly. GB2312_CHARSET with 宋体 may cause garbled text — verify encoding when modifying labels.

When working on language features, verify whether a token is only recognized lexically or is actually implemented end-to-end. In particular, `FOR` is present in the reserved-word/token layer but is not implemented as a working statement form in `STATEMENT`; do not describe it as supported unless you complete lexer, parser, code generation, and test coverage together.

## Testing Guidelines

There is no automated test framework. Test changes manually through the GUI:

1. Build the project in C++Builder.
2. Run `project/PL01.exe`, select a test case from the dropdown (or type a name).
3. Compare output with the matching `*.COD` file when one exists.
4. Add new paired test files, for example `FEATURE01.PL0` and `FEATURE01.COD`, for new syntax or compiler behavior.

The executable searches for `.PL0` files in: exe directory, `test_cases\` (exe level), `test_cases\` (parent level), and parent directory.

Cover normal cases and parser error cases when changing lexical analysis, grammar handling, or code generation.

Do not treat every file in `test_cases/` as a successful compilation sample. Some inputs are intentionally negative cases used to trigger parser or syntax errors. When reporting test results, explicitly distinguish:

1. positive execution/regression cases
2. negative syntax/error-handling cases

If you add or modify a grammar feature, update both categories when relevant.

## Commit & Pull Request Guidelines

The current history uses short, descriptive commit subjects such as `Add README`. Keep subjects imperative and concise, for example `Add for-loop code generation tests`. Pull requests should include a summary, affected compiler areas, manual test cases run, and screenshots only when UI behavior changes. Link related issues or assignment requirements when available.

## Agent-Specific Instructions

Do not delete or regenerate C++Builder project files unless required. Treat `*.dfm`, `*.bpr`, and `*.res` as IDE-managed artifacts and keep edits minimal. When editing `.dfm` files, preserve exact indentation and VCL property formatting.

The repository contains text files with legacy Chinese encoding issues, especially in `README.md`. If you need to edit or quote those files, check for mojibake first and avoid spreading corrupted text into new documentation or source comments. Prefer adding clean UTF-8 documentation rather than rewriting legacy files unless the task specifically asks for an encoding cleanup.

When building from the command line, remember: (1) copy `src\Unit1.dfm` to the root before linking (the `.rc` resource script references it), (2) use `cmd //c` wrapper for `ilink32`, (3) kill any running `PL01.exe` before relinking.
