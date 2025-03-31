#!/bin/bash

set -e

echo "[TEST] Resetting..."
../build/toydb <<EOF
reset
exit
EOF

echo "[TEST 1] WAL Recovery"
../build/toydb <<EOF
put a 1
put b 2
exit
EOF

../build/toydb <<EOF
recover
get a
get b
exit
EOF


echo "[TEST 2] Delete and Recovery"
../build/toydb <<EOF
reset
put x 100
del x
exit
EOF

../build/toydb <<EOF
recover
get x
exit
EOF


echo "[TEST 3] Flush recovery"
../build/toydb <<EOF
reset
put a 10
flush
exit
EOF

rm -f data/wal.log

../build/toydb <<EOF
recover
get a
exit
EOF


echo "[TEST 4] Compaction logic"
../build/toydb <<EOF
reset
put k 1
flush
put k 2
flush
compact
get k
exit
EOF

echo "[ALL TESTS DONE]"
