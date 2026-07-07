# 提交说明

本文档用于说明本仓库的课程设计交付内容、运行方式和检查顺序。

## 交付内容

| 内容 | 路径 | 说明 |
| --- | --- | --- |
| GUI 工程 | `PL01.bpr`, `src/`, `project/PL01.res` | C++Builder 6 VCL 项目 |
| 编译器核心 | `src/Unit1.cpp` | 词法分析、语法分析、P-Code 生成和解释执行 |
| 控制台测试器 | `test_console/pl0_test.cpp` | 自动回归测试入口 |
| 测试用例 | `test_cases/` | PL/0 源程序和对应输出 |
| 实验报告 | `PL0编译器扩充实验报告.docx` | 课程设计报告 |
| 测试说明 | `测试用例说明.md` | 测试覆盖点说明 |
| 项目说明 | `README.md` | GitHub 仓库主页说明 |

## 功能完成情况

| 扩展功能 | 状态 | 代表用例 |
| --- | --- | --- |
| `IF ... THEN ... ELSE` | 已实现 | `E110.PL0`, `ELSE_NOTEQ.PL0` |
| `FOR ... TO ... DO` | 已实现 | `FORT01.PL0`, `FORT03.PL0` |
| `FOR ... DOWNTO ... DO` | 已实现 | `FORT02.PL0` |
| `+=`, `-=`, `*=`, `/=` | 已实现 | `COMP01.PL0` |
| `++`, `--` | 已实现 | `INC01.PL0` |
| `RETURN` | 已实现 | `RET01.PL0` |
| `<>`, `!=`, `<=`, `>=` | 已实现 | `E110.PL0`, `ELSE_NOTEQ.PL0` |

## 运行 GUI

在 Windows 环境中打开 `PL01.bpr`，使用 Borland C++Builder 6 编译运行。也可以直接运行已生成的程序：

```powershell
.\project\PL01.exe
```

运行后在下拉框中选择测试用例，点击 `RUN` 编译并执行。新增测试文件后点击 `Refresh` 重新扫描。

## 运行控制台测试

```bash
cd test_console
bash run_tests.sh
```

控制台脚本用于快速检查主要正向回归用例。错误处理类和边界类用例可结合 GUI 或控制台驱动单独验证。

单个用例运行格式如下：

```powershell
cmd /c test_console\pl0_test.exe test_cases\COMP01.PL0 build_tmp\COMP01.COD
```

## 推荐检查顺序

1. 阅读 `README.md`，确认项目结构和构建方式。
2. 打开 `src/Unit1.cpp`，检查扩展 token、语法分析和解释执行逻辑。
3. 运行 `test_console/run_tests.sh`，确认主要回归用例通过。
4. 使用 GUI 运行 `E110.PL0`、`FORT01.PL0`、`COMP01.PL0`、`RET01.PL0` 等代表用例。
5. 对照 `PL0编译器扩充实验报告.docx` 查看实验设计、结果截图和分析。

## 注意事项

- `test_cases/` 中包含正向、反向和边界用例，不应把所有文件都视为成功编译样例。
- `project/` 和 `test_console/` 中可能包含编译生成的二进制文件，提交前应根据 `.gitignore` 规则检查。
- 修改编译器行为时，需要同步维护 GUI 版本 `src/Unit1.cpp` 和控制台版本 `test_console/pl0_test.cpp`。
