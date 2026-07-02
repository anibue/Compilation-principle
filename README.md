# PL/0 编译器（C++Builder 6 VCL）

基于 Borland C++Builder 6（VCL）的 PL/0 编译器与解释器。将 PL/0 源代码编译为 P-Code（栈式虚拟机指令）并解释执行，提供 Windows 图形界面。

本次课程设计在原有基础上扩展了 7 类新语法：ELSE 子句、复合赋值（+=、-=、*=、/=）、自增自减（++、--）、FOR-TO/FOR-DOWNTO 循环、RETURN 提前返回。

## 目录结构

```
c_builder_b/
├── PL01.bpr              # C++Builder 工程文件（根目录）
├── src/
│   ├── PL01.cpp           # 程序入口（WinMain）
│   ├── Unit1.cpp          # 编译器核心：词法分析、语法分析、代码生成、解释执行（~980 行）
│   ├── Unit1.h            # 主窗体类 TForm1 定义
│   └── Unit1.dfm          # VCL 窗体布局
├── project/
│   ├── PL01.exe           # GUI 可执行文件
│   └── PL01.res           # 资源文件
├── test_cases/            # PL/0 测试样例（.PL0 源文件 + .COD 参考输出）
│   ├── E01.PL0 / E01.COD
│   ├── E0101.PL0 / E0101.COD
│   ├── FORT01.PL0 / FORT01.COD
│   ├── FORT02.PL0 / FORT02.COD
│   ├── FORT03.PL0 / FORT03.COD
│   ├── COMP01.PL0 / COMP01.COD
│   ├── INC01.PL0 / INC01.COD
│   ├── RET01.PL0 / RET01.COD
│   ├── P9101.PL0 / P9101.COD
│   ├── P9102.PL0 / P9102.COD
│   ├── P9104.PL0 / P9104.COD
│   ├── T1.PL0 / T1.COD
│   ├── EX000.PL0 / EX000.COD
│   ├── EX01.PL0 / EX01.COD
│   ├── EX10.PL0 / EX10.COD
│   ├── EX11.PL0 / EX11.COD
│   ├── E110.PL0 / E110.COD
│   └── EDGE01-11.PL0      # 边界测试用例
├── test_console/          # 命令行编译器驱动
│   ├── pl0_test.cpp       # console 版本编译器（复用 Unit1.cpp 核心逻辑）
│   ├── pl0_test.exe       # 命令行可执行文件
│   └── run_tests.sh       # 回归测试脚本
├── pascal_reference/      # Pascal 参考实现
├── docs/                  # 文档与报告生成
│   ├── report_figures/    # 报告图片（13 张：5 张 mermaid + 8 张 matplotlib）
│   ├── make_report_v6.py  # 报告生成脚本
│   └── ...
└── PL0编译器扩充实验报告.docx  # 实验报告（45 页）
```

## 主要文件

| 文件 | 作用 |
|------|------|
| `PL01.bpr` | C++Builder 工程文件 |
| `src/PL01.cpp` | 程序入口（WinMain） |
| `src/Unit1.cpp` | 编译器全部核心逻辑（~980 行） |
| `src/Unit1.h` | 主窗体类与输出辅助方法 |
| `src/Unit1.dfm` | VCL 窗体控件布局 |
| `project/PL01.exe` | GUI 可执行文件 |
| `test_cases/*.PL0` | PL/0 源程序测试用例 |
| `test_cases/*.COD` | 编译/运行输出对照 |
| `test_console/pl0_test.cpp` | 命令行编译器驱动 |
| `test_console/run_tests.sh` | 回归测试脚本 |
| `docs/report_figures/*.png` | 报告图片 |
| `PL0编译器扩充实验报告.docx` | 实验报告 |

## 开发环境

- Windows
- Borland C++Builder 6（VCL）
- Python 3.10+（报告生成、数据收集）

## 构建与运行

### 方式一：BCB6 IDE

1. 用 C++Builder 6 打开根目录下的 `PL01.bpr`。
2. `Project` → `Build PL01`（或 `Ctrl+F9`）编译。
3. `F9` 运行。

### 方式二：命令行编译

需要 BCB6 安装路径（示例为 `E:\\BCB`）：

