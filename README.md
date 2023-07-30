# IMS Project - Queueing System for Tesco Online Orders

This project was developed as part of the course "Modeling and Simulation" (IMS) at FIT VUT Brno. The aim of this project was to model a queueing system for delivering online orders from the retail chain Tesco. The simulation was carried out to identify bottlenecks in the system and propose optimizations for better resource utilization and customer satisfaction.

## Table of Contents

- [Introduction](#introduction)
- [Authors](#authors)
- [Validity Verification](#validity-verification)
- [Analysis of the System](#analysis-of-the-system)
- [Simulation Model Architecture](#simulation-model-architecture)
- [Simulation Experiments](#simulation-experiments)
- [Results and Conclusion](#results-and-conclusion)
- [Dependencies](#dependencies)
- [How to Use](#how-to-use)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Introduction

In this project, we present a simulation model of the queueing system for Tesco's online order delivery process. The purpose of this model is to analyze the current system, identify potential areas of improvement, and propose optimizations to enhance the overall efficiency.

## Authors

- Pavol Babjak (xbabja03)
- Lukáš Václavek (xvacla32)

## Validity Verification

To validate the model, we followed these steps:

1. Conducted interviews with 2 employees from the courier company responsible for deliveries.
2. Collected data on actual orders.
3. Developed the simulation model based on the collected data.
4. Ran simulations using the model and tested proposed optimizations.

The model was considered valid based on the results obtained.

## Analysis of the System

The delivery system consists of two main components:

1. Tesco: The retail chain that prepares and organizes online orders.
2. DoDo: The external courier company responsible for delivering the orders.

The order preparation process at Tesco involves several steps, such as product picking from the shelves, sorting, and packing. The orders are then handed over to DoDo for delivery.

## Simulation Model Architecture

The simulation model was implemented using C/C++ programming language. Standard libraries were used for the implementation.

## Simulation Experiments

The simulation experiments were carried out to analyze the system under different scenarios. We simulated the order delivery process, evaluated performance metrics, and proposed various optimizations.

## Results and Conclusion

Based on the simulation experiments, we drew the following conclusions:

- Identified bottleneck areas in the delivery process.
- Proposed optimizations to enhance resource utilization.
- Analyzed the impact of different scenarios on the system.

## Dependencies

The simulation model relies on the following dependencies:

- C/C++ standard libraries

## How to Use

To run the simulation, follow these steps:

1. Compile the source code using `make`.
2. Execute the program using `make run`.

The simulation will output the results of the experiments and the performance metrics.

## Contributing

Contributions to this project are welcome. If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

We would like to express our gratitude to the employees of the courier company who provided valuable information for the simulation model.

