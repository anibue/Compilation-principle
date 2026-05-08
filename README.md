# PL/0 编译器（C++Builder 6 VCL）

基于 Borland C++Builder 6（VCL）的 PL/0 编译器与解释器。将 PL/0 源代码编译为 P-Code（栈式虚拟机指令）并解释执行，提供 Windows 图形界面。

## 目录结构

```
c_builder_b/
├── PL01.bpr              # C++Builder 工程文件（根目录）
├── src/
│   ├── PL01.cpp           # 程序入口（WinMain）
│   ├── Unit1.cpp          # 编译器核心：词法分析、语法分析、代码生成、解释执行
│   ├── Unit1.h            # 主窗体类 TForm1 定义
│   └── Unit1.dfm          # VCL 窗体布局
├── project/
│   └── PL01.res           # 资源文件
├── test_cases/            # PL/0 测试样例
├── pascal_reference/      # Pascal 参考实现
└── docs/                  # 文档
```

## 主要文件

| 文件 | 作用 |
|------|------|
| `PL01.bpr` | C++Builder 工程文件 |
| `src/PL01.cpp` | 程序入口（WinMain） |
| `src/Unit1.cpp` | 编译器全部核心逻辑（~810 行） |
| `src/Unit1.h` | 主窗体类与输出辅助方法 |
| `src/Unit1.dfm` | VCL 窗体控件布局 |
| `test_cases/*.PL0` | PL/0 源程序测试用例 |
| `test_cases/*.COD` | 编译/运行输出对照 |

## 开发环境

- Windows
- Borland C++Builder 6（VCL）

## 构建与运行

### 方式一：BCB6 IDE

1. 用 C++Builder 6 打开根目录下的 `PL01.bpr`。
2. `Project` → `Build PL01`（或 `Ctrl+F9`）编译。
3. `F9` 运行。

### 方式二：命令行编译

需要 BCB6 安装路径（示例为 `E:\BCB`）：

```bash
# 编译
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/Unit1.obj" src/Unit1.cpp
bcc32 -c -tWM -I"E:/BCB/include" -I"E:/BCB/include/vcl" -o"project/PL01.obj" src/PL01.cpp

# 链接
ilink32 -aa -Tpe -x -Gn \
  -jE:\BCB\Lib\Obj -jE:\BCB\Lib -jE:\BCB\Lib\Release \
  E:\BCB\Lib\c0w32.obj E:\BCB\Lib\Release\rtl.bpi E:\BCB\Lib\Release\vcl.bpi E:\BCB\Lib\Release\vclx.bpi \
  E:\BCB\Lib\memmgr.lib E:\BCB\Lib\Obj\sysinit.obj \
  project\PL01.obj project\Unit1.obj,project\PL01.exe,, \
  E:\BCB\Lib\import32.lib E:\BCB\Lib\cp32mti.lib,,project\PL01.res
```

### 运行程序

1. 运行生成的 `project/PL01.exe`。
2. 在右侧输入框输入测试用例名（如 `E01`、`EX01`、`E110`）。
3. 选择"显示"或"不显示"目标代码，点击 `RUN`。
4. 程序先在当前目录查找 `.PL0` 文件，未找到则查找 `test_cases\` 子目录。

## 语言特性

**已实现：** PROGRAM、CONST、VAR、PROCEDURE 声明；BEGIN/END 块；赋值（`:=`）；算术运算（`+` `-` `*` `/`）；比较（`=` `<>` `!=` `<` `<=` `>` `>=`）；ODD；IF/THEN/ELSE；WHILE/DO；CALL；READ/WRITE；复合赋值（`*=` `/=`）；嵌套过程与词法作用域（最多 3 层）。

**已词法识别但未实现代码生成/解释：** FOR/TO/DOWNTO、RETURN、`++` `--` `+=` `-=`。

## 测试样例

`test_cases/` 包含正向（回归）和负向（错误处理）两类样例：

| 样例 | 说明 |
|------|------|
| `E01.PL0` / `E01.COD` | 基础测试 |
| `EX01.PL0` / `EX01.COD` | 扩展特性测试 |
| `EX10.PL0` / `EX10.COD` | 扩展测试 |
| `EX11.PL0` / `EX11.COD` | 扩展测试 |
| `E110.PL0` / `E110.COD` | 扩展测试 |

## 参考资料

- `pascal_reference/PL0.PAS` — 原始 Pascal 参考实现
