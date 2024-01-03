#!/bin/bash  
echo  
echo Number of arguments passed to the shell script :  $#   

dir="./Producted_Files"

echo 
echo

for tld   # check one tld at a time
do
	echo "		---  Search: ${tld}  ---"
	tld_length=`expr length "$tld"`  

	counter_appear=0

	for dot_out_file in "$( find $dir -type f )"  # check all files
	do	

		locations=( $(cat ${dot_out_file}) )
		for ((i=0; i <= ${#locations[@]} - 1; i=i+2))
		do

		loc_length=`expr length "${locations[i]}"`
		flag=0
		start=$(($loc_length-$tld_length))

        
        temp="${locations[i]:start:tld_length}"   # If the length of location is 5 and the length of TLD is 3 start from 2 position 

		num_of_appearances=${locations[$(($i+1))]}  # Get the num_of_appearances of the current location

		if [ "$temp" == "$tld" ]
			then
				flag=$(($flag+1))  # ok increase the counter of num_of_appearances --->> The 2 strings are equal
		fi
                
		if [ "$flag" -eq 1 ] 
			then
				counter_appear=`expr $counter_appear + $num_of_appearances`  # increase the counter of num_of_appearances
		fi
	
		done
	done

	echo -n "TLD is ${tld} -->>   "

	echo "Counter( Number of appearances ) = $counter_appear"  # Print result of current TLD
	echo
done
