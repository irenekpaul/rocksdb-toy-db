#!/bin/bash

DB=./build/toydb
LOAD_SIZES=(10000 14444 18888 23333 27777 32222 36666 41111 45555 50000)

mkdir -p perf_logs

WAL_CSV="perf_logs/wal_size.csv"
RECOVERY_CSV="perf_logs/recovery_latency.csv"
RANGE_CSV="perf_logs/range_latency.csv"
THROUGHPUT_CSV="perf_logs/throughput.csv"

echo "load_size,wal_size_bytes" > "$WAL_CSV"
echo "load_size,recovery_time_ms" > "$RECOVERY_CSV"
echo "load_size,range_time_ms" > "$RANGE_CSV"
echo "load_size,workload_type,ops_per_sec" > "$THROUGHPUT_CSV"

now_ms() {
  python3 -c 'import time; print(int(time.time() * 1000))'
}

for N in "${LOAD_SIZES[@]}"; do
  echo -e "\n=== Load Size: $N ==="

  echo "reset" > temp_input.txt
  for ((i = 0; i < N; i++)); do
    echo "put key$i val$i" >> temp_input.txt
  done
  echo "exit" >> temp_input.txt

  START=$(now_ms)
  $DB < temp_input.txt > /dev/null
  END=$(now_ms)

  WAL_SIZE=$(stat -f%z data/wal.log 2>/dev/null || echo 0)
  echo "$N,$WAL_SIZE" >> "$WAL_CSV"

  START=$(now_ms)
  echo -e "recover\nexit" | $DB > /dev/null
  END=$(now_ms)
  RECOVERY_TIME=$((END - START))
  echo "$N,$RECOVERY_TIME" >> "$RECOVERY_CSV"

  echo -e "recover\nflush\nget_range key0 key$((N-1))\nexit" > temp_input.txt
  START=$(now_ms)
  $DB < temp_input.txt > /dev/null
  END=$(now_ms)
  RANGE_TIME=$((END - START))
  echo "$N,$RANGE_TIME" >> "$RANGE_CSV"

  for TYPE in write-heavy read-heavy balanced; do
    echo "reset" > temp_input.txt
    for ((i = 0; i < N; i++)); do
      case $TYPE in
        write-heavy)
          echo "put k$i v$i" >> temp_input.txt
          ;;
        read-heavy)
          echo "put k$i v$i" >> temp_input.txt
          echo "get k$i" >> temp_input.txt
          ;;
        balanced)
          if ((i % 2 == 0)); then
            echo "put k$i v$i" >> temp_input.txt
          else
            echo "get k$((i - 1))" >> temp_input.txt
          fi
          ;;
      esac
    done
    echo "exit" >> temp_input.txt

    START=$(now_ms)
    $DB < temp_input.txt > /dev/null
    END=$(now_ms)
    DURATION=$((END - START))

    if [ "$TYPE" == "read-heavy" ]; then
      TOTAL_OPS=$((N * 2))
    else
      TOTAL_OPS=$N
    fi

    if [ "$DURATION" -gt 0 ]; then
      OPS_PER_SEC=$(echo "scale=2; 1000 * $TOTAL_OPS / $DURATION" | bc)
    else
      OPS_PER_SEC="N/A"
    fi

    echo "$N,$TYPE,$OPS_PER_SEC" >> "$THROUGHPUT_CSV"
  done
done

rm -f temp_input.txt
echo -e "\nPerformance test completed. CSVs saved to perf_logs/"

FLUSH_CSV="perf_logs/range_flush_threshold.csv"
echo "flush_threshold,range_time_ms" > $FLUSH_CSV

for THRESHOLD in 1000 3000 5000 7000 10000; do
  echo "reset" > temp_input.txt
  for ((i = 0; i < 20000; i++)); do
    echo "put k$i v$i" >> temp_input.txt
    if (( (i + 1) % THRESHOLD == 0 )); then
      echo "flush" >> temp_input.txt
    fi
  done
  echo "get_range k0 k19999" >> temp_input.txt
  echo "exit" >> temp_input.txt

  START=$(now_ms)
  $DB < temp_input.txt > /dev/null
  END=$(now_ms)
  RANGE_MS=$((END - START))
  echo "$THRESHOLD,$RANGE_MS" >> $FLUSH_CSV
done

COMPACT_CSV="perf_logs/compaction_range_latency.csv"
echo "load_size,compaction,range_time_ms" > $COMPACT_CSV

for COMPACT in "true" "false"; do
  for SIZE in 10000 20000 30000 40000; do
    echo "reset" > temp_input.txt
    for ((i = 0; i < SIZE; i++)); do
      echo "put x$i v$i" >> temp_input.txt
    done
    echo "flush" >> temp_input.txt
    if [ "$COMPACT" = "true" ]; then
      echo "compact_level 0" >> temp_input.txt
    fi
    echo "get_range x0 x$((SIZE - 1))" >> temp_input.txt
    echo "exit" >> temp_input.txt

    START=$(now_ms)
    $DB < temp_input.txt > /dev/null
    END=$(now_ms)
    LATENCY=$((END - START))
    echo "$SIZE,$COMPACT,$LATENCY" >> $COMPACT_CSV
  done
done