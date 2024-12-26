#!/bin/bash
seed=$1

instances_dir="datasets"
output_dir="testing"

mkdir -p "${output_dir}"

popsize=20      
ngen=20        
nobj=2           
pcross_bin=0.6   
pmut_bin=0.01     

for instance_path in "${instances_dir}"/*; do

    instance=$(basename "${instance_path}")
    
    output_file="${output_dir}/out_${instance}_${seed}.out"
    
    ./nsga2r "${seed}" "${instance_path}" "${popsize}" "${ngen}" "${nobj}" "${pcross_bin}" "${pmut_bin}" > "${output_file}"
    
    echo "Generado: ${output_file}"
done