#include <iostream>
#include <fstream> 
#include <random>
#include <vector>

// Parameters for the simulation
int N_PARTICLES = 10000;
int E_THRESHOLD = 1000;
int MAX_PASSES = 5000000;


// Function to run the Monte Carlo simulation
int monte_carlo_simulation(int energy_threshold, std::mt19937& rng) {
  // Initialise variables
  int energy = 0;
  int passes = 0;
  
  // Monte Carlo simulation loop
  while (energy < energy_threshold) {
    // If number of passes is > 5 * standard deviation -> return
    if (passes > MAX_PASSES) {
      return -1;
    }

    // Increment passes
    passes++;
    
    // Apply kick to particle
    std::uniform_int_distribution<int> kick_distribution(0, 1);
    int kick = (kick_distribution(rng) == 0) ? -1 : 1;
    energy += kick;
  }

  return passes;
}


// Function to run several simulations
void run_simulation(int n_particles) {
  // Create a random number generator (Mersenne Twister engine)
  std::random_device rd;
  std::mt19937 rng(rd());

  // Create vector of valid passes (below MAX_PASSES)
  std::vector<int> valid_passes;

  for (int i=0; i<n_particles; i++) {
    int trial = monte_carlo_simulation(E_THRESHOLD, rng);

    // Only include trials that didn't exceed MAX_PASSES
    if (trial != -1) {
      valid_passes.push_back(trial);
    }

    std::cout << "Simulation " << i + 1 << ": ";
    if (trial != -1) {
      std::cout << trial << " passes" << std::endl;
    } else {
      std::cout << "Exceeded max passes (" << MAX_PASSES << ")" << std::endl;
    }
  }

  // Save valid passes to a CSV file
  std::ofstream file("valid_passes.csv");
  if (file.is_open()) {
      for (int passes : valid_passes) {
          file << passes << "\n";  // Write each pass count to a new line (for CSV, separate by commas if needed)
      }
      file.close();
      std::cout << "Pass counts successfully saved to 'valid_passes.csv'" << std::endl;
  } else {
      std::cerr << "Error: Unable to open file for writing." << std::endl;
  }

  // Calculate the average of valid passes
  if (!valid_passes.empty()) {
    long long total_passes = 0;
    for (int passes : valid_passes) {
      total_passes += passes;
    }
    double average_passes = static_cast<double>(total_passes) / valid_passes.size();

    std::cout << "______________" << std::endl;
    std::cout << "Average number of passes for valid trials: " << average_passes << std::endl;
  } else {
    std::cout << "No valid trials (all exceeded MAX_PASSES)" << std::endl;
  }
}


int main() {
  // Run several simulations
  run_simulation(N_PARTICLES);
  return 0;
}
