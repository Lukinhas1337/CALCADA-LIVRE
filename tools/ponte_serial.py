"""Ponte opcional: le a serial do carrinho, grava CSV e (se pedir) envia as ocorrencias a uma API.

    python tools/ponte_serial.py --porta COM3
    python tools/ponte_serial.py --porta COM3 --url http://localhost:8000/api/ocorrencias --rota R01
O Arduino Uno nao tem Wi-Fi: a rede fica por conta do computador. Se a API estiver fora do ar,
o evento fica numa fila local (pendentes.jsonl) e e reenviado depois. O FORMATO DO JSON deve ser
combinado com quem faz o backend do grupo (ajuste montar_json). Sem --url, so grava o CSV.
"""
import argparse
import csv
import datetime as dt
import json
import sys
from pathlib import Path

try:
    import serial          # pip install pyserial
except ImportError:
    sys.exit("Instale: pip install pyserial")


def montar_json(rota, tipo, dist, luz):
    return {"rota_id": rota, "tipo": tipo, "distancia_cm": dist, "luz": luz,
            "origem": "carrinho", "detectado_em": dt.datetime.now().isoformat(timespec="seconds")}


def enviar(url, corpo):
    import requests        # pip install requests
    r = requests.post(url, json=corpo, timeout=5)
    return r.status_code in (200, 201)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--porta", required=True)
    ap.add_argument("--baud", type=int, default=9600)
    ap.add_argument("--rota", default="R01")
    ap.add_argument("--url", default="")
    ap.add_argument("--saida", default="logs")
    a = ap.parse_args()

    pasta = Path(a.saida); pasta.mkdir(exist_ok=True)
    arq = pasta / f"serial_{dt.datetime.now():%Y%m%d_%H%M%S}.csv"
    pend = pasta / "pendentes.jsonl"
    with arq.open("w", newline="", encoding="utf-8") as f, serial.Serial(a.porta, a.baud, timeout=2) as porta:
        w = csv.writer(f)
        print("Gravando em", arq, "(Ctrl+C para parar)")
        try:
            while True:
                linha = porta.readline().decode("utf-8", errors="ignore").strip()
                if not linha:
                    continue
                print(linha)
                partes = linha.split(",")
                w.writerow([dt.datetime.now().isoformat(timespec="seconds")] + partes); f.flush()
                if partes[0] == "E" and len(partes) >= 5 and a.url:       # evento: E,t,TIPO,dist,luz
                    corpo = montar_json(a.rota, partes[2], partes[3], partes[4])
                    try:
                        if not enviar(a.url, corpo):
                            raise RuntimeError("API recusou")
                    except Exception as e:                                # rede/API fora: guarda e segue
                        with pend.open("a", encoding="utf-8") as p:
                            p.write(json.dumps(corpo, ensure_ascii=False) + "\n")
                        print("  (fila local: ", e.__class__.__name__, ")", sep="")
        except KeyboardInterrupt:
            print("Encerrado.")


if __name__ == "__main__":
    main()
