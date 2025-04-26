import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("scalability_results.csv")

# Normalize time and compute speedup
speedup_data = {}
for bench in df['benchmark'].unique():
    sub = df[df['benchmark'] == bench].sort_values('threads')
    base_time = sub[sub['threads'] == 1]['time'].values[0]
    speedup = base_time / sub['time'].values
    speedup_data[bench] = (sub['threads'].values, speedup)

# Ideal speedup
threads = range(1, 9)
ideal = list(threads)

# Plotting
plt.figure(figsize=(6, 4))
for bench, (x, y) in speedup_data.items():
    plt.plot(x, y, marker='o', label=bench.capitalize())

plt.plot(threads, ideal, linestyle='--', color='black', label='Ideal')

plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Scalability of Genetic Benchmark")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("scalability_plot.pdf")
