# SCA_research
ASCON Reference Implementation: https://github.com/ascon/ascon-c

## Export the path to the arm compiler:
For example
```export PATH="/mnt/d/opt/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH"```

## Steps:
Running from home directory:
```
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

```