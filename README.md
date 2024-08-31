# SISTOPE-LAB2-1-2024
Laboratorio de sistemas operativos
Leer flags y luego la logica de la solucion

## Para armar el proyecto
```bash
cd LAB/
sudo cmake -S ./ -B build
cd build
sudo make
```

### Para ejecutarlo a modo de prueba con parametros
```bash
./LAB2 -N "image" -p 2 -p 1.4 -u 0.6 -v 0.6 -C "supercarpeta" -R "clasificacion" -w 4
```

Hay un error a la hora de comunicar los procesos a traves de los pipes,
ya que al ser una estructura de datos, no se puede enviar directamente, sino que hay que 
"descomprimir" la estructura y enviar cada uno de los elementos por separado, es decir, enviar
una línea de píxeles por vez, y no la estructura completa.