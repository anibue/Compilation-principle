# c_builder_b

基于 Borland C++Builder（VCL）的 PL/0 编译器实验项目。提供图形界面用于加载/编译 PL/0 源程序并查看输出结果，附带测试样例与参考实现。

## 目录结构

- `src/`：主程序源码与窗体代码
- `project/`：C++Builder 工程文件与资源文件
- `test_cases/`：PL/0 测试样例与输出结果
- `pascal_reference/`：PL/0 Pascal 参考实现
- `docs/`：实验文档（若有）

## 主要文件

- `src/PL01.cpp`：程序入口
- `src/Unit1.cpp` / `src/Unit1.h`：主窗体及逻辑
- `project/PL01.bpr`：工程文件
- `test_cases/*.PL0`：测试源程序
- `test_cases/*.COD`：编译/运行输出对照

## 开发环境

- Windows
- Borland C++Builder 6（VCL）

## 运行方式

1. 使用 C++Builder 打开 `project/PL01.bpr`。
2. 编译生成可执行文件。
3. 运行程序，在界面中加载或输入 PL/0 源码并执行编译。

## 测试样例

`test_cases/` 目录包含多个样例，例如：

- `EX01.PL0` / `EX01.COD`
- `E01.PL0` / `E01.COD`
- `EX10.PL0` / `EX10.COD`
- `EX11.PL0` / `EX11.COD`

## 参考资料

- `pascal_reference/PL0.PAS`

> 本项目基于较早期的 C++Builder 工程结构，若在现代环境中构建可能需要迁移或调整工程配置。
