import matplotlib.pyplot as plt
import numpy as np
import json
import os
import sys

BENCHMARK_RESULTS_DIR=sys.argv[1]

middleware_colors = {
    'rmw_zenoh_cpp' : 'orange',
    'rmw_cyclonedds_cpp' : 'peachpuff',
    'rmw_fastrtps_cpp' : 'tomato'}

middleware_list = ["rmw_zenoh_cpp", "rmw_cyclonedds_cpp", "rmw_fastrtps_cpp"]

def read_benchmark_json(file_name):
    benchmark_json_data = None
    with open(file_name) as f:
        benchmark_json_data = json.load(f)
    return benchmark_json_data

def get_real_time_list_from_benchmark_json(benchmark_json_data):
    real_time_list = []
    for benchmark_info in benchmark_json_data["benchmarks"]:
        if benchmark_info["run_type"] == "iteration":
            real_time_list.append(benchmark_info["real_time"])
    return real_time_list

def get_middleware_dataset_for_scenario(scenario_name):
    middleware_datasets = []
    for middleware_name in middleware_list:
        file_name = os.path.join(BENCHMARK_RESULTS_DIR, scenario_name, f"{middleware_name}.json")
        benchmark_json_data = read_benchmark_json(file_name)
        dataset = get_real_time_list_from_benchmark_json(benchmark_json_data)
        middleware_datasets.append(
            {
                "name" : middleware_name,
                "dataset" : dataset,
                "color" : middleware_colors[middleware_name],
            })
    return middleware_datasets

def plot_dataset_of_scenario(plt, middleware_datasets):
    labels = []
    colors = []
    datasets = []

    for x in middleware_datasets:
        labels.append(x["name"])
        colors.append(x["color"])
        datasets.append(x["dataset"])

    fig, ax = plt.subplots()
    ax.set_ylabel('real time (ns)')

    bplot = ax.boxplot(datasets,
                       patch_artist=True,
                       tick_labels=labels)

    # fill with colors
    for patch, color in zip(bplot['boxes'], colors):
        patch.set_facecolor(color)

for scenario_name in os.listdir(BENCHMARK_RESULTS_DIR):
    middleware_datasets = get_middleware_dataset_for_scenario(scenario_name)
    plot_dataset_of_scenario(plt, middleware_datasets)

plt.show()