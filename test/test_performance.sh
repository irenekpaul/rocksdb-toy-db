#!/bin/bash

set -e


echo "[PERF] Measuring single write latency..."
cat > temp_input.txt <<EOF
reset
exit
EOF
../build/toydb < temp_input.txt

START=$(python3 -c 'import time; print(int(time.time() * 1000))')
../build/toydb <<EOF
put key_perf test
exit
EOF
END=$(python3 -c 'import time; print(int(time.time() * 1000))')

ELAPSED=$(($END - $START))
echo "[PERF] Single put took ${ELAPSED} ms"

N=10000

echo "[PERF] Writing $N keys for WAL recovery test..."
echo "reset" > temp_input.txt
for i in $(seq 1 $N); do
  echo "put key_$i val_$i" >> temp_input.txt
done
echo "exit" >> temp_input.txt
../build/toydb < temp_input.txt

echo "[PERF] Measuring recovery time..."
START=$(python3 -c 'import time; print(int(time.time() * 1000))')
../build/toydb <<EOF
recover
exit
EOF
END=$(python3 -c 'import time; print(int(time.time() * 1000))')

ELAPSED=$(($END - $START))
echo "[PERF] Recovery from WAL took ${ELAPSED} ms for $N keys"


echo "[PERF] Flushing and deleting WAL..."
echo "flush
exit" > temp_input.txt
../build/toydb < temp_input.txt
rm -f ../data/wal.log

echo "[PERF] Measuring recovery time after flush..."
START=$(python3 -c 'import time; print(int(time.time() * 1000))')
../build/toydb <<EOF
recover
exit
EOF
END=$(python3 -c 'import time; print(int(time.time() * 1000))')

ELAPSED=$(($END - $START))
echo "[PERF] Recovery after flush took ${ELAPSED} ms"

rm -f temp_input.txt
