import subprocess
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument("--workload", type=str, required=True)
parser.add_argument("--db", type=str, default="./build/toydb")
parser.add_argument("--mode", type=str, choices=["load", "run"], default="run")
args = parser.parse_args()

def parse_line(line):
    parts = line.strip().split()
    if not parts:
        return None
    op = parts[0]
    if op in {"INSERT", "UPDATE"} and len(parts) >= 3:
        key = parts[2]
        record = line.split("[", 1)[-1].strip(" ]")
        return f'put {key} "{record}"'
    elif op == "READ" and len(parts) >= 3:
        return f'get {parts[2]}'
    elif op == "SCAN" and len(parts) >= 3:
        return f'get_range {parts[2]} z'
    return None

with open(args.workload) as f:
    ops = [parse_line(line) for line in f if parse_line(line)]

if not ops:
    print("[ERROR] No valid operations parsed.")
    exit(1)

input_lines = []
if args.mode == "run":
    input_lines.append("recover")
input_lines.extend(ops)
if args.mode == "load":
    input_lines.append("flush")
input_lines.append("exit")

with open("temp_ycsb_input.txt", "w") as f:
    f.write("\n".join(input_lines))

start = time.time()
proc = subprocess.run(f"{args.db} < temp_ycsb_input.txt", shell=True, capture_output=True, text=True)
end = time.time()

output = proc.stdout.strip().split("\n")
success = sum(1 for line in output if "[OK]" in line)

print("\n=== YCSB Runner Report ===")
print(f"Total ops: {len(ops)}")
print(f"Successful ops: {success}")
print(f"Average latency: {(end - start) * 1000 / len(ops):.2f} ms")
print(f"Total time: {end - start:.2f} s")

print("\n[Sample output]")
for line in output[:10]:
    print(line)
