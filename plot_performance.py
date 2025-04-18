import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

sns.set(style="whitegrid")


os.makedirs("perf_plots", exist_ok=True)

df_wal = pd.read_csv("perf_logs/wal_size.csv")
plt.figure()
sns.lineplot(data=df_wal, x="load_size", y="wal_size_bytes", marker="o")
plt.title("WAL Size vs Load Size")
plt.xlabel("Number of Operations")
plt.ylabel("WAL Size (bytes)")
plt.savefig("perf_plots/wal_size.png")


df_recovery = pd.read_csv("perf_logs/recovery_latency.csv")
plt.figure()
sns.lineplot(data=df_recovery, x="load_size", y="recovery_time_ms", marker="o", color="green")
plt.title("Recovery Latency vs Load Size")
plt.xlabel("Number of Operations")
plt.ylabel("Recovery Time (ms)")
plt.savefig("perf_plots/recovery_latency.png")

df_range = pd.read_csv("perf_logs/range_latency.csv")
plt.figure()
sns.lineplot(data=df_range, x="load_size", y="range_time_ms", marker="o", color="orange")
plt.title("Range Query Latency vs Load Size")
plt.xlabel("Number of Keys")
plt.ylabel("Range Query Time (ms)")
plt.savefig("perf_plots/range_latency.png")

df_throughput = pd.read_csv("perf_logs/throughput.csv")
plt.figure()
sns.lineplot(data=df_throughput, x="load_size", y="ops_per_sec", hue="workload_type", marker="o")
plt.title("Throughput vs Load Size by Workload Type")
plt.xlabel("Number of Operations")
plt.ylabel("Operations per Second")
plt.legend(title="Workload Type")
plt.savefig("perf_plots/throughput.png")

# Plot: Flush Threshold vs Range Latency
df_flush = pd.read_csv("perf_logs/range_flush_threshold.csv")
plt.figure()
sns.lineplot(data=df_flush, x="flush_threshold", y="range_time_ms", marker="o", color="purple")
plt.title("Flush Threshold vs Range Query Latency")
plt.xlabel("Flush Threshold (# keys before flush)")
plt.ylabel("Range Query Time (ms)")
plt.savefig("perf_plots/range_flush_threshold.png")

# Plot: Compaction vs No Compaction Range Latency
df_compaction = pd.read_csv("perf_logs/compaction_range_latency.csv")
plt.figure()
sns.lineplot(data=df_compaction, x="load_size", y="range_time_ms", hue="compaction", marker="o")
plt.title("Range Latency: With vs Without Compaction")
plt.xlabel("Number of Keys")
plt.ylabel("Range Query Time (ms)")
plt.legend(title="Compaction")
plt.savefig("perf_plots/compaction_range_latency.png")


