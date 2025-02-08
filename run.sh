cp MBedAES.c ./ELMO/Examples/DPATraces/MBedAES/MBedAES.c
cd ELMO/Examples/DPATraces/MBedAES
make clean
make all
cd ../../..
./elmo Examples/DPATraces/MBedAES/MBedAES.bin
cd ..
gcc ref.c -o ref.o
./ref.o
python3 output.py
python3 diff.py