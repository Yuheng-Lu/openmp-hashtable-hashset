#!/bin/bash

# Ensure the script is run with KEY_T and size as arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <KEY_T> <size> <num_thread>"
    exit 1
fi

KEY_T=$1
SIZE=$2
NUM_THREAD=$3

# Clean and build with the specified KEY_T
make clean
make KEY_T="$KEY_T"

# Initialize accumulators
total_parallel_insert=0
total_serial_insert=0
total_parallel_lookup=0
total_serial_lookup=0
total_parallel_delete=0
total_serial_delete=0

# Run the benchmark 5 times
for i in {1..5}; do
    echo "Run #$i"
    output=$(./benchmark "$SIZE" "$NUM_THREAD")

    # Extract relevant values using regex
    parallel_insert=$(echo "$output" | grep "Parallel Insert Time" | awk '{print $4}')
    serial_insert=$(echo "$output" | grep "Serial Insert Time" | awk '{print $4}')
    parallel_lookup=$(echo "$output" | grep "Parallel Lookup Time" | awk '{print $4}')
    serial_lookup=$(echo "$output" | grep "Serial Lookup Time" | awk '{print $4}')
    parallel_delete=$(echo "$output" | grep "Parallel Delete Time" | awk '{print $4}')
    serial_delete=$(echo "$output" | grep "Serial Delete Time" | awk '{print $4}')

    # Accumulate times
    total_parallel_insert=$(echo "$total_parallel_insert + $parallel_insert" | bc)
    total_serial_insert=$(echo "$total_serial_insert + $serial_insert" | bc)
    total_parallel_lookup=$(echo "$total_parallel_lookup + $parallel_lookup" | bc)
    total_serial_lookup=$(echo "$total_serial_lookup + $serial_lookup" | bc)
    total_parallel_delete=$(echo "$total_parallel_delete + $parallel_delete" | bc)
    total_serial_delete=$(echo "$total_serial_delete + $serial_delete" | bc)
done

# Calculate averages
avg_parallel_insert=$(echo "$total_parallel_insert / 5" | bc -l)
avg_serial_insert=$(echo "$total_serial_insert / 5" | bc -l)
avg_parallel_lookup=$(echo "$total_parallel_lookup / 5" | bc -l)
avg_serial_lookup=$(echo "$total_serial_lookup / 5" | bc -l)
avg_parallel_delete=$(echo "$total_parallel_delete / 5" | bc -l)
avg_serial_delete=$(echo "$total_serial_delete / 5" | bc -l)

# Calculate speedups
speedup_insert=$(echo "$avg_serial_insert / $avg_parallel_insert" | bc -l)
speedup_lookup=$(echo "$avg_serial_lookup / $avg_parallel_lookup" | bc -l)
speedup_delete=$(echo "$avg_serial_delete / $avg_parallel_delete" | bc -l)

# Display results
echo "Average Performance Summary:"
echo "----------------------------"
printf "Insert - Parallel: %.6f s | Serial: %.6f s | Speedup: %.2fx\n" "$avg_parallel_insert" "$avg_serial_insert" "$speedup_insert"
printf "Lookup - Parallel: %.6f s | Serial: %.6f s | Speedup: %.2fx\n" "$avg_parallel_lookup" "$avg_serial_lookup" "$speedup_lookup"
printf "Delete - Parallel: %.6f s | Serial: %.6f s | Speedup: %.2fx\n" "$avg_parallel_delete" "$avg_serial_delete" "$speedup_delete"
