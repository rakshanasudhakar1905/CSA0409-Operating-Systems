#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p evidence
CC=${CC:-gcc}
"$CC" -std=c11 -O2 -Wall -Wextra code/task6_copy_file.c -o code/task6_copy_file
"$CC" -std=c11 -O2 -Wall -Wextra code/task7_process_control.c -o code/task7_process_control
"$CC" -std=c11 -O2 -Wall -Wextra code/task10_process_thread_bench.c -pthread -o code/task10_process_thread_bench
{
  echo "=== Task 6: copy-file buffer experiment ==="
  (cd evidence && ../code/task6_copy_file)
  echo
  echo "=== Task 7: process control ==="
  ../code/task7_process_control
  echo
  echo "=== Task 10: process/thread benchmark ==="
  ../code/task10_process_thread_bench
} | tee evidence/run_output.txt
if command -v strace >/dev/null 2>&1; then
  echo "=== strace summary for task 6 (4096-byte read size is in program) ===" | tee evidence/strace_output.txt
  (cd evidence && strace -c ../code/task6_copy_file) 2>> evidence/strace_output.txt
else
  echo "strace is not installed. Install with: sudo apt install strace" > evidence/strace_output.txt
fi
