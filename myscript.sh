#!/bin/bash

RUNS=10

chars="/\\"

loading_anim() {

	local index size
	size=${#chars}

	while sleep 0.1; do
		printf "%s\b" "${chars:index++%size:1}"
	done

}

if [ $# -ge 1 ]; then
	RUNS=${1}
fi

sum12=0
sum14=0
sum24=0

echo "Executing ${RUNS} tests"

for i in `seq 1 ${RUNS}`; do

	echo -n "Starting test ${i}"
	dots=`echo "10 - ${#i}" | bc`
	printf ".%0.s" `seq 1 ${dots}`

	tput civis
	loading_anim &
	loading_pid=$!

	result=`./test.sh 2> /dev/null`
	result12=`echo "${result}" | grep "1-2" | awk '{print $3}'`
	result14=`echo "${result}" | grep "1-4" | awk '{print $3}'`
	result24=`echo "${result}" | grep "2-4" | awk '{print $3}'`
	sum12=`echo "scale=2; ${sum12} + ${result12}" | bc`
	sum14=`echo "scale=2; ${sum14} + ${result14}" | bc`
	sum24=`echo "scale=2; ${sum24} + ${result24}" | bc`

	disown ${loading_pid}
	kill "${loading_pid}" &> /dev/null
	tput cnorm

	echo "Done (${result12} | ${result14} | ${result24})"

done

final12=`echo "scale=2; ${sum12} / ${RUNS}" | bc`
final14=`echo "scale=2; ${sum14} / ${RUNS}" | bc`
final24=`echo "scale=2; ${sum24} / ${RUNS}" | bc`
echo "Finished. Average: ${final12} | ${final14} | ${final24}"