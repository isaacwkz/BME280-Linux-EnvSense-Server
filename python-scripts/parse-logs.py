# Log parser written by ChatGPT
# Arg: file to parse

import json
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime
import argparse

# Function to parse the logs and prepare data for plotting
def parse_logs(file_path):
    timestamps = []
    temps = []
    hums = []
    press = []

    with open(file_path, 'r') as file:
        for line in file:
            log_entry = json.loads(line.strip())
            timestamps.append(datetime.strptime(log_entry['timestamp'], "%Y-%m-%d %H:%M:%S"))
            temps.append(log_entry['temp'])
            hums.append(log_entry['hum'])
            press.append(log_entry['pres'])

    # Create a DataFrame
    df = pd.DataFrame({
        'timestamp': timestamps,
        'temp': temps,
        'hum': hums,
        'pres': press
    })

    # Set timestamp as index
    df.set_index('timestamp', inplace=True)

    # Remove rows with timestamps that are more than 5 minutes apart
    df = df[df.index.to_series().diff().dt.total_seconds().fillna(0).lt(300) | df.index.isin([df.index[0]])]

    return df

# Main execution
if __name__ == "__main__":
    # Set up argument parsing
    parser = argparse.ArgumentParser(description='Plot environmental data from a log file.')
    parser.add_argument('log_file', type=str, help='Path to the log file (e.g., foobar.log)')
    
    args = parser.parse_args()
    
    # Specify the path to the log file from the argument
    log_file_path = args.log_file
    
    # Parse log data
    df = parse_logs(log_file_path)

    # Plotting
    fig, ax1 = plt.subplots(figsize=(12, 6))

    # Plot Temperature
    color1 = 'tab:blue'
    ax1.set_xlabel('Timestamp')
    ax1.set_ylabel('Temperature (°C)', color=color1)
    ax1.plot(df.index, df['temp'], label='Temperature (°C)', marker='o', linestyle='None', color=color1, markersize=2)  # Smaller dots
    ax1.tick_params(axis='y', labelcolor=color1)
    
    ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
    color2 = 'tab:green'
    ax2.set_ylabel('Humidity (%)', color=color2)  # we already handled the x-label with ax1
    ax2.plot(df.index, df['hum'], label='Humidity (%)', marker='o', linestyle='None', color=color2, markersize=2)  # Smaller dots
    ax2.tick_params(axis='y', labelcolor=color2)

    ax3 = ax1.twinx()  # instantiate a third axes that shares the same x-axis
    color3 = 'tab:red'
    ax3.spines['right'].set_position(('outward', 60))  # move the third axis outwards
    ax3.set_ylabel('Pressure (Pa)', color=color3)  # we already handled the x-label with ax1
    ax3.plot(df.index, df['pres'], label='Pressure (Pa)', marker='o', linestyle='None', color=color3, markersize=2)  # Smaller dots
    ax3.tick_params(axis='y', labelcolor=color3)
    
    # Set formatting for the pressure y-axis to avoid scientific notation
    ax3.yaxis.get_major_formatter().set_useOffset(False)
    ax3.yaxis.get_major_formatter().set_scientific(False)

    # Adding titles, legends, and grid
    plt.title('Environmental Conditions Over Time')
    ax1.legend(['Temperature (°C)'], loc='upper left')
    ax2.legend(['Humidity (%)'], loc='upper right')
    ax3.legend(['Pressure (Pa)'], loc='center right')

    plt.grid()
    fig.tight_layout()  # to prevent clipping of y-labels
    
    plt.show()