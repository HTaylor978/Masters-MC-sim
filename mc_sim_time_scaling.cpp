#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <vector>

// Constants
const long double SPEED_OF_LIGHT{3e8L};                          // m/s
const long double BOLTZMANN_CONSTANT{1.380649e-23L};             // J/K
const long double HYDROGEN_MASS{1.673e-27L};                     // kg
const long double PERMEABILITY_OF_FREE_SPACE{4 * M_PI * 1e-7L};  // m kg s^-2 A^-2
const long double POLARIZABILITY{4.5L * pow(5.29177e-11L, 3)};   // m^3
const long double WAVELENGTH{1300e-9L};                          // m
const long double WAVENUMBER{2 * M_PI / WAVELENGTH};             // m^-1

// Parameters
const int MAX_PASSES{1000000};
const long double ENERGY_THRESHOLD{2e-3L};   // K
const long double POWER{10};                 // W
const long double TEMPERATURE{1e-3L};        // K
const long double BEAM_WAIST{100e-6L};       // m
const int NUM_PARTICLES{250};
const int MAX_KICKS_RECORDED{1000000};

// Function to assign random velocity components
void assign_random_velocities(long double temperature, long double& v_x, long double& v_y, long double& v_z, std::mt19937& rng) {
    long double v_squared = 3 * (BOLTZMANN_CONSTANT * temperature) / HYDROGEN_MASS;
    std::uniform_real_distribution<long double> dist(0.0L, 1.0L);
    long double r1 = dist(rng), r2 = dist(rng), r3 = dist(rng);
    long double norm = sqrt(r1 * r1 + r2 * r2 + r3 * r3);
    v_x = sqrt(v_squared) * (r1 / norm);
    v_y = sqrt(v_squared) * (r2 / norm);
    v_z = sqrt(v_squared) * (r3 / norm);
}

// Monte Carlo simulation function
long double monte_carlo_simulation(long double t_surf, std::mt19937& rng, int particle_id, std::vector<int>& pass_counts, std::vector<int>& kick_counts, std::vector<long double>& energy_kicks_recorded) {
    long double temperature = TEMPERATURE;
    long double elapsed_time = 0.0L;
    int passes = 0;
    int kicks = 0;
    std::uniform_real_distribution<long double> phase_dist(0.0L, 2.0L * M_PI);

    while (temperature < ENERGY_THRESHOLD) {
        if (passes > MAX_PASSES) {
            std::cout << "Particle " << particle_id << " failed to escape.\n";
            return -1;
        }

        long double v_x, v_y, v_z;
        assign_random_velocities(temperature, v_x, v_y, v_z, rng);
        long double z_kinetic_energy = 0.5L * HYDROGEN_MASS * v_z * v_z;
        long double v_r = sqrt(v_x * v_x + v_y * v_y);
        long double t_cross = BEAM_WAIST / v_r;
        int n_kicks = std::max(static_cast<int>(std::round(t_cross / t_surf)), 1);

        long double phi = 0.0L;
        long double kick = 0.0L;
        long double delta_z_energy = 0.0L;

        for (int n = 1; n < n_kicks + 1; n++) {
            phi = phase_dist(rng);
            kick = 0.5e-6L * BOLTZMANN_CONSTANT * (
                cos(2 * WAVENUMBER * v_z * (elapsed_time + n * t_surf) + phi) -
                cos(2.0L * WAVENUMBER * v_z * (elapsed_time + (n - 1) * t_surf) + phi)
            );
            delta_z_energy += kick;

            // Store absolute value of the kick if limit not reached
            if (energy_kicks_recorded.size() < MAX_KICKS_RECORDED) {
                energy_kicks_recorded.push_back(std::abs(kick) * 1e6L / BOLTZMANN_CONSTANT); // in µK
            }

        }

        z_kinetic_energy = std::max(z_kinetic_energy + delta_z_energy, -(z_kinetic_energy + delta_z_energy));
        long double v_z_squared = (2.0L * z_kinetic_energy) / HYDROGEN_MASS;
        temperature = (HYDROGEN_MASS * (v_x * v_x + v_y * v_y + v_z_squared)) / (3.0L * BOLTZMANN_CONSTANT);
        elapsed_time += t_cross;
        passes++;
        kicks += n_kicks;
    }

    std::cout << "Particle " << particle_id << " escaped in " << elapsed_time << " seconds in " << passes << " passes (" << kicks << " Kicks)" << std::endl;

    // Store pass and kick counts for statistics
    pass_counts.push_back(passes);
    kick_counts.push_back(kicks);

    return elapsed_time;
}

