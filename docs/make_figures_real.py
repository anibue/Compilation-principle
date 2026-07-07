# -*- coding: utf-8 -*-
"""Generate matplotlib figures from REAL run results."""
import os, json
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams["font.sans-serif"] = ["SimHei", "Microsoft YaHei", "DejaVu Sans"]
plt.rcParams["axes.unicode_minus"] = False

OUT = r"C:\Users\xwmsu\Desktop\c_builder_b\docs\report_figures"
DATA = json.load(open(os.path.join(OUT, "run_results.json"),
                       encoding="utf-8"))


# =========================================================
# Figure: 真实运行输出柱状图（各测试样例 P-Code 长度）
# =========================================================
def fig_pcode_lengths():
    positive_cases = [c for c, r in DATA.items() if not r["is_negative"]]
    pcode_lens = [DATA[c]["pcode_len"] for c in positive_cases]
    fig, ax = plt.subplots(figsize=(11, 5.0))
    bars = ax.bar(positive_cases, pcode_lens,
                  color="#5b8def", edgecolor="#222")
    for b, v in zip(bars, pcode_lens):
        ax.text(b.get_x() + b.get_width()/2, v + 0.5, str(v),
                ha="center", fontsize=9.5)
    ax.set_ylabel("生成的 P-Code 指令条数")
    ax.set_title("图 6.3  各正向测试样例生成 P-Code 的长度对比（真实运行结果）",
                 fontsize=12.5, fontweight="bold")
    ax.grid(True, axis="y", linestyle=":", alpha=0.4)
    ax.set_ylim(0, max(pcode_lens) + 8)
    plt.xticks(rotation=20)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_pcode_lengths.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: FORT03 累计和曲线（基于真实输出 1..10 + sum=55）
# =========================================================
def fig_fort03_cumulative():
    actual = [int(x) for x in DATA["FORT03"]["run_output"][:10]]
    sums = np.cumsum(actual)
    fig, ax = plt.subplots(figsize=(8.5, 4.5))
    ax.plot(actual, sums, "o-", color="#2b4f81",
            linewidth=2, markersize=7, label="$\\sum_{i=1}^{n} i$")
    for x, y in zip(actual, sums):
        ax.annotate(f"{y}", (x, y),
                    textcoords="offset points", xytext=(0, 8),
                    ha="center", fontsize=9, color="#333")
    ax.set_xlabel("循环变量 i")
    ax.set_ylabel("累计和 S")
    ax.set_title("图 6.4  FORT03 在循环过程中 S 的累计值（真实运行轨迹）",
                 fontsize=12.5, fontweight="bold")
    ax.grid(True, linestyle=":", alpha=0.5)
    ax.legend(loc="upper left")
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_fort03_cumulative.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: 反向用例触发的错误号分布
# =========================================================
def fig_error_codes():
    err_counter = {}
    for c, r in DATA.items():
        if r["is_negative"]:
            for m in r["err_marks"]:
                # m looks like ^19
                err_counter[m] = err_counter.get(m, 0) + 1
    items = sorted(err_counter.items(), key=lambda x: int(x[0][1:]))
    codes = [k for k, _ in items]
    counts = [v for _, v in items]
    fig, ax = plt.subplots(figsize=(8, 4.3))
    bars = ax.bar(codes, counts, color="#e57373", edgecolor="#222")
    for b, v in zip(bars, counts):
        ax.text(b.get_x() + b.get_width()/2, v + 0.05, str(v),
                ha="center", fontsize=10)
    ax.set_xlabel("错误号")
    ax.set_ylabel("在反向用例中触发的次数")
    ax.set_title("图 6.5  反向测试用例触发的错误号分布（实际统计）",
                 fontsize=12.5, fontweight="bold")
    ax.grid(True, axis="y", linestyle=":", alpha=0.4)
    ax.set_ylim(0, max(counts) + 0.8)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_error_codes.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: 各类 OPR 指令在所有测试中出现的频次
# =========================================================
def fig_opr_freq():
    counter = {}
    for c, r in DATA.items():
        for (_, op, _, a) in r["pcode"]:
            if op == "OPR":
                counter[a] = counter.get(a, 0) + 1
    items = sorted(counter.items())
    op_names = {
        0: "RET", 1: "NEG", 2: "ADD", 3: "SUB",
        4: "MUL", 5: "DIV", 6: "ODD",
        8: "EQL", 9: "NEQ", 10: "LSS", 11: "GEQ",
        12: "GTR", 13: "LEQ", 14: "WRT", 15: "WRLN",
        16: "READ"
    }
    labels = [f"OPR {a}\n{op_names.get(a, '?')}" for a, _ in items]
    vals = [v for _, v in items]
    fig, ax = plt.subplots(figsize=(12, 4.5))
    bars = ax.bar(labels, vals, color="#7e57c2", edgecolor="#222")
    for b, v in zip(bars, vals):
        ax.text(b.get_x() + b.get_width()/2, v + 0.2, str(v),
                ha="center", fontsize=9)
    ax.set_ylabel("在所有正向 P-Code 中出现次数")
    ax.set_title("图 6.6  OPR 指令 A 域各操作的频次（基于真实 P-Code 统计）",
                 fontsize=12.5, fontweight="bold")
    ax.grid(True, axis="y", linestyle=":", alpha=0.4)
    plt.xticks(rotation=25)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_opr_freq.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: 整体指令类型分布饼图
