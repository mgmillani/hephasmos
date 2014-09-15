#!/bin/bash

fname(){
	echo "$1" | sed 's:\..*::'
}

assemble(){
	IFILE="$1"
	MACHINE="$2"
	FILEN=$(fname $1)
	hephasmos --input "$IFILE" --machine "$MACHINE" --messages asmmessages --output "$FILEN.mem" --type Mecaria0 --symbols "$FILEN.sym"
}

files=(ahmes.ahm cromag.cmg neander.nea pericles.per pitagoras.pit queops.que ramses.rhm reg.reg volta.vlt numbers.pit labels.que iloc8.il8)
machines=(ahmes cromag neander pericles pitagoras queops ramses reg volta pitagoras queops iloc8)

for ((i=0 ; i < ${#files[@]} ; i++))
do
	echo "${files[$i]}"
	assemble "${files[$i]}" "machines/${machines[$i]}"
	name=$(fname "${files[$i]}")	
	diff "$name.ok" "$name.mem"
	echo '------------'
done

