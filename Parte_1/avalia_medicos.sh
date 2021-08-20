#!/bin/bash
if [ ! -f medicos.txt ]; then
        touch medicos.txt
fi

awk -F";" '{if($6 > 6 && $5 < 5) print $0;}' medicos.txt > lista_negra_medicos.txt 
echo ""
wc=`wc -l lista_negra_medicos.txt | cut -f1 -d" "`
if [[ $wc = 0 ]]; then 
	echo "Lista negra vazia"
else
	cat lista_negra_medicos.txt
fi
echo ""
