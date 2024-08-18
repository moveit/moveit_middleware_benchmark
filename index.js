import * as fastrtps_benchmark_data from './rmw_fastrtps/common.js';
import * as cyclonedds_benchmark_data from './rmw_cyclonedds/common.js';
// @TODO CihatAltiparmak when enable, add below line for zenoh bechmark dataset
// import * as zenoh_benchmark_data from './rmw_zenoh/common.js';

'use strict';

const middlewareColors = {
	rmw_zenoh: '#dea584',
	rmw_cyclonedds: '#00add8',
	rmw_fastrtps: '#f34b7d',

};

function reformat_dataset(middleware_data_list) {
	const map = new Map();

	for (const [middleware_name, middleware] of middleware_data_list) {
		for (const scenario of Object.keys(middleware.entries)) {
			if (!map.has(scenario)) {
				map.set(scenario, new Map());
			}
			map.get(scenario).set(middleware_name, new Array());
		}
	}

	for (const scenario of map.keys()) {

		for (const middleware_name of map.get(scenario).keys()) {

			for (const commit of middleware_data_list.get(middleware_name).entries[scenario]) {

				map.get(scenario).get(middleware_name).push({
					id: commit.commit.id,
					'value': commit.benches[0].value,
					'unit': commit.benches[0].unit
				});
			}
		}
	}


	return map;
}

function init() {

	const fastrtps_data = fastrtps_benchmark_data.BENCHMARK_DATA;
	const cyclonedds_data = cyclonedds_benchmark_data.BENCHMARK_DATA;
	// @TODO CihatAltiparmak when enable, add below line for zenoh bechmark dataset
	// const data = zenoh_benchmark_data.BENCHMARK_DATA;


	const middleware_datasets = reformat_dataset(
		new Map([
			["rmw_fastrtps", fastrtps_data],
			["rmw_cyclonedds", cyclonedds_data]
			// ["rmw_zenoh", data]]
		]));

	return middleware_datasets;
}

function renderGraph(parent, name, dataset, label) {
	const canvas = document.createElement('canvas');
	canvas.className = 'benchmark-chart';
	parent.appendChild(canvas);

	const data = {
		labels: label,
		datasets: dataset,
	};
	const options = {
		scales: {
			xAxes: [{
				scaleLabel: {
					display: true,
					labelString: 'commit',
				},
			}],
			yAxes: [{
				scaleLabel: {
					display: true,
					labelString: 'ns/iter',
				},
				ticks: {
					beginAtZero: true,
				}
			}],
		},
	};

	new Chart(canvas, {
		type: 'line',
		data,
		options,
	});
}


function create_plot_datasets(scenario_dataset) {

	var most_commit_number = 0;
	var longest_commit_array = new Array();

	for (const [middleware_name, commit_array] of scenario_dataset.entries()) {
		if (commit_array.length > most_commit_number) {
			most_commit_number = commit_array.length;
			longest_commit_array = commit_array;
		}
	}

	const plot_dataset = new Array();
	for (const [middleware_name, commit_array] of scenario_dataset.entries()) {
		plot_dataset.push({
			id: commit_array.map(commit => commit.id),
			label: middleware_name,
			data: commit_array.map(commit => commit.value),
			borderColor: middlewareColors[middleware_name],
			backgroundColor: middlewareColors[middleware_name] + '60'
		})
	}

	return {
		"plot_dataset": plot_dataset,
		"longest_commit_array": longest_commit_array.map(commit => commit.id.slice(0, 7))
	};
}

function renderBenchmark(scenario_name, scenarioDataset, main) {
	const setElem = document.createElement('div');
	setElem.className = 'benchmark-set';
	main.appendChild(setElem);

	const nameElem = document.createElement('h1');
	nameElem.className = 'benchmark-title';
	nameElem.textContent = scenario_name;
	setElem.appendChild(nameElem);

	const graphsElem = document.createElement('div');
	graphsElem.className = 'benchmark-graphs';
	setElem.appendChild(graphsElem);

	const plot_dataset_info = create_plot_datasets(scenarioDataset);
	renderGraph(graphsElem, scenario_name, plot_dataset_info["plot_dataset"], plot_dataset_info["longest_commit_array"]);
}

function renderAllChars(dataSet) {

	const main = document.getElementById('main');
	for (const scenario_name of dataSet.keys()) {
		renderBenchmark(scenario_name, dataSet.get(scenario_name), main);
	}
}

renderAllChars(init()); // Start