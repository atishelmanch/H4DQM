#!/bin/bash

input="/tmp/"
output="/tmp"
run="0"
spill="0"
prescale=1

TEMP=`getopt -o i:o:r:s:p: --long input:,output:,run:,spill:prescale: -n 'runDQM.sh' -- "$@"`
if [ $? != 0 ] ; then echo "Options are wrong..." >&2 ; exit 1 ; fi

eval set -- "$TEMP"


while true; do
case "$1" in
-i | --input ) input="$2"; shift 2 ;;
-o | --output ) output="$2"; shift 2 ;;
-r | --run ) run="$2"; shift 2;;
-s | --spill ) spill="$2"; shift 2;;
-p | --prescale ) prescale=$2; shift 2;;
-- ) shift; break ;;
* ) break ;;
esac
done



### PLOT MAKER -- make me configurable
### /home/cmsdaq/DAQ/H4DQM/bin/plotterHodo -i $output -o $output  -r $run -s $spill -I integrated.root
### /home/cmsdaq/DAQ/H4DQM/bin/plotterDAQStatus -i $output -o $output  -r $run -s $spill -I integrated.root
### /home/cmsdaq/DAQ/H4DQM/bin/plotterTDC -i $output -o $output  -r $run -s $spill 
#for runtype in beam ped led;do
if [ $((spill%5)) -eq 1 ]; then
    if [ $((spill)) -ne 3 ]; then #skip spill 3 so that it's faster to see plots of first spill in the run
	/home/cmsdaq/DAQ/H4DQM/bin/unpack -i $input  -o $output -r $run -s $spill -p $prescale	    
	for runtype in led ped beam;do
	    /home/cmsdaq/DAQ/H4DQM/bin/plotterTotal -i $output -o $output  -r $run -s $spill -t$runtype -I integrated.root 
#/home/cmsdaq/DAQ/H4DQM/bin/plotterDigitizer -i $output -o $output  -r $run -s $spill 
	    
	    cd $output/$run/$spill/$dir/$runtype/
	    mkdir hodo
	    mkdir DAQ
	    mkdir ADC
	    mkdir TDC 
	    mkdir digitizer 
	    
	    mv total/hodo_* hodo/
	    mv total/DAQStatus_* DAQ/
	    mv total/ADC_* ADC/
	    mv total/TDC_* TDC/
	    mv total/digitizer_* digitizer/
	    mv total/*.root .
	    
	    rm -r total
	    cd -
	    
## hodo , TDC , DAQ 
# copy skeleton php
	    rsync -aP /home/cmsdaq/skel_DQM/ $output/$run/ 
	    rsync -aP /home/cmsdaq/skel_DQM/ $output/$run/$spill/

	    
	    rsync -aP $output/$run/ pcethtb3.cern.ch:/data/public_DQM_plots/$run/


	done
    

	for runtype in ped beam;do
	    rsync -aP /home/cmsdaq/skel_DQM/ $output/$run/$spill/$runtype/
	    for dir in digitizer hodo DAQ TDC ADC
#for dir in hodo DAQ ADC TDC
	    do
 		rsync -aP /home/cmsdaq/skel_DQM/ $output/$run/$spill/$runtype/$dir/
	    done
	done


    
# touch -R
	find $output/$run/$spill -type f -exec touch {} \;
	
	[ -h $output/$run/last ] && rm $output/$run/last
	[ -h $output/last ] && rm $output/last
	
	ln -s $output/$run/$spill $output/$run/last
	ln -s $output/$run $output/last
	
	rsync -aP $output/$run/ pcethtb3.cern.ch:/data/public_DQM_plots/$run/
	rsync -aP $output/last pcethtb3.cern.ch:/data/public_DQM_plots/
    fi
fi