int main() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::ofstream file("simulation_results.csv");
    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return 1;
    }

    file << "tau_test, avg_time, std_error_time, avg_passes, std_error_passes, avg_kicks, std_error_kicks, avg_energy_per_kick_µK, std_error_energy_per_kick_µK\n";

    long double speed = 2.87L;
    long double tau = WAVELENGTH / (2.0L * speed);

    std::vector<long double> factors = {100, 50, 10, 3, 2, 1, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
    for (long double factor : factors) {
        long double tau_test = tau * factor;
        std::vector<long double> escape_times;
        std::vector<int> pass_counts;
        std::vector<int> kick_counts;
        std::vector<long double> energy_kicks_recorded;
        int failed_particles = 0;

        for (int i = 0; i < NUM_PARTICLES; i++) {
            long double time_to_escape = monte_carlo_simulation(tau_test, rng, i + 1, pass_counts, kick_counts, energy_kicks_recorded);
            if (time_to_escape > 0) {
                escape_times.push_back(time_to_escape);
            } else {
                failed_particles++;
            }
        }

        int successful_particles = escape_times.size();
        if (successful_particles > 0) {
            // Average escape time
            long double avg_time = 0.0L;
            for (long double t : escape_times) avg_time += t;
            avg_time /= successful_particles;

            long double variance_time = 0.0L;
            for (long double t : escape_times) variance_time += (t - avg_time) * (t - avg_time);
            long double std_error_time = sqrt(variance_time / successful_particles);

            // Average passes
            long double avg_passes = 0.0L;
            for (int p : pass_counts) avg_passes += p;
            avg_passes /= successful_particles;

            long double variance_passes = 0.0L;
            for (int p : pass_counts) variance_passes += (p - avg_passes) * (p - avg_passes);
            long double std_error_passes = sqrt(variance_passes / successful_particles);

            // Average kicks
            long double avg_kicks = 0.0L;
            for (int k : kick_counts) avg_kicks += k;
            avg_kicks /= successful_particles;

            long double variance_kicks = 0.0L;
            for (int k : kick_counts) variance_kicks += (k - avg_kicks) * (k - avg_kicks);
            long double std_error_kicks = sqrt(variance_kicks / successful_particles);

            // Average energy per kick
            long double avg_energy_kick = 0.0L;
            for (long double e : energy_kicks_recorded) avg_energy_kick += e;
            avg_energy_kick /= energy_kicks_recorded.size();

            long double variance_energy_kick = 0.0L;
            for (long double e : energy_kicks_recorded) variance_energy_kick += (e - avg_energy_kick) * (e - avg_energy_kick);
            long double std_error_energy_kick = sqrt(variance_energy_kick / energy_kicks_recorded.size());

            // Write to file
            file << tau_test << "," << avg_time << "," << std_error_time << "," << avg_passes << "," << std_error_passes << "," << avg_kicks << "," << std_error_kicks << "," << avg_energy_kick << "," << std_error_energy_kick << "\n";

            std::cout << "[T = t_surf * " << factor << "] Avg energy per kick: "
          << avg_energy_kick << " +/- " << std_error_energy_kick << " µK\n";

        }
    }

    file.close();
    std::cout << "Results saved to simulation_results.csv\n";
    return 0;
}
