import numpy as np
import matplotlib.pyplot as plt

# Function to read pass counts from the CSV file
def read_passes_from_file(filename):
    # Read data from the CSV file
    with open(filename, 'r') as file:
        passes = [int(line.strip()) for line in file if line.strip().isdigit()]
    return np.array(passes)

# Function to plot histogram of pass counts
def plot_histogram(passes):
    # Calculate the mean and error on the mean
    mean_value = np.mean(passes)
    std_dev = np.std(passes, ddof=1)  # Use ddof=1 for sample standard deviation
    sem = std_dev / np.sqrt(len(passes))  # Standard error of the mean

    plt.figure(figsize=(12, 8))
    
    # Create histogram with edge color for borders
    n, bins, patches = plt.hist(passes, bins=100, color='skyblue', edgecolor='black', linewidth=1.2)
    
    # Add titles and labels
    plt.title('Distribution of Pass Counts in Monte Carlo Simulation')
    plt.xlabel('Number of Passes')
    plt.ylabel('Frequency')
    
    # Customize grid
    plt.grid()

    # Add mean value as a vertical line
    plt.axvline(mean_value, color='red', linestyle='--', linewidth=1.5, label=f'Mean: {mean_value:.3g} ± {sem:.3g}')
    
    # Annotate the mean value
    plt.annotate(f'Mean: {mean_value:.3g}\n± {sem:.3g}', 
                 xy=(mean_value, max(n) * 0.7),
                 xytext=(mean_value + mean_value * 0.1, max(n) * 0.8),
                 arrowprops=dict(facecolor='red', arrowstyle='->'),
                 fontsize=12, color='red')

    # Add legend for the mean line
    plt.legend()

    # Tight layout and save the figure
    plt.tight_layout()
    plt.savefig("enhanced_mc_plot_with_mean_and_error.png", dpi=300)
    plt.show()

if __name__ == "__main__":
    # Read the pass data from the CSV file
    filename = "valid_passes.csv"  # Ensure this file contains the pass counts
    passes = read_passes_from_file(filename)
    
    # Plot the histogram of pass counts
    plot_histogram(passes)
