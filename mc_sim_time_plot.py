import numpy as np
import matplotlib.pyplot as plt

# Load data from CSV file
data = np.loadtxt("simulation_results.csv", delimiter=",", skiprows=1)

# Extract columns
tau = data[:, 0]
avg_time = data[:, 1]
std_err_time = data[:, 2]
avg_passes = data[:, 3]
std_err_passes = data[:, 4]
avg_kicks = data[:, 5]
std_err_kicks = data[:, 6]
avg_energy_per_kick = data[:, 7]
std_err_energy_per_kick = data[:, 8]

# --- Plot 1: Escape Time vs Tau ---
plt.figure(figsize=(8, 6))
plt.errorbar(tau, avg_time, yerr=std_err_time,
             fmt='o', capsize=5, label="Escape Time")
plt.xlabel("Switching Period (s)")
plt.ylabel("Average Escape Time (s)")
plt.title("Escape Time vs Switching Period")
plt.xscale("log")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

# --- Plot 2: Passes vs Tau ---
plt.figure(figsize=(8, 6))
plt.errorbar(tau, avg_passes, yerr=std_err_passes, fmt='o',
             capsize=5, color='orange', label="Passes")
plt.xlabel("Switching Period (s)")
plt.ylabel("Average Passes")
plt.title("Passes vs Switching Period")
plt.xscale("log")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

# --- Plot 3: Kicks vs Tau ---
plt.figure(figsize=(8, 6))
plt.errorbar(tau, avg_kicks, yerr=std_err_kicks, fmt='o',
             capsize=5, color='green', label="Kicks")
plt.xlabel("Switching Period (s)")
plt.ylabel("Average Kicks")
plt.title("Kicks vs Switching Period")
plt.xscale("log")
plt.yscale("log")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

# --- Plot 4: Energy per Kick vs Tau ---
plt.figure(figsize=(8, 6))
plt.errorbar(tau, avg_energy_per_kick, yerr=std_err_energy_per_kick,
             fmt='o', capsize=5, color='red', label="Energy per Kick")
plt.xlabel("Switching Period (s)")
plt.ylabel("Average Energy per Kick (J)")
plt.title("Energy per Kick vs Switching Period")
plt.xscale("log")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
