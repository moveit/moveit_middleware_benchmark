# Inspired from https://unix.stackexchange.com/questions/31414/how-can-i-pass-a-command-line-argument-into-a-shell-script
helpFunction()
{
   echo ""
   echo "Usage: $0 -i initial_script -m middleware_name -d benchmark_results_directory"
   echo -i "\t-i initial_script to run once all benchmarks are started"
   echo -m "\t-m selected middleware to benchmark"
   echo -d "\t-d the directory the benchmark results is saved"
   exit 1 # Exit script after printing help
}

while getopts "i:m:d:" opt
do
   case "$opt" in
      i ) initial_script="$OPTARG" ;;
      m ) middleware_name="$OPTARG" ;;
      d ) benchmark_results_directory="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$initial_script" ] || [ -z "$middleware_name" ] || [ -z "$benchmark_results_directory" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

echo "middleware name is $middleware_name"
echo "benchmark results directory is $benchmark_results_directory"

echo "Benchmarking is starting!"
echo "Starting initial scripts before benchmarks run!"
sh "$initial_script"
echo "Initial script has finished! Now starting to benchmark middleware with scenarios!"

export RMW_IMPLEMENTATION=${middleware_name}

mkdir ${benchmark_results_directory}/scenario_basic_service_client -p
ros2 daemon stop
ros2 launch moveit_middleware_benchmark scenario_basic_service_client_benchmark.launch.py benchmark_command_args:="--benchmark_out=${benchmark_results_directory}/scenario_basic_service_client/${middleware_name}.json --benchmark_out_format=json"

mkdir ${benchmark_results_directory}/scenario_perception_pipeline -p
ros2 daemon stop
ros2 launch moveit_middleware_benchmark scenario_perception_pipeline_benchmark.launch.py benchmark_command_args:="--benchmark_out=${benchmark_results_directory}/scenario_perception_pipeline/${middleware_name}.json --benchmark_out_format=json --benchmark_repetitions=1 --benchmark_min_time=20x" selected_test_case_index:=0
