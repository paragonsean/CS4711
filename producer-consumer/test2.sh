#!/usr/bin/env bash
set -euo pipefail

# file: run_grouped_to_csv.sh

# --- 1) Define your parameter sets ---
SLEEP_TIMES=(
  0.00001 0.00002 0.00003 0.00004 0.00005 0.00006 0.00007 0.00008 0.00009
  0.0001  0.0002  0.0003  0.0004  0.0005  0.0006  0.0007  0.0008  0.0009
  0.001   0.002   0.003   0.004   0.005   0.006   0.007   0.008   0.009
  0.01    0.02    0.03    0.04    0.05    0.06    0.07    0.08    0.09
  0.1     0.2     0.3     0.4     0.5     0.6     0.7     0.8     0.9
  1       2            4       8             16 
  32     64                
)

PRODUCERS=(1 4 16)
CONSUMERS=(1 2 4 16)

# --- 2) Build flat test-case arrays (Cartesian product of PRODUCERS×CONSUMERS) ---
declare -a producers   # will hold each test-case’s producer count
declare -a consumers   # parallel array for consumer count

for p in "${PRODUCERS[@]}"; do
  for c in "${CONSUMERS[@]}"; do
    producers+=("$p")
    consumers+=("$c")
  done
done

# Sanity check: must have 3×4 = 12 entries
if [[ ${#producers[@]} -ne ${#consumers[@]} ]]; then
  echo "Error: producers/consumers array length mismatch!" >&2
  exit 1
fi

# --- 3) Prepare output CSV ---
OUT="grouped_results.csv"
> "$OUT"
echo "test_case,sleep_s,producers,consumers,real_s,user_s,sys_s,produced,consumed" > "$OUT"

# --- 4) Loop over test cases, then over sleep times ---
total_runs=$(( ${#producers[@]} * ${#SLEEP_TIMES[@]} ))
current_run=0

echo "Starting $total_runs runs; results → $OUT"

for idx in "${!producers[@]}"; do
  tc=$(( idx + 1 ))        # test_case index: 1..12
  p=${producers[$idx]}     # producer count for this test_case
  c=${consumers[$idx]}     # consumer count

  for t in "${SLEEP_TIMES[@]}"; do
    current_run=$((current_run + 1))
    echo "Run $current_run/$total_runs → tc=$tc (P=$p, C=$c), sleep=$t s"

    # run & capture output + timing
    prog_out=$(mktemp)
    time_out=$(mktemp)

    { time -p ./producer_consumer "$t" "$p" "$c" >"$prog_out"; } 2>"$time_out"
    exit_status=$?

    if [[ $exit_status -ne 0 ]]; then
      echo "  ⚠️ Exit $exit_status; skipping parsing for tc=$tc, t=$t" >&2
    else
      # parse counts
      produced=$(awk '/Total items produced:/  {print $4}' "$prog_out")
      consumed=$(awk '/Total items consumed:/  {print $4}' "$prog_out")

      # parse timings
      real=$(awk '/^real/ {print $2}' "$time_out")
      user=$(awk '/^user/ {print $2}' "$time_out")
      sys=$(awk '/^sys/  {print $2}' "$time_out")

      # append CSV row
      echo "${tc},${t},${p},${c},${real},${user},${sys},${produced},${consumed}" >> "$OUT"
    fi

    rm -f "$prog_out" "$time_out"
  done
done

echo "Done: results saved to $OUT"
