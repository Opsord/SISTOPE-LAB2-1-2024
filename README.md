# SISTOPE-LAB1-1-2024
Laboratorio de sistemas operativos
Leer flags y luego la logica de la solucion

## Desde 0
```bash
cd LAB/
cd build/
sudo cmake -S ../ -B .
sudo make
```
## Solo compilar
```bash
sudo cmake -S ../ -B .
sudo make
```

## Compilar con gdb
```bash
sudo cmake -S ../ -B . -DCMAKE_BUILD_TYPE=Debug
sudo make
```

## Ejecutar desde carpeta build
```bash
./LAB2 -N "image" -C supercarpeta -R clasi
```
### O mas compelto
```bash
./LAB2 -N "image" -p 2 -p 1.4 -u 0.6 -v 0.6 -C "supercarpeta" -R "clasi"
```