# =========================================================
def fig_pcode_distribution():
    counter = {}
    for c, r in DATA.items():
        if r["is_negative"]: continue
        for (_, op, _, _) in r["pcode"]:
            counter[op] = counter.get(op, 0) + 1
    labels = list(counter.keys())
    sizes = list(counter.values())
    # color
    cmap = plt.cm.Set2
    colors = [cmap(i/len(labels)) for i in range(len(labels))]
    fig, ax = plt.subplots(figsize=(7, 6))
    wedges, texts, autotexts = ax.pie(sizes, labels=labels, autopct="%.1f%%",
                                       colors=colors, startangle=90,
                                       wedgeprops=dict(edgecolor="white",
                                                       linewidth=1.4))
    for t in autotexts: t.set_color("#222"); t.set_fontsize(9.5)
    ax.set_title("图 6.7  P-Code 指令类型分布（正向用例统计）",
                 fontsize=12.5, fontweight="bold")
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_pcode_distribution.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: 单测试用例 P-Code 折线图（FORT03）
# =========================================================
def fig_fort03_trace():
    pcode = DATA["FORT03"]["pcode"]
    addrs = [a for (a, op, _, _) in pcode]
    ops = [op for (_, op, _, _) in pcode]
    # encode op as integer height
    op_height = {op: i for i, op in enumerate(["LIT", "LOD", "STO",
                                                 "OPR", "INI", "JMP",
                                                 "JPC", "CAL"])}
    heights = [op_height.get(o, 0) for o in ops]
    fig, ax = plt.subplots(figsize=(11, 4.5))
    ax.plot(addrs, heights, "o-", color="#3949ab", linewidth=1.6)
    for (a, op, _, _) in pcode:
        ax.annotate(op, (a, op_height[op]),
                    textcoords="offset points", xytext=(0, 6),
                    ha="center", fontsize=7.5, color="#333")
    ax.set_xlabel("P-Code 地址 (CX)")
    ax.set_yticks(list(op_height.values()))
    ax.set_yticklabels(list(op_height.keys()))
    ax.set_title("图 6.8  FORT03 的 P-Code 地址-指令轨迹",
                 fontsize=12.5, fontweight="bold")
    ax.grid(True, axis="y", linestyle=":", alpha=0.5)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_fort03_trace.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: 源码文件分布 (统计 Unit1.cpp 各函数行数)
# =========================================================
def fig_func_lines():
    src = r"C:\Users\xwmsu\Desktop\c_builder_b\src\Unit1.cpp"
    funcs = {}
    cur, lines = None, 0
    for ln in open(src, encoding="utf-8", errors="replace").readlines():
        if ln.startswith("void ") or ln.startswith("void __fastcall"):
            if cur is not None:
                funcs[cur] = max(funcs.get(cur, 0), lines)
            cur = ln.strip().split("(")[0].replace("void ", "").strip()
            if cur.startswith("__fastcall "):
                cur = cur.replace("__fastcall ", "")
            lines = 0
        elif cur:
            lines += 1
    if cur: funcs[cur] = max(funcs.get(cur, 0), lines)
    funcs = dict(sorted(funcs.items(), key=lambda x: -x[1])[:10])
    labels = list(funcs.keys())
    vals = list(funcs.values())
    fig, ax = plt.subplots(figsize=(10, 5))
    bars = ax.barh(labels, vals, color="#26a69a", edgecolor="#222")
    for b, v in zip(bars, vals):
        ax.text(v + 0.5, b.get_y() + b.get_height()/2, str(v),
                va="center", fontsize=9)
    ax.set_xlabel("函数体行数")
    ax.invert_yaxis()
    ax.set_title("图 4.1  Unit1.cpp 中各核心函数的规模（Top 10）",
                 fontsize=12.5, fontweight="bold")
    ax.grid(True, axis="x", linestyle=":", alpha=0.4)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_func_lines.png"), dpi=180)
    plt.close()


# =========================================================
# Figure: 测试通过率扇形图
# =========================================================
def fig_test_pass():
    pos = sum(1 for r in DATA.values() if not r["is_negative"])
    neg = sum(1 for r in DATA.values() if r["is_negative"])
    fig, ax = plt.subplots(figsize=(6, 6))
    ax.pie([pos, neg],
           labels=[f"正向通过\n({pos} 项)", f"反向触发\n({neg} 项)"],
           colors=["#66bb6a", "#ef5350"],
           autopct="%.1f%%", startangle=90,
           wedgeprops=dict(edgecolor="white", linewidth=1.6),
           textprops=dict(fontsize=11))
    ax.set_title("图 6.1  测试用例分类饼图", fontsize=13, fontweight="bold")
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, "fig_test_pass.png"), dpi=180)
    plt.close()


if __name__ == "__main__":
    fig_pcode_lengths()
    fig_fort03_cumulative()
    fig_error_codes()
    fig_opr_freq()
    fig_pcode_distribution()
    fig_fort03_trace()
    fig_func_lines()
    fig_test_pass()
    print("All matplotlib figures generated.")