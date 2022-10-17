#!/bin/bash

# time between printing consequent symbols
if [ $# -ge 1 ]
then
	delta=$1
fi
delta=${delta:-0.05}

# making hex-onebyte sequence from input without line numbers
hex_text=$(od -An -t x1)

# making text of (row, col, hex_char_from_input)
row=0
col=0
indexed_text=''
all_lines=0
for c in $hex_text
do
    if [ $c = "0a" ]
	then 
        (( row++ ))
		(( all_lines++ ))
		col=0
    else
	    if ! [ $c = "20" ]
		then
			indexed_char="$row $col $c"
			if [ -z "$indexed_text" ]
			then
				indexed_text=$indexed_char
			else
				indexed_text="$indexed_text\n$indexed_char"
			fi
		fi
		(( col++ )) 
    fi
done
