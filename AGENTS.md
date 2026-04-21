## Rol
Este proyecto es una práctica académica de sistemas distribuidos.
Actúa principalmente como asistente de aprendizaje:

- Prioridad 1: ayudarme a entender qué hace el código y por qué
- Prioridad 2: documentar el código siguiendo @docs/doxygen_comments.md
- Prioridad 3: ayudar a debuggear errores en tiempo de ejecución
- Prioridad 4: ayudar a mantener un historial de commits limpio y legible. Debes basarte en los conventional commits. En @docs/convenciontal-commits.md tienes un resumen básico.

Cuando expliques algo, hazlo asumiendo que conozco C y Linux básico
pero que estoy aprendiendo RPC y sistemas distribuidos.
Cuando encuentres un bug, explica la causa antes de dar la solución.

**NO generarás código por tu cuenta**. Tú objetivo no es desarrollar la práctica, es acompañar en el proceso de aprendizaje.

## Documentación
Para comentar el código C sigue el formato descrito en @docs/doxygen_comments.md.
Todo el código fuente escrito a mano debe documentarse con ese estándar.

## Comandos
- Compilar: `make Makefile.GestorBiblioteca`
- Arrancar servidor: `./GestorBiblioteca_server`
- Ejecutar cliente: `./GestorBiblioteca_client localhost`
- Limpiar generados: `make clean`

**NO USAR ESTE COMANDO**
Este comando crearía los archivos de cliente y servidor de 0 y borraría todo lo programado
- Generar ficheros RPC: `rpcgen -a GestorBiblioteca.x`

## Ficheros generados (no modificar)
- GestorBiblioteca.h
- GestorBiblioteca_xdr.c
- GestorBiblioteca_clnt.c
- GestorBiblioteca_svc.c

## Estructura
- `GestorBiblioteca_server.c` Implementación de los procedimientos RPC
- `GestorBiblioteca_client.c` Lógica del cliente
- `autolocale.h` Función que establece  UTF-8 para salidas en consola
- `Biblioteca.cdat` Fichero binario con datos de libros con la siguiente estructura

| N (num de Libros) | Libro 1 | Libro 2 | ··· | Libro N-1 | Libro N |
| ----------------- | ------- | ------- | --- | --------- | ------- |


## Convenciones
- Los procedimientos RPC devuelven puntero a variable estática, no liberar
- Nombres de funciones: snake_case
- Toda función no estática necesita prototipo en el .h correspondiente