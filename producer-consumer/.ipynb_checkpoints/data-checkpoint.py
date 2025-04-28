# Import necessary libraries
import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
file_path = '/mnt/data/results_timing_allnormal.csv'
data = pd.read_csv(file_path)

# Display the first few rows of the data to understand its structure
data.head()

# Create a new column that combines producers and consumers for analysis
data['prod_cons_combo'] = data['producers'].astype(str) + ' producers, ' + data['consumers'].astype(str) + ' consumers'

# Plotting throughput vs. sleep time, producers, and consumers
plt.figure(figsize=(18, 6))

# Throughput vs. Sleep Time
plt.subplot(1, 3, 1)
plt.scatter(data['sleep_s'], data['throughput'], c=data['sleep_s'], cmap='viridis', alpha=0.7)
plt.title('Throughput vs. Sleep Time')
plt.xlabel('Sleep Time (seconds)')
plt.ylabel('Throughput (items/sec)')

# Throughput vs. Producers
plt.subplot(1, 3, 2)
plt.scatter(data['producers'], data['throughput'], c=data['sleep_s'], cmap='viridis', alpha=0.7)
plt.title('Throughput vs. Number of Producers')
plt.xlabel('Number of Producers')
plt.ylabel('Throughput (items/sec)')

# Throughput vs. Consumers
plt.subplot(1, 3, 3)
plt.scatter(data['consumers'], data['throughput'], c=data['sleep_s'], cmap='viridis', alpha=0.7)
plt.title('Throughput vs. Number of Consumers')
plt.xlabel('Number of Consumers')
plt.ylabel('Throughput (items/sec)')

plt.tight_layout()
plt.show()

# Plotting elapsed time vs. sleep time, producers, and consumers
plt.figure(figsize=(18, 6))

# Elapsed Time vs. Sleep Time
plt.subplot(1, 3, 1)
plt.scatter(data['sleep_s'], data['elapsed_s'], c=data['sleep_s'], cmap='plasma', alpha=0.7)
plt.title('Elapsed Time vs. Sleep Time')
plt.xlabel('Sleep Time (seconds)')
plt.ylabel('Elapsed Time (seconds)')

# Elapsed Time vs. Producers
plt.subplot(1, 3, 2)
plt.scatter(data['producers'], data['elapsed_s'], c=data['sleep_s'], cmap='plasma', alpha=0.7)
plt.title('Elapsed Time vs. Number of Producers')
plt.xlabel('Number of Producers')
plt.ylabel('Elapsed Time (seconds)')

# Elapsed Time vs. Consumers
plt.subplot(1, 3, 3)
plt.scatter(data['consumers'], data['elapsed_s'], c=data['sleep_s'], cmap='plasma', alpha=0.7)
plt.title('Elapsed Time vs. Number of Consumers')
plt.xlabel('Number of Consumers')
plt.ylabel('Elapsed Time (seconds)')

plt.tight_layout()
plt.show()

# Create two pivot tables: one for throughput and one for elapsed time
pivot_throughput = data.pivot_table(values='throughput', index='producers', columns='consumers', aggfunc='max')
pivot_elapsed = data.pivot_table(values='elapsed_s', index='producers', columns='consumers', aggfunc='min')

# Plotting throughput and elapsed time heatmaps
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# Throughput Heatmap
cax1 = axes[0].matshow(pivot_throughput, cmap='viridis')
fig.colorbar(cax1, ax=axes[0])
axes[0].set_title('Throughput (items/sec) vs. Producers and Consumers')
axes[0].set_xlabel('Number of Consumers')
axes[0].set_ylabel('Number of Producers')
axes[0].set_xticks(range(len(pivot_throughput.columns)))
axes[0].set_yticks(range(len(pivot_throughput.index)))
axes[0].set_xticklabels(pivot_throughput.columns)
axes[0].set_yticklabels(pivot_throughput.index)

# Elapsed Time Heatmap
cax2 = axes[1].matshow(pivot_elapsed, cmap='plasma')
fig.colorbar(cax2, ax=axes[1])
axes[1].set_title('Elapsed Time (seconds) vs. Producers and Consumers')
axes[1].set_xlabel('Number of Consumers')
axes[1].set_ylabel('Number of Producers')
axes[1].set_xticks(range(len(pivot_elapsed.columns)))
axes[1].set_yticks(range(len(pivot_elapsed.index)))
axes[1].set_xticklabels(pivot_elapsed.columns)
axes[1].set_yticklabels(pivot_elapsed.index)

plt.tight_layout()
plt.show()

# Grouping the data by producer-consumer combinations to find the best combo for throughput and elapsed time
best_combo = data.groupby('prod_cons_combo').agg({
    'throughput': 'max',  # Max throughput for each combination
    'elapsed_s': 'min'    # Min elapsed time for each combination
}).reset_index()

# Sorting to find the best performing combo for both throughput and elapsed time
best_combo_sorted = best_combo.sort_values(by=['throughput', 'elapsed_s'], ascending=[False, True])

best_combo_sorted.head()

# Find the fastest test case in terms of elapsed time
fastest_test_case = data.loc[data['elapsed_s'].idxmin()]

# Extract the number of producers and consumers for the fastest test case
fastest_producers = fastest_test_case['producers']
fastest_consumers = fastest_test_case['consumers']
fastest_elapsed_time = fastest_test_case['elapsed_s']

fastest_producers, fastest_consumers, fastest_elapsed_time
