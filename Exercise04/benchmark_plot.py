import subprocess
import re
import os
import matplotlib.pyplot as plt

# Processor counts to benchmark
processors = [1, 2, 4, 8]

def run_mpi_program(binary_path, np):
    """Runs the MPI executable and extracts execution time in seconds."""
    # Ensure binary path exists before invoking mpirun
    if not os.path.exists(binary_path):
        raise FileNotFoundError(f"Binary not found at path: {os.path.abspath(binary_path)}")

    # Command without hardcoded './' prefix
    cmd = ["mpirun", "-np", str(np), binary_path]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    output = result.stdout
    # Match pattern: 'Execution Time : 0.123456 seconds'
    match = re.search(r"Execution Time\s*:\s*([\d\.]+)", output)
    if match:
        return float(match.group(1))
    else:
        raise RuntimeError(
            f"Could not parse execution time from {binary_path} with np={np}.\n"
            f"STDOUT:\n{output}\n"
            f"STDERR:\n{result.stderr}"
        )

print("=== Running Benchmarks ===")

# Relative paths from Exercise04/ to sibling folders
ex2_binary = "../Exercise02/sum_mpi"
ex3_binary = "../Exercise03/pi_monte_carlo"

# Benchmark Exercise 2 (sum_mpi)
time_ex2 = []
print("Benchmarking Exercise 2 (sum_mpi)...")
for p in processors:
    t = run_mpi_program(ex2_binary, p)
    time_ex2.append(t)
    print(f"  Processes: {p} -> Time: {t:.6f} s")

# Benchmark Exercise 3 (pi_monte_carlo)
time_ex3 = []
print("\nBenchmarking Exercise 3 (pi_monte_carlo)...")
for p in processors:
    t = run_mpi_program(ex3_binary, p)
    time_ex3.append(t)
    print(f"  Processes: {p} -> Time: {t:.6f} s")

# Calculate dynamic speedup: S(p) = T(1) / T(p)
speedup_ex2 = [time_ex2[0] / t for t in time_ex2]
speedup_ex3 = [time_ex3[0] / t for t in time_ex3]
ideal_speedup = [float(p) for p in processors]

# --- Generate Single Combined PNG with 2 Subplots ---
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# Graph 1: Time vs Number of Processors
ax1.plot(processors, time_ex2, marker='o', color='tab:blue', linewidth=2, label='Ex 2: Array Sum')
ax1.plot(processors, time_ex3, marker='s', color='tab:red', linewidth=2, label='Ex 3: Monte Carlo Pi')
ax1.set_title('Time vs Number of Processors', fontsize=12, fontweight='bold')
ax1.set_xlabel('Number of Processors')
ax1.set_ylabel('Execution Time (seconds)')
ax1.set_xticks(processors)
ax1.grid(True, linestyle='--', alpha=0.6)
ax1.legend()

# Graph 2: Speedup vs Number of Processors
ax2.plot(processors, speedup_ex2, marker='o', color='tab:blue', linewidth=2, label='Ex 2 Speedup')
ax2.plot(processors, speedup_ex3, marker='s', color='tab:red', linewidth=2, label='Ex 3 Speedup')
ax2.plot(processors, ideal_speedup, linestyle='--', color='gray', label='Ideal Linear Speedup')
ax2.set_title('Speedup vs Number of Processors', fontsize=12, fontweight='bold')
ax2.set_xlabel('Number of Processors')
ax2.set_ylabel('Speedup (T1 / Tp)')
ax2.set_xticks(processors)
ax2.grid(True, linestyle='--', alpha=0.6)
ax2.legend()

plt.tight_layout()
plt.savefig('exercise4_comparison.png', dpi=300)
plt.close()

print("\nBenchmark finished successfully. Plot saved as 'exercise4_comparison.png'.")