import matplotlib.pyplot as plt
import pandas as pd

# Load your data
df = pd.read_csv("recovery_log.csv", names=["WAL_Size_Bytes", "Recovery_Time_µs"])

# Convert bytes to KB for readability
df["WAL_Size_KB"] = df["WAL_Size_Bytes"] / 1024

# Plot
plt.figure(figsize=(8, 5))
plt.plot(df["WAL_Size_KB"], df["Recovery_Time_µs"], marker='o')
plt.title("Recovery Time vs WAL Size (RocksDB Toy)")
plt.xlabel("WAL Size (KB)")
plt.ylabel("Recovery Time (µs)")
plt.grid(True)
plt.tight_layout()
plt.savefig("recovery_vs_wal.png")
plt.show()
