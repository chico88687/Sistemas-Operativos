#!/bin/bash
if [ ! -f medicos.txt ]; then
	touch medicos.txt
fi

if [ ! -f pacientes.txt ]; then 
	touch pacientes.txt
fi
if [[ $# = 2 ]]; then
#	num_l=`cut -f3 -d";" pacientes.txt | grep -i "$1" | wc -l`
	num_l=`grep -i ";$1;" pacientes.txt | cut -f3 -d";" | grep -i "$1" |wc -l`
	echo "O número de pacientes inseridos em $1 é: $num_l"
	num_s=`awk -v var=$2 -F";" 'BEGIN {num=0} {if($7 > var) num=num+1;}END {print(num)}' medicos.txt`
	echo "O número de médicos inscritos na plataforma com saldo acima de $2 é: $num_s"
else 
	echo "Número de argumentos inválidos."
fi
