# SD_Prac1

En [este enlace](https://javiiariass.github.io/SD_Prac1) está la documentación que se ha generado de los servicios y funciones usadas en esta práctica

## Consideraciones en el código

### Cliente

#### Menus

En los menús, tanto el normal como el admin, he cogido las opciones como char para evitar que haya errores (a veces me equivocaba escribiendo una letra y, al pedir un entero, el programa se colgaba). Con esto conseguimos que, al menos no se congele el programa.
Para devolver el valor entero lo casteamos y le restamos el valor entero de '0'. Si casteamos el caracter '5' a entero nos da su código ascii. Si a este código le restamos el código ascii de '0', conseguimos el valor real del número. (53-48 = 5)

#### scanf

Para los campos de libro que puedan tener varias palabras (nombre o titulos):

- He usado scanf("%[^\n]", string) para leer toda la línea hasta que el usuario escriba el retorno de carro.
- Con scanf(" %s", string) solo se leería la primera palabra.

#### Manejo Nuevo libro

En vez de pedir todos los datos del libro del tirón, primero pido ISBN, compruebo si ya existe en servidor para no pedir los demás datos.

#### Manejo Buscar libros

`strchr` es igual que `strstr` pero buscando un único carácter. Devuelve nulo si el elemento no está en el string de entrada.

- He hecho una variante de la función `Comprobar` que nos has proporcionado. Esta función también devuelve verdadero o falso, pero modifica la Cadena pasada por parámetro para:
  - Si encuentra el texto en el campo de libro -> lo formateamos resaltandolo en negrita. Devuelve _true_
  - Si no encuentra el texto lo deja igual devolviendo _false_.

Por tanto, mantiene el mismo flujo que `Comprobar`.

Para hacer esto hago lo siguiente:

1. He usado `strcasestr` en vez de `strstr`. Es prácticamente lo mismo solo que `strcasestr` no considera las mayúsculas. Ej. Si el autor es 'Pepe Lopez' y busco 'pepe' `strstr` no va a encontrar nada pero `strcasestr` sí que lo hace. (para usarlo hay que incluir `#define _GNU_SOURCE` e `#include <string.h>`)
2. Después uso `snprintf`, básicamente hace lo mismo que `printf` pero, en vez de escribirlo en terminal, lo guarda en el string que le pases por parámetro. Con esto contateno el texto antes del encontrado, el texto encontrado resaltado y lo siguiente al texto encontrado

El formato en `snprintf` es:

```c
"%.*s\x1b[31m%.*s\x1b[37m%s"
```

- `%.*s` -> parecido a `%s` pero en vez de imprimir la cadena entera que le pases hasta el final (`\0`), imprime el número exacto de carácteres que le digas.
- `\x1b[31m` -> código ANSI para escribir el texto en rojo.
- `\x1b[37m` -> código ANSI para escribir el texto en blanco.
  (Hay muchos códigos y se pueden combinar, por ej \x1b[31m es solo rojo pero si ponemos \x1b[1;31m) es texto en negrita y de color rojo.

Por tanto, sería algo así:

| %.\*s                                                                    | \x1b[31m           | %.\*s                                                                                  | \x1b[37m                      | %s                                                              |
| ------------------------------------------------------------------------ | ------------------ | -------------------------------------------------------------------------------------- | ----------------------------- | --------------------------------------------------------------- |
| Primera parte del texto. Hasta el número de caracteres que le indiquemos | Ponemos texto rojo | Segunda parte del texto (en negrita). Hasta el número de caracteres que le indiquemos. | Ponemos texto blanco de nuevo | Mostramos el resto del texto de la cadena hasta el final (`\0`) |

También he usado `strncpy` que hace lo mismo que `strcpy` pero indicandole un valor máximo por si el texto a copiar ocupa más que el tamaño de destino.

> [!WARNING] 
> La función `Formatea` que nos has proporcionado rompe un poco el formato  con esta función -> los códigos ascii también los cuenta para el tamaño de `Texto` (aunque realmente no se muestren). Pueden descuadrarse un poco algunos elementos al buscar.

##### OR binario

He usado `|=`: es un OR binario -> para bool_t, así no pasa de TRUE si le sumas otro TRUE (bool_t son enteros al final).

```c
Encontrado |= formatea_texto(L->Isbn, Texto);
Encontrado |= formatea_texto(L->Titulo, Texto);
Encontrado |= formatea_texto(L->Autor, Texto);
Encontrado |= formatea_texto(L->Pais, Texto);
Encontrado |= formatea_texto(L->Idioma, Texto);
```

hace lo mismo que:

```c
if (formatea_texto(L->Isbn, Texto)) Encontrado = TRUE;
if (formatea_texto(L->Titulo, Texto)) Encontrado = TRUE;
if (formatea_texto(L->Autor, Texto)) Encontrado = TRUE;
if (formatea_texto(L->Pais, Texto)) Encontrado = TRUE;
if (formatea_texto(L->Idioma, Texto)) Encontrado = TRUE;
```

### Servidor

#### Servicio `devolver`

En la implementación del servicio, el enunciado dice lo siguiente:

```md
El libro no se puede devolver, porque no hay ni usuarios en lista de espera **ni libros
prestados**.
```

Si no hay libros prestados, debería dar igual si hay, o no, lista de espera, ¿no?
En caso de devolver 2, **solo** contemplo que **no** haya **libros prestados**.

#### Servicio `ordenar`

##### Caso ambiguo

El enunciado dice lo siguiente:

```md
\*Realizará una ordenación de los libros almacenados en el vector dinámico por el campo ‘Campo’. Una vez
terminada la ordenación el campo ‘Campo’ será guardado en el servidor para futuras ordenaciones.
Previamente a este proceso se verificará que el Ida sea correcto y **que el vector dinámico tiene libros**
```

No se indica, en el caso de que el vector no tenga libros, si devolver _TRUE_ o _FALSE_
En caso de que no haya libros, no ordeno el vector pero devuelvo _TRUE_ (el vector vacío está ordenado).

##### Implementación de ordenación

He usado qsort, para qsort hace falta pasarle una función comparadora. He adaptado la función que nos has proporcionado para usarla con qsort. La he nombrado de otra manera para que puedas seguir usando la función original (`EsMenor(int P1, int P2, int Campo)`) si lo necesitas.

La función comparadora debe devolver lo siguiente:

- Un número negativo si libroA(P1) va ANTES que libroB(P2)
- Un 0 si son IGUALES
- Un número positivo si libroA(P1) va DESPUÉS que libroB(P2)

Como strcmp ya hace eso internamente simplemente tengo que devolverlo como resultado para los campos de tipo `Cadena`. Para los enteros conseguimos lo mismo con una simple resta.

El switch adaptado sería:

```c
switch (CampoOrdenacion) // El del servidor directamente
	{
	case 0:
		salida = strcmp(libroA->Isbn, libroB->Isbn);
		break;
	case 1:
		salida = strcmp(libroA->Titulo, libroB->Titulo);
		break;
	case 2:
		salida = strcmp(libroA->Autor, libroB->Autor);
		break;
	case 3:
		salida = libroA->Anio - libroB->Anio;
		break;
	case 4:
		salida = strcmp(libroA->Pais, libroB->Pais);
		break;
	case 5:
		salida = strcmp(libroA->Idioma, libroB->Idioma);
		break;
	case 6:
		salida = libroA->NoLibros - libroB->NoLibros;
		break;
	case 7:
		salida = libroA->NoPrestados - libroB->NoPrestados;
		break;
	case 8:
		salida = libroA->NoListaEspera - libroB->NoListaEspera;
		break;
	}
```

#### Carga de datos de fichero

En la función que carga los datos del binario `.cdat` a memoria en el servidor, he implementado que se asignen automáticamente los libros que haya disponibles si hay lista de espera en esos libros.

Si el fichero cargado tiene información inconsistente (por ejemplo, un libro que tenga a la vez ejemplares disponibles (`NoLibros > 0`) y gente en lista de espera (`NoListaEspera > 0`)), el servidor purga esa inconsistencia distribuyendo los libros disponibles a los usuarios en espera antes de empezar a servir peticiones.

Además, el método de carga de datos usa un array temporal de libros en vez de sobreescribir el vector principal directamente. Solo una vez se haya podido reservar la memoria temporal (`malloc`) y cargar (`fread`) exitosamente todo el fichero nuevo, se procede a liberar el vector antiguo y reasignarlo. Con esto evitamos liberar el vector anterior si ocurre un problema y no podemos abrir el nuevo.
