#!/bin/bash

set -e

DB_PATH=../build/toydb
PYTHON_RUNNER=python3
ycsb_runner=ycsb_runner.py

mkdir -p logs

for workload in a b c d e f; do
  echo "[+] Generating workload$workload ops..."
  # ./bin/ycsb load basic -P workloads/workload$workload -p basicdb.command=cat > ycsb_load_ops_$workload.txt
  # ./bin/ycsb run basic -P workloads/workload$workload -p basicdb.command=cat > ycsb_run_ops_$workload.txt

  echo "[+] Running workload$workload (load phase)..."
  $PYTHON_RUNNER $ycsb_runner --workload ycsb_load_ops_$workload.txt --db $DB_PATH --mode load > logs/load_wl${workload}.log

  echo "[+] Running workload$workload (run phase)..."
  $PYTHON_RUNNER $ycsb_runner --workload ycsb_run_ops_$workload.txt --db $DB_PATH --mode run > logs/run_wl${workload}.log

echo "[✓] Finished workload $workload"
done

echo "[✓] All workloads complete. Logs saved in ./logs/"
