#!/bin/bash
grep ^a /etc/passwd | grep bash$ | head > pacientes.txt
sed -i 's/,*//g' pacientes.txt
sed -i 's/^a*//g' pacientes.txt 
sed -i 's/:/;/g' pacientes.txt
sed -i 's/;x;[0-9]*;[0-9]*//g' pacientes.txt
sed -i 's\;/bin/bash\\g' pacientes.txt
sed -i 's\/home/\;;\g' pacientes.txt
sed -i 's/$/@iscte-iul.pt;100/' pacientes.txt
cat pacientes.txt
echo ""
echo "Ficheiro pacientes.txt criado"
echo ""
