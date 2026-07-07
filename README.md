# PL/0 编译器扩充实验

这是一个基于 Borland C++Builder 6 VCL 的 PL/0 编译器与解释器。程序读取 PL/0 源文件，生成栈式虚拟机 P-Code，并在 Windows 图形界面中解释执行。

本项目在教学版 PL/0 编译器基础上扩展了条件分支、FOR 循环、复合赋值、自增自减、提前返回和更多比较运算符，并配套提供 GUI 程序、控制台测试器、测试用例和实验报告。

## 功能特性

| 类别 | 支持内容 |
| --- | --- |
| 程序结构 | `PROGRAM`, `CONST`, `VAR`, `PROCEDURE`, `BEGIN ... END` |
| 赋值语句 | `:=`, `+=`, `-=`, `*=`, `/=` |
| 算术运算 | `+`, `-`, `*`, `/` |
| 比较运算 | `=`, `<>`, `!=`, `<`, `<=`, `>`, `>=`, `ODD` |
| 控制流 | `IF ... THEN ... ELSE`, `WHILE ... DO`, `FOR ... TO ... DO`, `FOR ... DOWNTO ... DO` |
| 过程调用 | `CALL`, 嵌套过程, `RETURN` |
| 输入输出 | `READ`, `WRITE` |
| 简化写法 | `++`, `--` |

## 项目结构

```text
c_builder_b/
|-- PL01.bpr                    # C++Builder 6 工程文件
|-- src/                        # VCL 程序和编译器实现
|   |-- PL01.cpp                # WinMain 入口
|   |-- Unit1.cpp               # 词法分析、语法分析、代码生成、解释执行
|   |-- Unit1.h                 # 主窗体与共享声明
|   `-- Unit1.dfm               # VCL 窗体布局
|-- project/                    # C++Builder 资源与构建输出
|-- test_cases/                 # PL/0 测试源文件和 .COD 输出
|-- test_console/               # 控制台回归测试器
|   |-- pl0_test.cpp
|   `-- run_tests.sh
|-- pascal_reference/           # Pascal 参考实现
|-- docs/                       # 报告生成脚本和图表
|-- SUBMISSION.md               # 提交说明
|-- 测试用例说明.md              # 测试用例说明
`-- PL0编译器扩充实验报告.docx    # 实验报告
```

编译器核心逻辑主要位于 `src/Unit1.cpp`。如果修改编译器语义，需要同步维护 `test_console/pl0_test.cpp`，保证 GUI 和控制台测试器行为一致。

## 环境要求

- Windows
- Borland C++Builder 6
- Bash 兼容环境，用于运行 `test_console/run_tests.sh`
- Python 3.10+，仅用于运行 `docs/` 下的报告生成脚本

## 构建与运行

### 使用 C++Builder IDE

1. 用 Borland C++Builder 6 打开 `PL01.bpr`。
2. 编译工程。
3. 运行生成的 `project/PL01.exe`。

### 使用命令行构建

以下命令假设 BCB6 安装在 `E:\BCB`。

```bat
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp
cmd //c "E:\BCB\Bin\ilink32 -aa -Tpe -x -Gn -jE:\BCB\Lib\Obj -jE:\BCB\Lib -jE:\BCB\Lib\Release E:\BCB\Lib\c0w32.obj E:\BCB\Lib\Release\rtl.bpi E:\BCB\Lib\Release\vcl.bpi E:\BCB\Lib\Release\vclx.bpi E:\BCB\Lib\memmgr.lib E:\BCB\Lib\Obj\sysinit.obj project\PL01.obj project\Unit1.obj,project\PL01.exe,,E:\BCB\Lib\import32.lib E:\BCB\Lib\cp32mti.lib,,project\PL01.res"
```

重新链接前请关闭正在运行的 `PL01.exe`。如果资源脚本从根目录引用 `Unit1.dfm`，需要先把 `src\Unit1.dfm` 复制到仓库根目录。

### 运行 GUI

1. 启动 `project/PL01.exe`。
2. 在下拉框中选择 `.PL0` 测试文件。
3. 按需切换 P-Code 列表显示。
4. 点击 `RUN` 编译并执行。
5. 新增测试文件后点击 `Refresh` 重新扫描。

## 控制台回归测试

控制台测试器适合快速检查核心编译逻辑，不需要打开 GUI。

```bash
cd test_console
bash run_tests.sh
```

脚本会运行主要正向回归用例，并比较运行输出。边界用例和错误处理用例保存在 `test_cases/` 中，可结合 GUI 或控制台驱动单独验证。

## 测试用例

| 分组 | 文件 | 验证目标 |
| --- | --- | --- |
| 基础 PL/0 | `E01`, `E0101`, `P9101`, `P9102`, `P9104`, `T1` | 保证原有 PL/0 功能未被破坏 |
| 条件分支 | `E110`, `ELSE_NOTEQ` | 验证 `ELSE`, `<>`, `!=` |
| 循环语句 | `FORT01`, `FORT02`, `FORT03`, `EDGE01`-`EDGE07` | 验证 `FOR TO`, `FOR DOWNTO`, 嵌套循环和边界 |
| 赋值扩展 | `COMP01`, `INC01`, `EDGE09`-`EDGE11` | 验证复合赋值和 `++`/`--` |
| 提前返回 | `RET01` | 验证过程中的 `RETURN` |
| 错误处理 | `EX000`, `EX01`, `EX10`, `EX11`, `T2`, `test1` | 验证词法和语法错误处理 |

`test_cases/` 中并非所有文件都应成功编译。部分用例是故意构造的错误输入，用于验证错误提示和恢复逻辑。

## 实验报告

实验报告由 `docs/` 下的脚本生成。

```bash
D:/anaconda/python.exe docs/make_report_v6.py
D:/anaconda/python.exe docs/finalize_report.py
```

生成图表位于 `docs/report_figures/`，最终报告为 `PL0编译器扩充实验报告.docx`。

## 开发说明

- 保持 `.dfm`、`.bpr`、`.res` 等 C++Builder 工程文件的格式稳定。
- 修改编译器功能时，同步更新 GUI 版本 `src/Unit1.cpp` 和控制台版本 `test_console/pl0_test.cpp`。
- `.COD` 文件可能是参考输出，也可能是本地生成结果；引用前应确认对应 `.PL0` 是正向用例还是错误处理用例。
- 不建议把编译生成的 `.obj`、`.tds`、`.il*` 等文件作为源码维护。

## 参考资料

- `pascal_reference/PL0.PAS`：原始 Pascal 参考实现。
- `测试用例说明.md`：测试用例分组与验证目标。
- `SUBMISSION.md`：课程设计提交清单与检查顺序。
