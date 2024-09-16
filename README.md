# TP1 SO - README

<hr>


## Grupo 26

- [Nicole Salama](https://github.com/NikyySalama) - 64488
- [Valentin Garfi](https://github.com/vgarfi) - 64486
- [Esteban Bouyges](https://github.com/esteban481) - 67168

##  Trabajo Práctico

El trabajo práctico consta de 3 módulos que se intercomunican entre sí mediante mecanismos de IPCs

  + **main**: genera los esclavos y les envía los archivos a procesar, para luego enviar sus MD5 a **view** y guardarlos en **results.txt**.
  + **view**: imprime los datos enviados por **main** por *salida estandar*.
  + **slave**: realiza el hasheo del MD5 utilizando **md5sum**.
  + **shared_memory_lib**: librería para el manejo de datos y de la memoria compartida.
  + **semaphore_lib**: librería para el manejo de semáforos.
  + **utils**: librería para funciones generales del trabajo.
  + **error**: librería para el manejo de errores dentro del trabajo.



## Compilación

Dentro de la imágen de Docker provista por la cátedra de Sistemas Operativos, ejecutar `make` (o `make all`) para el compilado de los archivos .c.
Luego de la ejecución se generarán los siguientes archivos: 

  + **main**
  + **view**
  + **slave**

Si se desean remover los mismos, ejecutar `make clean` desde el mismo directorio donde fue realizada la compilación.

## Ejecución

Para correr el trabajo práctico, ejecutar el archivo **main** corriendo `./main` junto a la lista de archivos que se desean procesar, pasados como parámetros. Hemos desarrollado un script, **generator.sh**, que permite la creación de la carpeta `md5Files` con 100 archivos de tamaño aleatorio entre 10MB y 500MB.


Para ejecutar el trabajo, hay tres maneras:

1. Mediante el uso de pipes entre **main** y **view** 

```bash
./main ./md5Files/* | ./view
```

2. Ejecutando **main** en una terminal y **view** en otra

```bash
# Terminal 1:
./main ./md5Files/*
/main_view_connection

```

```bash
# Terminal 2:
 ./view /main_view_connection
```

3. Ejecutando **main** en *background* y **view** en *foreground*

```bash
./main md5Files/*&                      # background
./view /main_view_connection            # foreground
```

Al finalizar la ejecución, se podrá leer en el archivo llamado **results.txt** la información allí volcada, que coincide con la respuesta enviada por cada slave.

## Testeo

Para los testeos con **PVS-Studio**, y **Valgrind** se debe primero tener instalado **PVS-Studio**  (referirse a la documentación de la cátedra para su instalación).

Luego correr el siguiente comandos:

```bash
 make test
```

Junto con `VALGRIND_ARGS=a_ejecutar`, donde la ejecución a analizar debe estar entre comillas dobles. Por defecto, si no se introduce ningún argumento, se ejecutará con `”./main ./md5Files/*”`. Aconsejamos correr el script generador de archivos para ignresarlo como parámetro.

Los resultados para cada analyzer se encontrarán de la siguiente manera:

 * **PVS-Studio:**: Ver PVS-Studio.err
 * **Valgrind:**: Ver consola de comandos

Para remover los mismos, correr el comando `make cleanTest` en el mismo directorio donde fue realizada la compilación.

Trabajo práctico realizado por el grupo 26 de Sistemas Operativos