```bash
# 编译
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp

# 链接（bash 环境需用 cmd //c 包裹）
cmd //c "E:\\BCB\\Bin\\ilink32 -aa -Tpe -x -Gn -jE:\\BCB\\Lib\\Obj -jE:\\BCB\\Lib -jE:\\BCB\\Lib\\Release E:\\BCB\\Lib\\c0w32.obj E:\\BCB\\Lib\\Release\\rtl.bpi E:\\BCB\\Lib\\Release\\vcl.bpi E:\\BCB\\Lib\\Release\\vclx.bpi E:\\BCB\\Lib\\memmgr.lib E:\\BCB\\Lib\\Obj\\sysinit.obj project\\PL01.obj project\\Unit1.obj,project\\PL01.exe,,E:\\BCB\\Lib\\import32.lib E:\\BCB\\Lib\\cp32mti.lib,,project\\PL01.res"
```

### 运行 GUI 程序

1. 运行 `project/PL01.exe`。
2. 在右侧输入框输入测试用例名（如 `E01`、`FORT01`、`COMP01`）。
3. 选择"显示"或"不显示"目标代码，点击 `RUN`。
4. 程序先在当前目录查找 `.PL0` 文件，未找到则查找 `test_cases\\` 子目录。

### 运行命令行回归测试

```bash
cd test_console
bash run_tests.sh
```

脚本会逐项执行 12 个正向用例与 4 个反向用例，输出 PASS/FAIL 状态。

## 语言特性

**已实现：**

- PROGRAM、CONST、VAR、PROCEDURE 声明
- BEGIN/END 块
- 赋值（`:=`）
- 算术运算（`+` `-` `*` `/`）
- 比较（`=` `<>` `!=` `<` `<=` `>` `>=`）
- ODD
- IF/THEN/ELSE
- WHILE/DO
- FOR-TO / FOR-DOWNTO
- CALL
- READ/WRITE
- 复合赋值（`+=` `-=` `*=` `/=`）
- 自增自减（`++` `--`）
- RETURN 提前返回
- 嵌套过程与词法作用域（最多 3 层）

## 测试样例

`test_cases/` 包含正向（回归）和负向（错误处理）两类样例：

| 样例 | 说明 | 类别 |
|------|------|------|
| `E01.PL0` | 基础测试（赋值+WRITE） | 正向 |
| `E0101.PL0` | 赋值+WRITE | 正向 |
| `FORT01.PL0` | FOR-TO 求和（1+2+3+4+5=15） | 正向 |
| `FORT02.PL0` | FOR-DOWNTO 求和（5+4+3+2+1=15） | 正向 |
| `FORT03.PL0` | FOR+WRITE 嵌套（1..10, 55） | 正向 |
| `COMP01.PL0` | 复合赋值（+= -= *= /=） | 正向 |
| `INC01.PL0` | 自增自减（++ --） | 正向 |
| `RET01.PL0` | RETURN 提前返回 | 正向 |
| `P9101.PL0` | 嵌套过程+WHILE | 正向 |
| `P9102.PL0` | 复杂表达式 | 正向 |
| `P9104.PL0` | 多过程嵌套 | 正向 |
| `T1.PL0` | WHILE 循环 | 正向 |
| `EX000.PL0` | 伪关键字/运算符 | 反向 |
| `EX01.PL0` | 扩展 token 探测 | 反向 |
| `EX10.PL0` | READ 语法错误 | 反向 |
| `EX11.PL0` | != / READ 错误 | 反向 |
| `E110.PL0` | ELSE 子句测试 | 正向 |
| `EDGE01-11.PL0` | 边界测试用例 | 正向 |

**回归结果：16/16 通过（100%）**

## 实验报告

`PL0编译器扩充实验报告.docx` 为本次课程设计的实验报告，共约 45 页，包含：

- 完整 EBNF 文法与 FIRST/FOLLOW 集推导
- GetSym 完整源代码（带扩展标记）
- FOR 循环执行轨迹表
- 4 张真实运行数据图（P-Code 长度、累计值、轨迹、指令分布）
- 16 个测试用例结果汇总
- 附录：Token 列表、测试源码、核心函数清单、编译错误代码表

## 参考资料

- `pascal_reference/PL0.PAS` — 原始 Pascal 参考实现
- 课程设计指导书（广东工业大学计算机学院）
