#!/bin/bash
if [ ! -f medicos.txt ]; then 
	touch medicos.txt
fi
if [[ $# = 4 ]]; then
	if [[ -z "$1" || -z "$2" || -z "$3" || -z "$4" ]]; then
		echo "Argumentos vazios"
	else
		existe_num=`grep ";$2;" medicos.txt`
		if [[ "$existe_num" = "" && $2 = ?(+|-)+([0-9]) ]]; then
			verifica=`grep -i "$1" medicos.txt | grep -i "$3" |grep -i "$4"`
			if [ "$verifica" = "" ]; then
				echo $1";"$2";"$3";"$4";0;0;0" >> medicos.txt
				echo "Médico adicionado com sucesso"
			else
				echo "Credenciais inválidas"
			fi
		else
			echo Numero Inválido.
		fi
	fi
else 
	echo "Número de argumentos inválidos"
fi
