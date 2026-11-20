#!/usr/bin/env bash
# Roda no PC (precisa de g++): 1) logica de decisao  2) firmware completo (3 LEDs)  3) firmware minimo (LED da placa)
set -e
cd "$(dirname "$0")"
echo "== 1) Logica de decisao =="
g++ -std=c++17 -Wall -Wextra -o /tmp/test_logica test_logica.cpp && /tmp/test_logica
echo; echo "== 2) Firmware simulado (modo 3 LEDs) =="
g++ -std=c++17 -Wall -Wextra -Wno-unused-function -Imock -o /tmp/test_firmware_sim test_firmware_sim.cpp && /tmp/test_firmware_sim
echo; echo "== 3) Firmware simulado (versao minima: LED da placa) =="
g++ -std=c++17 -Wall -Wextra -Wno-unused-function -Imock -o /tmp/test_firmware_minimo test_firmware_minimo.cpp && /tmp/test_firmware_minimo
