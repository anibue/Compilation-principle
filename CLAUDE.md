# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

A PL/0 compiler and interpreter built with Borland C++Builder 6 (VCL). It compiles PL/0 source code into P-Code (stack-based VM instructions) and interprets it, all within a Windows GUI application. This is a university compiler course project.

## Build

**BCB6 IDE:** Open `PL01.bpr` (root directory) in Borland C++Builder 6. Build to produce `project/PL01.exe`.

**Command line** (requires BCB6 installed, e.g. at `E:\BCB`):
```bash
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp
ilink32 -aa -Tpe -x -Gn -jE:\BCB\Lib\Obj -jE:\BCB\Lib -jE:\BCB\Lib\Release E:\BCB\Lib\c0w32.obj E:\BCB\Lib\Release\rtl.bpi E:\BCB\Lib\Release\vcl.bpi E:\BCB\Lib\Release\vclx.bpi E:\BCB\Lib\memmgr.lib E:\BCB\Lib\Obj\sysinit.obj project\PL01.obj project\Unit1.obj,project\PL01.exe,,E:\BCB\Lib\import32.lib E:\BCB\Lib\cp32mti.lib,,project\PL01.res
```

Note: `ilink32` must be invoked via `cmd //c "..."` wrapper when running from bash to avoid path parsing issues.

Do not assume a modern build pipeline, CI, or automated test harness exists. If you introduce one, keep it additive.

## Usage

1. Run `project/PL01.exe`.
2. The dropdown (ComboCase) auto-populates with `.PL0` file names found in the exe directory and `test_cases\`.
3. Select or type a test case name (e.g., `E01`), then click **RUN**.
4. Output appears in the Memo panel and a `.COD` file is generated alongside the source.
5. **刷新列表** rescans directories. **清空输出** clears the Memo.

## Architecture

The entire compiler is a **single-file implementation** in `src/Unit1.cpp` (~900 lines). All compiler state is global. There is no modular separation between lexer, parser, code generator, and interpreter.

**File roles:**
- `src/PL01.cpp` — VCL entry point (WinMain), 21 lines.
- `src/Unit1.h` — Form class `TForm1` with VCL controls (`ComboCase`, `ButtonRun`, `ButtonRefresh`, `ButtonClear`, `Memo1`, `ListSwitch`) and helper output methods (`printfs`, `printcs`, `printls`, `printrs`, `prinths`).
- `src/Unit1.cpp` — Everything: lexer (`GetSym`), recursive-descent parser (`Block`, `STATEMENT`, `EXPRESSION`, `TERM`, `FACTOR`, `CONDITION`), code generator (`GEN`), symbol table (`ENTER`, `POSITION`), stack-based interpreter (`Interpret`), and UI logic (`LoadTestCases`, `FormCreate`, event handlers).
- `src/Unit1.dfm` — VCL form layout.
- `pascal_reference/PL0.PAS` — Original Pascal reference implementation this was ported from.

**Key data structures (all file-scope globals in Unit1.cpp):**
- `SYMBOL` enum — all token types (base PL/0 + extensions like ELSESYM, FORSYM, PLUSEQ, etc.)
- `FCT` enum — P-Code opcodes (LIT, OPR, LOD, STO, CAL, INI, JMP, JPC)
- `INSTRUCTION` struct — P-Code instruction: F (function), L (level), A (address)
- `TABLE[100]` — symbol table entries with name, kind (CONSTANT/VARIABLE/PROCEDUR), value/level/ADR/size
- `CODE[200]` — generated P-Code program

**Compilation pipeline** (orchestrated by `ButtonRunClick`):
1. Initialize reserved words, symbol sets, and operator mappings.
2. Locate input `.PL0` file: try exe dir, then `test_cases\`, then parent dir's `test_cases\`, then parent dir itself.
3. `GetSym` -> parse `PROGRAM` header -> `Block()` -> expect `.` -> `Interpret()` if no errors.
4. Output goes to both the GUI memo and a `.COD` file.

**UI initialization** (`FormCreate` -> `LoadTestCases`):
- Uses Win32 `FindFirstFile`/`FindNextFile` to scan for `*.PL0` in exe dir, `test_cases\` (exe and parent level), and parent dir.
- Deduplicates entries and populates the ComboCase dropdown.

## Language Features

**Working:** PROGRAM, CONST, VAR, PROCEDURE declarations; BEGIN/END blocks; assignment (`:=`); arithmetic (`+`, `-`, `*`, `/`); comparisons (`=`, `<>`, `!=`, `<`, `<=`, `>`, `>=`); ODD; IF/THEN/ELSE; WHILE/DO; CALL; READ/WRITE; compound assignment (`*=`, `/=`); nested procedures with lexical scoping (up to 3 levels).

**Lexed but NOT implemented in codegen/interpreter:**
- `FOR` / `TO` / `DOWNTO` — tokens exist, keywords registered, but `STATEMENT()` has no case for `FORSYM`
- `RETURN` — token exists but not handled
- `++`, `--`, `+=`, `-=` — tokens lexed but not handled in `STATEMENT()`

## Testing

There is no automated test framework. Test changes manually through the GUI.

1. Build in C++Builder.
2. Run representative inputs from `test_cases/*.PL0`.
3. Compare output with matching `*.COD` file when one exists.
4. Add paired test files (e.g., `FEATURE01.PL0` + `FEATURE01.COD`) for new features.

Some test cases are intentionally **negative** (trigger parser/syntax errors). When reporting results, explicitly distinguish positive (regression) cases from negative (error-handling) cases.

## Coding Style

- Preserve the existing C++Builder/VCL style in `Unit1.cpp`. Avoid broad formatting churn.
- Compiler symbols use uppercase: `SYM`, `CODE`, `TABLE`, enum values like `IDENT`, `WHILESYM`. Follow that convention when extending internals.
- Keep VCL form declarations and event handlers aligned with the `.dfm` file.
- Do not delete or regenerate C++Builder project files (`*.dfm`, `*.bpr`, `*.res`). Treat them as IDE-managed artifacts.
- The `.bpr` file is at the project root (not `project/`), because its relative paths (`src\`, `project\`) are resolved from the `.bpr`'s directory.
- The repository has legacy Chinese encoding issues (mojibake), especially in `README.md`. Prefer adding clean UTF-8 documentation rather than rewriting legacy files unless specifically asked.

## Commit Style

Short, imperative subjects (e.g., `Add for-loop code generation tests`).
