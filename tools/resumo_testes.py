"""Le docs/testes_campo.csv e imprime a tabela de taxa de acerto por cenario e iluminacao.

    python tools/resumo_testes.py
Colunas do CSV: cenario, iluminacao, tentativa, esperado, detectado, tempo_reacao_ms, observacao
  - detectado: ok (reagiu como esperado) ou falha
Avisa quando algum cenario tem menos de 5 tentativas (meta da versao minima: 5 por cenario e iluminacao).
"""
import csv
import sys
from collections import defaultdict
from pathlib import Path

ARQ = Path(__file__).resolve().parents[1] / "docs" / "testes_campo.csv"
if not ARQ.exists():
    sys.exit(f"Nao encontrei {ARQ}")

grupos = defaultdict(lambda: {"n": 0, "ok": 0, "tempos": []})
with ARQ.open(encoding="utf-8") as f:
    for r in csv.DictReader(f):
        if not r.get("detectado"):
            continue                                   # linha de modelo ainda nao preenchida
        g = grupos[(r["cenario"], r["iluminacao"])]
        g["n"] += 1
        g["ok"] += r["detectado"].strip().lower() == "ok"
        if r.get("tempo_reacao_ms", "").strip().isdigit():
            g["tempos"].append(int(r["tempo_reacao_ms"]))

if not grupos:
    sys.exit("Nenhuma tentativa preenchida ainda em docs/testes_campo.csv")

print("| Cenario | Iluminacao | Tentativas | Acertos | Taxa | Tempo medio de reacao |")
print("|---|---|---|---|---|---|")
for (cen, ilu), g in sorted(grupos.items()):
    taxa = 100 * g["ok"] / g["n"]
    tempo = f"{sum(g['tempos']) / len(g['tempos']):.0f} ms" if g["tempos"] else "-"
    aviso = "  <- faltam tentativas (meta: 5)" if g["n"] < 5 else ""
    print(f"| {cen} | {ilu} | {g['n']} | {g['ok']} | {taxa:.0f}% | {tempo} |{aviso}")
