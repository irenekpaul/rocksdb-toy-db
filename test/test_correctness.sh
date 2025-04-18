#!/bin/bash

DB=./build/toydb
PASSED=0
FAILED=0


test_case() {
  DESC="$1"
  INPUT="$2"
  EXPECT="$3"

  OUTPUT=$(echo -e "$INPUT" | $DB | grep -v '\[DEBUG\]' | grep -v '\[OK\]')

  if echo "$OUTPUT" | grep -q "$EXPECT"; then
    echo "[PASS] $DESC"
    PASSED=$((PASSED+1))
  else
    echo "FAIL: $DESC"
    echo "  Expected: $EXPECT"
    echo "  Got:      $OUTPUT"
    FAILED=$((FAILED+1))
  fi
}

mkdir -p data
rm -f data/*

test_case "Basic put/get" "put a 1\nget a\nexit" "1"

test_case "Overwrite value" "put a 1\nput a 2\nget a\nexit" "2"

test_case "Delete key" "put a 5\ndel a\nget a\nexit" "NULL"

test_case "Flush and retrieve" "put b 10\nflush\nget b\nexit" "10"


echo -e "put x 100\nflush\nexit" | $DB

test_case "Recover after flush" "recover\nget x\nexit" "100"


echo -e "put walkey 77\nexit" | $DB

test_case "Recover from WAL" "recover\nget walkey\nexit" "77"


echo -e "put f 12\nflush\nexit" | $DB

test_case "No WAL reuse after flush" "recover\nget f\nexit" "12"


echo -e "put a1 1\nput a2 2\nput a3 3\nflush\nexit" | $DB

test_case "Recover multiple keys" "recover\nget a2\nexit" "2"


test_case "Range query a-c" "put a 1\nput b 2\nput c 3\nflush\nget_range a c\nexit" "a=1"


echo -e "put e 5\nflush\nput f 6\nflush\nexit" | $DB

test_case "Recovery after multiple flushes" "recover\nget f\nexit" "6"


echo -e "put z1 100\nflush\nput z2 200\nexit" | $DB

test_case "Recover from SST + WAL" "recover\nget z1\nget z2\nexit" "200"


echo -e "put gone 9\ndel gone\nflush\nexit" | $DB

test_case "Delete persists after flush" "recover\nget gone\nexit" "NULL"


test_case "Overwrite after delete" "put m 1\ndel m\nput m 2\nget m\nexit" "2"


echo -e "put p 7\nflush\nreset\nexit" | $DB

test_case "Reset clears DB" "recover\nget p\nexit" "NULL"

echo ""
echo "Tests passed: $PASSED"
echo "Tests failed: $FAILED"

exit $FAILED
