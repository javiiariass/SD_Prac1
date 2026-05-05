/**
 * @file   GestorBiblioteca_client.c
 * @brief  Implementación de la lógica del cliente RPC para la Biblioteca.
 *
 * Contiene la interfaz de usuario y la lógica para conectarse
 * y consumir los servicios expuestos por el servidor RPC.
 *
 * @author Javi
 */

#define _GNU_SOURCE // Para habilitar strcasestr()
#include "GestorBiblioteca.h"

// establece la codificación del sistema al programa
#include "autolocale.h"

//  librerias importadas por el profesor
#include <stdlib.h>
// #include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define Cls system("clear")
#define Pause system("read -p \"Pulsa la tecla return para continuar..... \" a")
#define MostrarAviso(Texto) \
	{                       \
		printf(Texto);      \
		Pause;              \
	}
// ******************************************** Variables globales ********************************************
int ida = -1;
// ******************************************** MENUS ********************************************

/**
 * @brief Muestra el menú principal de la aplicación y captura la opción del usuario.
 *
 * Presenta las opciones básicas (Administración, Consulta, Préstamo, Devolución)
 * e impide que el usuario introduzca una opción fuera de rango.
 *
 * @return Opción seleccionada por el usuario (entero entre 0 y 4).
 */
int MenuPrincipal()
{
	char Salida;
	do
	{
		Cls;
		printf(" GESTOR BIBLIOTECARIO 1.0 (M. PRINCIPAL)\n");
		printf("*****************************************\n");
		printf("\t1.- M. Administración\n");
		printf("\t2.- Consulta de libros\n");
		printf("\t3.- Préstamo de libros\n");
		printf("\t4.- Devolución de libros\n");
		printf("\t0.- Salir\n\n");
		printf("\t(9.- Ejecutar Test Automático de Servicios)\n\n");
		printf(" Elige opción: ");
		scanf(" %c", &Salida);
		if ((Salida < '0' || Salida > '4') && Salida != '9')
			MostrarAviso("\n\n *** Error en la entrada de Datos.***\n\n");
	} while ((Salida < '0' || Salida > '4') && Salida != '9');
	
	
	// Pasamos el char a entero
	return ((int)Salida - '0');
}

/**
 * @brief Despliega el menú de administración y lee la opción seleccionada.
 *
 * Muestra opciones como cargar datos, nuevo libro, comprar, etc., y se asegura
 * de devolver un valor dentro de los límites esperados.
 *
 * @return Opción elegida por el administrador (entero entre 0 y 8).
 */
int MenuAdministracion()
{
	char Salida;
	do
	{
		Cls;
		printf(" GESTOR BIBLIOTECARIO 1.0 (M. ADMINISTRACION)\n");
		printf("**********************************************\n");
		printf("\t1.- Cargar datos Biblioteca\n");
		printf("\t2.- Guardar datos Biblioteca\n");
		printf("\t3.- Nuevo libro\n");
		printf("\t4.- Comprar libros\n");
		printf("\t5.- Retirar libros\n");
		printf("\t6.- Ordenar libros\n");
		printf("\t7.- Buscar libros\n");
		printf("\t8.- Listar libros\n");
		printf("\t0.- Salir\n\n");
		printf(" Elige opción: ");
		scanf(" %c", &Salida);
		if (Salida < '0' || Salida > '8')
			MostrarAviso("\n\n *** Error en la entrada de Datos.***\n\n");
	} while (Salida < '0' || Salida > '8');
	return ((int)Salida-'0');
}

// ******************************************** funciones auxiliares ********************************************

/**
 * @brief Formatea un texto a un ancho determinado rellenando con un carácter.
 *
 * Utilizada para alinear texto en consola, asumiendo ciertas longitudes de las
 * cadenas y tratando caracteres extendidos (como UTF-8).
 *
 * @param[out] Salida    Cadena donde se devolverá el resultado formateado.
 * @param[in]  Texto     Cadena de entrada original.
 * @param[in]  Ancho     Anchura final esperada.
 * @param[in]  Caracter  Carácter empleado para rellenar los espacios restantes.
 */
void Formatea(char *Salida, const char *Texto, int Ancho, char Caracter)
{
	Cadena Vacia;
	int len = Ancho - strlen(Texto);
	int l = 0, c = 0;

	while (Texto[l] != '\0')
	{
		if ((unsigned char)Texto[l] > 128)
			c++;
		l++;
	}
	len += c / 2;

	if (len < 0)
		len = 0;
	for (int i = 0; i < len; i++)
		Vacia[i] = Caracter;
	Vacia[len] = '\0';

	sprintf(Salida, "%s%s", Texto, Vacia);
}

/**
 * @brief Muestra tabulados los campos de un libro con opción a imprimir la cabecera.
 *
 * Emplea funciones de formato de texto para imprimir ordenadamente la tabla
 * de libros y su correspondiente fila.
 *
 * @param[in] L         Puntero a la estructura TLibro que se va a mostrar.
 * @param[in] Pos       Índice o posición del libro en la lista (indexado desde 0).
 * @param[in] Cabecera  Si es TRUE, imprime los nombres de las columnas antes del libro.
 */
void MostrarLibro(TLibro *L, int Pos, bool_t Cabecera)
{
	Cadena T, A, B, PI;
	if (Cabecera == TRUE)
	{
		printf("%-*s%-*s%-*s%*s%*s%*s\n", 5, "POS", 58, "TITULO", 18, "ISBN", 4, "DIS", 4, "PRE", 4, "RES");
		printf("     %-*s%-*s%-*s\n", 30, "AUTOR", 28, "PAIS (IDIOMA)", 12, "AÑO");
		Formatea(B, "*", 93, '*');
		printf("%s\n", B);
	}
	Formatea(T, L->Titulo, 58, ' ');
	Formatea(A, L->Autor, 30, ' ');
	strcpy(B, L->Pais);
	strcat(B, "(");
	strcat(B, L->Idioma);
	strcat(B, ")");
	Formatea(PI, B, 28, ' ');
	printf("%-5d%s%-*s%*d%*d%*d\n", Pos + 1, T, 18, L->Isbn, 4, L->NoLibros, 4, L->NoPrestados, 4, L->NoListaEspera);
	printf("     %s%s%-*d\n", A, PI, 12, L->Anio);
}

/**
 * @brief Resalta la primera coincidencia de texto en un campo de libro.
 *
 * Busca `texto_buscado` dentro de `campo_original` sin distinguir entre mayúsculas
 * y minúsculas. Si se encuentra la subcadena, se sobrescribe el campo original
 * insertando códigos ANSI para resaltar el texto en rojo.
 *
 * @param[in,out] campo_original Cadena que se sobrescribe con la versión formateada.
 * @param[in] texto_buscado Subcadena que se desea buscar y resaltar.
 * @return TRUE si se encontró y formateó el texto, FALSE en caso contrario.
 */
bool_t formatea_texto(char *campo_original, char *texto_buscado)
{
	char *posicion_encontrado = strcasestr(campo_original, texto_buscado);

	if (posicion_encontrado == NULL)
		return FALSE;

	// Vemos las direcciones donde debería empezar la negrita
	int tamano_antes = posicion_encontrado - campo_original;
	int tamano_negrita = strlen(texto_buscado);

	// Creamos string con las negritas
	Cadena campo_con_negrita;
	snprintf(campo_con_negrita, sizeof(campo_con_negrita), "%.*s\x1b[31m%.*s\x1b[37m%s",
			 tamano_antes, campo_original,
			 tamano_negrita, posicion_encontrado,
			 posicion_encontrado + tamano_negrita);
	// escribimos campo_original -> pero el número de caracteres que indicatamano_antes
	// escribimos posicion_encontrado -> pero el número de caracteres que indicatamano_antes
	// nos ubicamos al final de las negritas(inicio del puntero más el tamaño de las negritas -> final)
	// y escribimos lo que queda del campo_original

	// igual que strcpy pero con un límite máximo (tamaño de cadena)
	// reemplazamos por el texto nuevo
	strncpy(campo_original, campo_con_negrita, sizeof(Cadena));

	// si el texto ocupa lo mismo o más que el tamaño cadena no se ha escrito `\0` al final.
	campo_original[sizeof(Cadena) - 1] = '\0';

	return TRUE;
}

/**
 * @brief Comprueba y formatea coincidencias de búsqueda en un libro.
 *
 * Dependiendo de `Campo`, busca `Texto` en un único campo del libro o en todos
 * los campos si se solicita el comodín '*'. Marca las coincidencias usando
 * códigos ANSI para facilitar su visualización en la consola.
 *
 * @param[in,out] L Libro que se examina y se puede modificar para resaltar resultados.
 * @param[in] Texto Texto de búsqueda a localizar en los campos del libro.
 * @param[in] Campo Código que indica el campo a buscar: 'i', 't', 'a', 'p', 'd' o '*'.
 * @return TRUE si la búsqueda encontró alguna coincidencia, FALSE si no.
 */
bool_t comprobar_formateado(TLibro *L, char* Texto, char Campo)
{
	bool_t Encontrado = FALSE;

	switch (tolower(Campo))
	{
	case 'i':
		Encontrado = formatea_texto(L->Isbn, Texto);
		break;
	case 't':
		Encontrado = formatea_texto(L->Titulo, Texto);
		break;
	case 'a':
		Encontrado = formatea_texto(L->Autor, Texto);
		break;
	case 'p':
		Encontrado = formatea_texto(L->Pais, Texto);
		break;
	case 'd':
		Encontrado = formatea_texto(L->Idioma, Texto);
		break;
	case '*':
		// En el caso '*' comprobamos todos y aplicamos negrita a todos los que coincidan.
		//  `|=` es OR binario
		Encontrado |= formatea_texto(L->Isbn, Texto);
		Encontrado |= formatea_texto(L->Titulo, Texto);
		Encontrado |= formatea_texto(L->Autor, Texto);
		Encontrado |= formatea_texto(L->Pais, Texto);
		Encontrado |= formatea_texto(L->Idioma, Texto);
		break;
	}
	return Encontrado;
}

/**
 * @brief ORIGINAL DEL PROFESOR. Comprueba si el texto especificado existe dentro de un campo del libro.
 *
 * @warning NO UTILIZAR EN ESTA IMPLEMENTACIÓN. Se mantiene únicamente
 * por motivos de compatibilidad o por si el profesor la usa en el script de comprobación de la práctica.
 * En su lugar, se utiliza comprobar_formateado().
 *
 * Busca de forma estricta o total ('*') si la sub-cadena `Texto`
 * aparece en un campo (ISBN, título, autor...). Resulta fundamental para la
 * implementación de búsquedas.
 *
 * @param[in] L      Puntero a estructura TLibro sobre el que se va a buscar.
 * @param[in] Texto  Sub-cadena de la cual se desea comprobar la ocurrencia.
 * @param[in] Campo  Letra que indica el filtro a aplicar:
 *                   - 'i': ISBN, 't': Título, 'a': Autor, 'p': País, 'd': Idioma, '*': Cualquier campo.
 * @return           TRUE si contiene el texto indicado, o FALSE en caso contrario.
 * @see comprobar_formateado
 */
bool_t Comprobar(TLibro *L, Cadena Texto, char Campo)
{
	bool_t Encontrado = FALSE;
	switch (tolower(Campo))
	{
	case 'i':
		Encontrado = strstr(L->Isbn, Texto) != NULL ? TRUE : FALSE;
		break;
	case 't':
		Encontrado = strstr(L->Titulo, Texto) != NULL ? TRUE : FALSE;
		break;
	case 'a':
		Encontrado = strstr(L->Autor, Texto) != NULL ? TRUE : FALSE;
		break;
	case 'p':
		Encontrado = strstr(L->Pais, Texto) != NULL ? TRUE : FALSE;
		break;
	case 'd':
		Encontrado = strstr(L->Idioma, Texto) != NULL ? TRUE : FALSE;
		break;
	case '*':
		Encontrado = (strstr(L->Isbn, Texto) != NULL || strstr(L->Titulo, Texto) != NULL || strstr(L->Autor, Texto) != NULL ||
					  strstr(L->Pais, Texto) != NULL || strstr(L->Idioma, Texto) != NULL)
						 ? TRUE
						 : FALSE;
	}
	return Encontrado;
}

/**
 * @brief Comprueba que la respuesta de una llamada RPC no sea NULL.
 *
 * Si es nula, significa que hubo un error de comunicación RPC, por lo que
 * se informa al usuario mediante clnt_perror sobre el fallo antes de volver
 * a la rutina normal.
 *
 * @param[in] resultado Puntero con el resultado estático de una llamada RPC.
 * @param[in] clnt      Puntero opaco al handle del cliente (struct CLIENT).
 * @return              TRUE si `resultado` es distinto de NULL, FALSE en caso contrario.
 */
bool_t comprueba_llamada(void *resultado, CLIENT *clnt)
{
	if (resultado == NULL)
	{
		clnt_perror(clnt, "call failed");
		return FALSE;
	}
	return TRUE;
}

// ******************************************** manejar servicios ********************************************

/**
 * @brief Gestiona la desconexión del administrador enviando la petición al servidor.
 *
 * Se invoca a `desconexion_1` usando el ID global `ida`. Si tiene éxito,
 * reinicia la variable a `-1`.
 *
 * @param[in,out] clnt  Handle del cliente RPC (struct CLIENT).
 */
static void maneja_desconexion(CLIENT *clnt)
{
	bool_t *resultado;
	if (ida == -1)
		return;

	resultado = desconexion_1(&ida, clnt);
	if (!comprueba_llamada(resultado, clnt))
		return;

	if (*resultado)
	{
		ida = -1;
		printf("Admin desconectado con éxito.\n");
	}
	else
		printf("Error desconectando admin\n");
}

/**
 * @brief Solicita al usuario el nombre de fichero y carga datos en el servidor.
 *
 * Comprueba primero si ya hay datos cargados en el servidor y pregunta al
 * usuario antes de sobrescribirlos. Luego llama a `cargardatos_1` para realizar
 * la carga remota del fichero especificado.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_carga_datos(CLIENT *clnt)
{

	// Comprobamos si el servidor tiene libros para avisar que se sobreescribe
	int *num_libros = nlibros_1(&ida, clnt);

	if (comprueba_llamada(num_libros, clnt) && *num_libros > 0)
	{
		char respuesta;
		printf("CUIDADO. Ya hay %d datos cargados, si continúa sobreescribirá los datos existentes.\n", *num_libros);
		printf("¿Desea continuar (s/N)?: ");
		scanf(" %c", &respuesta);

		// cualquier cosa que no sea 'S' o 's' se toma como un no
		if (tolower(respuesta) != 's')
		{
			printf("Operación abortada\n");
			return;
		}
	}

	// cargamos datos

	Cadena nombre_fichero;
	TFichero parametros_fichero;

	// pedimos nombre fichero
	printf("Introduzca nombre del fichero a cargar: ");
	scanf(" %s", parametros_fichero.NomFile);

	// Si se llama desde menú admin no debería 'ida' ser -1 -> no lo controlo
	// if (ida == -1)...
	parametros_fichero.Ida = ida;

	int *resultado = cargardatos_1(&parametros_fichero, clnt);

	if (!comprueba_llamada(resultado, clnt))
		return;

	switch (*resultado)
	{
	case 1:
		printf("Datos cargados y ordenados correctamente.\n");
		break;
	case 0:
		printf("Error abriendo fichero o error de memoria dinámica.\n");
		break;
	case -1:
		printf("No hay administrador válido o Ida no coincide con el servidor.\n");
		break;
	}
}

/**
 * @brief Guarda los datos actuales de la biblioteca desde el servidor a disco.
 *
 * Pide confirmación al usuario antes de sobrescribir el fichero de destino y
 * llama al procedimiento remoto `guardardatos_1`.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_guarda_datos(CLIENT *clnt)
{
	char respuesta;
	printf("ATENCIÓN. Guardando el archivo sobreescribirá los datos existentes en él.\n");
	printf("¿Desea continuar (s/N)?: ");
	scanf(" %c", &respuesta);

	// cualquier cosa que no sea 'S' o 's' se toma como un no
	if (tolower(respuesta) != 's')
	{
		printf("Operación abortada\n");
		return;
	}

	// guardamos datos
	bool_t *resultado = guardardatos_1(&ida, clnt);

	if (!comprueba_llamada(resultado, clnt))
		return;

	if (*resultado)
		printf("Archivo guardado con éxito\n");
	else
	{
		printf("Error. Posibles errores: \n- Id admin incorrecta o No hay administrador válido.\n");
		printf("- No hay fichero abierto.\n- Error abriendo o escribiendo archivo.\n");
	}
}

/**
 * @brief Recoge datos de un nuevo libro y solicita su alta al servidor.
 *
 * Lee los campos del nuevo libro desde la entrada estándar, inicializa los
 * contadores internos y llama al servicio remoto `nuevolibro_1`.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_nuevo_libro(CLIENT *clnt)
{

	// Pedimos datos
	TLibro libro;
	printf("Introduce el Isbn: ");
	scanf(" %s", libro.Isbn);
	printf("Introduce el Autor: ");
	scanf("%[^\n]", libro.Autor); // lee todo hasta retorno de carro
	printf("Introduce el Título: ");
	scanf("%[^\n]", libro.Titulo);
	printf("Introduce el Año: ");
	scanf("%d", &libro.Anio);
	printf("Introduce el País: ");
	scanf("%[^\n]", libro.Pais);
	printf("Introduce el Idioma: ");
	scanf("%s", libro.Idioma);
	printf("Introduce el Número de Libros Inidial: ");
	scanf("%d", &libro.NoLibros);
	libro.NoListaEspera = 0;
	libro.NoPrestados = 0;

	TNuevo consulta;
	consulta.Ida = ida;
	consulta.Libro = libro;

	// comprobamos si la llamada ha sido correcta
	int *resultado = nuevolibro_1(&consulta, clnt);
	if (!comprueba_llamada(resultado, clnt))
		return;

	switch (*resultado)
	{
	case -1:
		printf("No hay administrador válido o Ida no coincide con el servidor.\n");
		break;
	case 0:
		printf("El isbn ya se encuentra en el sistema. Abortando...\n");
		break;
	case 1:
		printf("*** El libro ha sido añadido correctamente.**\n");
		break;
	}
}

/**
 * @brief Gestiona la compra de nuevas unidades para un libro existente.
 *
 * Pide el ISBN del libro, busca su posición en el servidor y, si existe,
 * solicita al usuario la cantidad a comprar para actualizar el inventario.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_compra_libro(CLIENT *clnt)
{

	// comprobar si hay libros en biblioteca
	int *resultado = nlibros_1(&ida, clnt);

	if (!comprueba_llamada(resultado, clnt))
		return;

	if (*resultado == 0)
	{
		printf("\nLa biblioteca está vacía.\n");
		return;
	}

	// Buscamos libro
	TConsulta consulta;
	printf("Introduce Isbn a Buscar: ");
	scanf("%s", consulta.Isbn);
	consulta.Ida = ida;

	int *posicion = buscar_1(&consulta, clnt);
	if (!comprueba_llamada(posicion, clnt))
		return;

	switch (*posicion)
	{
	case -2:
		printf("No hay administrador válido o Ida no coincide con el servidor.\n");
		break;
	case -1:
		printf("El isbn %s no se encuentra en el sistema.\n", consulta.Isbn);
		break;
	default:
	{

		// Si está el libro, lo mostramos y pedimos confirmación
		TPosicion posicionConsulta;
		posicionConsulta.Ida = ida; // aunque no se use en el servidor.
		posicionConsulta.Pos = *posicion;

		TLibro *libro = descargar_1(&posicionConsulta, clnt);
		if (!comprueba_llamada(libro, clnt))
			return;

		MostrarLibro(libro, 0, TRUE);

		// Pedimos confirmación
		char respuesta;
		printf("¿ Es este el libro que deseas comprar más unidades (s/N) ? ");
		scanf(" %c", &respuesta);

		// cualquier cosa que no sea 'S' o 's' se toma como un no
		if (tolower(respuesta) != 's')
		{
			printf("Operación abortada\n");
			return;
		}
		TComRet numero_libros_consulta;
		numero_libros_consulta.Ida = ida; // arriba ya comprobamos que era ida válido
		strcpy(numero_libros_consulta.Isbn, libro->Isbn);

		printf("Introduce Número de Libros comprados: ");
		scanf("%d", &numero_libros_consulta.NoLibros);

		// llamamos al servicio
		resultado = comprar_1(&numero_libros_consulta, clnt);
		if (!comprueba_llamada(resultado, clnt))
			return;

		// ya se ha comprobado antes un ida válido y un isbn correcto
		// nunca debería llegar a entrar a los errores.

		switch (*resultado)
		{
		case -1:
			printf("No hay administrador válido o Ida no coincide con el servidor.\n");
			break;
		case 0:
			printf("El isbn %s no se encuentra en el sistema.\n", numero_libros_consulta.Isbn);
			break;
		case 1:
			printf("*** Se han añadido los nuevos libros.**\n");
			break;
		}
	}
	}
}

/**
 * @brief Gestiona la retirada de unidades de un libro del inventario.
 *
 * Pide el ISBN del libro, comprueba su existencia en el servidor y solicita
 * al usuario el número de unidades a retirar.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_retira_libro(CLIENT *clnt)
{

	// comprobar si hay libros en biblioteca
	int *resultado = nlibros_1(&ida, clnt);

	if (!comprueba_llamada(resultado, clnt))
		return;

	if (*resultado == 0)
	{
		printf("\nLa biblioteca está vacía.\n");
		return;
	}

	// Buscamos libro
	TConsulta consulta;
	printf("Introduce Isbn a Buscar: ");
	scanf("%s", consulta.Isbn);
	consulta.Ida = ida;

	int *posicion = buscar_1(&consulta, clnt);
	if (!comprueba_llamada(posicion, clnt))
		return;

	switch (*posicion)
	{
	case -2:
		printf("No hay administrador válido o Ida no coincide con el servidor.\n");
		break;
	case -1:
		printf("El isbn %s no se encuentra en el sistema.\n", consulta.Isbn);
		break;
	default:
	{

		// Si está el libro, lo mostramos y pedimos confirmación
		TPosicion posicionConsulta;
		posicionConsulta.Ida = ida; // aunque no se use en el servidor.
		posicionConsulta.Pos = *posicion;

		TLibro *libro = descargar_1(&posicionConsulta, clnt);
		if (!comprueba_llamada(libro, clnt))
			return;

		MostrarLibro(libro, 0, TRUE);

		// Pedimos confirmación
		char respuesta;
		printf("¿ Es este el libro que deseas retirar unidades (s/N) ? ");
		scanf(" %c", &respuesta);

		// cualquier cosa que no sea 'S' o 's' se toma como un no
		if (tolower(respuesta) != 's')
		{
			printf("Operación abortada\n");
			return;
		}
		TComRet numero_libros_consulta;
		numero_libros_consulta.Ida = ida; // arriba ya comprobamos que era ida válido
		strcpy(numero_libros_consulta.Isbn, libro->Isbn);

		printf("Introduce Número de unidades a retirar: ");
		scanf("%d", &numero_libros_consulta.NoLibros);

		// llamamos al servicio
		resultado = retirar_1(&numero_libros_consulta, clnt);
		if (!comprueba_llamada(resultado, clnt))
			return;

		// ya se ha comprobado antes un ida válido y un isbn correcto
		// nunca debería llegar a entrar a esos errores.

		switch (*resultado)
		{
		case -1:
			printf("No hay administrador válido o Ida no coincide con el servidor.\n");
			break;
		case 0:
			printf("El isbn %s no se encuentra en el sistema.\n", numero_libros_consulta.Isbn);
			break;
		case 1:
			printf("*** Se han retirado el número de libros indicados.**\n");
			break;
		case 2:
			printf("No hay suficientes ejemplares disponibles para ser retirados.\n");
			break;
		}
	}
	}
}

/**
 * @brief Solicita el criterio de ordenación y pide al servidor ordenar la lista.
 *
 * Muestra un menú con los campos disponibles para ordenar y llama al servicio
 * remoto `ordenar_1` con el criterio seleccionado.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_ordenar_libros(CLIENT *clnt)
{

	// Pedimos código
	int opcion;
	do
	{
		Cls;
		printf("Elige el campo de Ordenación\n");
		printf("\t0.- Por Isbn\n");
		printf("\t1.- Por Título\n");
		printf("\t2.- Por Autor\n");
		printf("\t3.- Por Año\n");
		printf("\t4.- Por País\n");
		printf("\t5.- Por Idioma\n");
		printf("\t6.- Por n° de libros Disponibles\n");
		printf("\t7.- Por n° de libros Prestados\n");
		printf("\t8.- Por n° de libros en Espera\n");
		printf("\t9.- Cancelar\n");
		printf("Introduce Código: ");
		scanf("%d", &opcion);
		if (opcion < 0 || opcion > 9)
			MostrarAviso("\n\n *** Error en la entrada de Datos.***\n\n");
	} while (opcion < 0 || opcion > 9);

	if (opcion == 9)
	{
		printf("Operación abortada\n");
		return;
	}

	TOrdenacion consulta_ordenacion;
	consulta_ordenacion.Ida = ida;
	consulta_ordenacion.Campo = opcion;

	bool_t *resultado = ordenar_1(&consulta_ordenacion, clnt);
	if (!comprueba_llamada(resultado, clnt))
		return;

	if (*resultado)
	{
		printf("*** La biblioteca ha sido ordenada correctamente.***\n");
	}
	else
	{
		printf("No hay administrador válido o Ida no coincide con el servidor.\n");
	}
}

/**
 * @brief Ejecuta una búsqueda completa en la biblioteca descargando libros.
 *
 * Itera cada posición de la biblioteca en el servidor, descarga el libro y
 * comprueba si el texto de búsqueda aparece en el campo seleccionado.
 * Muestra todos los resultados coincidentes en pantalla.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 * @param[in] texto_busqueda Subcadena a buscar en los campos del libro.
 * @param[in] campo_busqueda Identificador de campo ('i','t','a','p','d','*').
 * @param[in] numero_libros Número total de libros existentes en el servidor.
 * @return TRUE si se encontró al menos un libro coincidente, FALSE en caso contrario.
 */
static bool_t ejecutar_busqueda(CLIENT *clnt, char *texto_busqueda,
								const char campo_busqueda, const int numero_libros)
{
	// Recorremos los libros y comprobamos coincidencias
	TPosicion posicion_actual;
	posicion_actual.Ida = ida;
	TLibro *libro_actual;
	bool_t primer_encontrado = TRUE; // para imprimir la cabecera con el primero
	int contador_encontrados = 0;

	// recorremos todos los libros
	for (int i = 0; i < numero_libros; i++)
	{
		posicion_actual.Pos = i;
		libro_actual = descargar_1(&posicion_actual, clnt);

		if (comprueba_llamada(libro_actual, clnt))
		{
			if (comprobar_formateado(libro_actual, texto_busqueda, campo_busqueda))
			{
				MostrarLibro(libro_actual, i, primer_encontrado);
				primer_encontrado = FALSE;
				++contador_encontrados;
			}
		}
	}

	if (contador_encontrados == 0)
	{
		printf("\nNo se encontraron libros que coincidan con la búsqueda.\n");
		return FALSE;
	}
	else
	{
		printf("\nSe encontraron %d libro(s).\n", contador_encontrados);
		return TRUE;
	}
}


/**
 * @brief Lee los criterios de búsqueda del usuario y ejecuta la búsqueda.
 *
 * Solicita el texto de búsqueda y el filtro de campo al usuario. Si la
 * entrada es válida, llama a `ejecutar_busqueda` para mostrar los resultados.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 * @return TRUE si se encontraron libros coincidentes, FALSE si no o si hubo error.
 */
static bool_t maneja_buscar_libros(CLIENT *clnt)
{
	bool_t texto_encontrado = FALSE;
	// comprobar si hay libros en biblioteca
	int *num_libros = nlibros_1(&ida, clnt);

	if (!comprueba_llamada(num_libros, clnt))
		return FALSE;

	if (*num_libros == 0)
	{
		printf("\nLa biblioteca está vacía.\n");
		return FALSE;
	}

	// Pedimos texto
	Cadena texto_busqueda;
	printf("Introduce el texto a Buscar: ");
	scanf(" %[^\n]", texto_busqueda);

	// Pedimos código
	unsigned char campo_busqueda;
	bool_t salir = FALSE;
	do
	{
		printf("Código de búsqueda:\n");
		printf("\tI.- Por Isbn\n");
		printf("\tT.- Por Título\n");
		printf("\tA.- Por Autor\n");
		printf("\tP.- Por País\n");
		printf("\tD.- Por Idioma\n");
		printf("\t*.- Por todos los campos\n");
		printf("\t0.- Cancelar\n\n");
		printf("Introduce Código: ");
		scanf(" %c", &campo_busqueda);

		// Convertimos a minúsculas
		campo_busqueda = tolower(campo_busqueda);

		// comprobamos el resultado con las posibles opciones "itapd*0"
		salir = strchr("itapd*0", campo_busqueda) != NULL;

		if (!salir)
			MostrarAviso("\n\n *** Error en la entrada de Datos.***\n\n");

		Cls;
	} while (!salir);

	if (campo_busqueda == '0')
	{
		printf("Operación abortada\n");
		return FALSE;
	}

	return ejecutar_busqueda(clnt, texto_busqueda, campo_busqueda, *num_libros);
}

/**
 * @brief Gestiona la solicitud de préstamo de un libro.
 *
 * Lanza primero una búsqueda y, si se encuentra el libro, pide la posición y
 * llama a `prestar_1` para solicitar la transacción de préstamo al servidor.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_prestar_libro(CLIENT *clnt)
{
	// primero se encarga buscar_libros. Si devuelve algo entonces preguntamos.
	if (!maneja_buscar_libros(clnt))
		return;

	// preguntamos si quiere retirar
	char respuesta;
	printf("¿ Quieres sacar algún libro de la biblioteca (s/N) ? ");
	scanf(" %c", &respuesta);

	// cualquier cosa que no sea 'S' o 's' se toma como un no
	if (tolower(respuesta) != 's')
	{
		printf("Operación abortada\n");
		return;
	}

	// pedimos posicion
	TPosicion pocision_consulta;
	printf("Introduce la posición del libro a solicitar su préstamo: ");
	scanf("%d", &pocision_consulta.Pos);
	pocision_consulta.Pos -= 1; // primer elemento en pantalla es 1
	pocision_consulta.Ida = ida;

	int *resultado = prestar_1(&pocision_consulta, clnt);
	if (!comprueba_llamada(resultado, clnt))
		return;

	// manejamos respuestas
	switch (*resultado)
	{
	case -1:
		printf("La posición indicada no está dentro de los límites del vector.\n");
		break;
	case 0:
		printf("No hay libros, usuario puesto en lista de espera.\n");
		break;
	case 1:
		printf("*** El préstamo se ha concedido, recoge el libro en el mostrador.**\n");
		break;
	}
}

/**
 * @brief Gestiona la devolución de un libro por parte del usuario.
 *
 * Pide el ISBN para localizar el libro, solicita confirmación y llama a
 * `devolver_1` para procesar la devolución en el servidor.
 *
 * @param[in,out] clnt Handle del cliente RPC usado para comunicarse con el servidor.
 */
static void maneja_devolver_libro(CLIENT *clnt)
{
	// comprobar si hay libros en biblioteca
	int *num_libros = nlibros_1(&ida, clnt);

	if (!comprueba_llamada(num_libros, clnt))
		return;

	if (*num_libros == 0)
	{
		printf("\nLa biblioteca está vacía.\n");
		return;
	}

	// pedimos isbn
	Cadena isbn_pedido;
	printf("Introduce el Isbn a Buscar: ");
	scanf(" %s", isbn_pedido);

	// llamamos a busqueda
	if (!ejecutar_busqueda(clnt, isbn_pedido, 'i', *num_libros))
		return; // salimos si no hay libros con el resultado

	// preguntamos si quiere devolver
	char respuesta;
	printf("¿ Quieres devolver algún libro de la biblioteca (s/N) ? ");
	scanf(" %c", &respuesta);

	// cualquier cosa que no sea 'S' o 's' se toma como un no
	if (tolower(respuesta) != 's')
	{
		printf("Operación abortada\n");
		return;
	}

	// pedimos posicion
	TPosicion pocision_consulta;
	printf("Introduce la posición del libro a devolver: ");
	scanf("%d", &pocision_consulta.Pos);
	pocision_consulta.Pos -= 1; // primer elemento en pantalla es 1
	pocision_consulta.Ida = ida;

	int *resultado = devolver_1(&pocision_consulta, clnt);
	if (!comprueba_llamada(resultado, clnt))
		return;

	// manejamos respuestas
	switch (*resultado)
	{
	case -1:
		printf("La posición indicada no está dentro de los límites del vector.\n");
		break;
	case 0:
		printf("*** Se ha devuelto el libro y se ha entregado a otro usuario de la lista de espera.**\n");
		break;
	case 1:
		printf("*** Se ha devuelto el libro y se pondrá en la estantería.**\n");
		break;
	case 2:
		printf("Error. No se puede devolver porque no está prestado.\n");
		break;
	}
}

/**
 * @brief Lista todos los libros de la biblioteca por pantalla.
 *
 * Invoca al servicio nlibros_1 para saber la cantidad de libros y luego
 * itera llamando al servicio descargar_1 por cada posición, mostrando los
 * resultados por pantalla.
 *
 * @param[in,out] clnt  Handle del cliente RPC (struct CLIENT).
 */
static void maneja_listar_libros(CLIENT *clnt)
{
	int i;
	TPosicion posicion_actual;
	TLibro *libro;
	int *num_libros = nlibros_1(&ida, clnt);

	if (!comprueba_llamada(num_libros, clnt))
		return;

	if (*num_libros == 0)
	{
		printf("\nLa biblioteca está vacía.\n");
		return;
	}

	// MostrarAviso("Mostrando libros\n");
	posicion_actual.Ida = ida;
	for (i = 0; i < *num_libros; i++)
	{
		posicion_actual.Pos = i;

		libro = descargar_1(&posicion_actual, clnt);
		if (!comprueba_llamada(libro, clnt))
			return;

		// Muestra cabecera en el primer libro
		MostrarLibro(libro, i, (i == 0) ? TRUE : FALSE);
	}

	MostrarAviso("\n");
}

/**
 * @brief Implementa el bucle de opciones de administración y lanza las peticiones.
 *
 * Muestra el menú de administración del sistema repetidamente. Ante cada opción
 * realiza las acciones y llamadas RPC correspondientes hasta elegir 0 (desconectar).
 *
 * @param[in,out] clnt Handle del cliente para comunicación RPC (struct CLIENT).
 */
static void maneja_menu_admin(CLIENT *clnt)
{
	int opcion;

	do
	{
		opcion = MenuAdministracion();
		switch (opcion)
		{
		case 1:

			maneja_carga_datos(clnt);
			break;

		case 2:

			maneja_guarda_datos(clnt);
			break;

		case 3:
			maneja_nuevo_libro(clnt);
			break;
		case 4:
			maneja_compra_libro(clnt);
			break;
		case 5:
			maneja_retira_libro(clnt);
			break;
		case 6:
			maneja_ordenar_libros(clnt);
			break;
		case 7:
			maneja_buscar_libros(clnt);
			break;
		case 8:
			maneja_listar_libros(clnt);
			break;
		case 0:
			maneja_desconexion(clnt);
			break;
		default:
			break;
		}

	Pause;
	} while (opcion != 0);
}

/**
 * @brief Gestiona la conexión del administrador solicitando clave de acceso.
 *
 * Muestra el prompt, lee la clave, llama a la función `conexion_1` del
 * servidor y si recibe un id válido establece la variable global `ida`.
 *
 * @param[in,out] clnt El handle del cliente para RPC (struct CLIENT).
 */
static void maneja_conexion_admin(CLIENT *clnt)
{
	int *resultado;
	// pedir clave
	// Cadena clave;
	// printf("Introduzca la contraseña de asdministrador: ");
	// scanf("%s", clave);

	// TODO descomentar solicitar por teclado contraseña cuando todo esté acabado
	Cadena clave = "563498";

	resultado = conexion_1(clave, clnt);
	if (!comprueba_llamada(resultado, clnt))
		return;

	switch (*resultado)
	{
	case -1:
		MostrarAviso("Error. Ya hay un usuario identificado como administrador.\n");
		break;
	case -2:
		MostrarAviso("Error. Contraseña incorrecta.\n");
		break;
	default:
		MostrarAviso("Accediendo a menú administración.\n");
		ida = *resultado;
		maneja_menu_admin(clnt);
		break;
	}
}

/**
 * @brief Función esqueleto generada por rpcgen para probar las llamadas.
 *
 * Contiene un bloque de pruebas estandarizado. No es llamado obligatoriamente
 * durante el ciclo de vida real del cliente, pero es ilustrativo de los tipos
 * y argumentos que requiere cada llamada RPC.
 *
 * @param[in] host Puntero a cadena con la dirección/host del servidor RPC.
 */
void gestorbiblioteca_1(char *host)
{
	CLIENT *clnt;
	int *result_1;
	char conexion_1_arg;
	bool_t *result_2;
	int desconexion_1_arg;
	int *result_3;
	TFichero cargardatos_1_arg;
	bool_t *result_4;
	int guardardatos_1_arg;
	int *result_5;
	TNuevo nuevolibro_1_arg;
	int *result_6;
	TComRet comprar_1_arg;
	int *result_7;
	TComRet retirar_1_arg;
	bool_t *result_8;
	TOrdenacion ordenar_1_arg;
	int *result_9;
	int nlibros_1_arg;
	int *result_10;
	TConsulta buscar_1_arg;
	TLibro *result_11;
	TPosicion descargar_1_arg;
	int *result_12;
	TPosicion prestar_1_arg;
	int *result_13;
	TPosicion devolver_1_arg;

#ifndef DEBUG
	clnt = clnt_create(host, GESTORBIBLIOTECA, GESTORBIBLIOTECA_VER, "tcp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
#endif /* DEBUG */

	result_1 = conexion_1(&conexion_1_arg, clnt);
	if (result_1 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_2 = desconexion_1(&desconexion_1_arg, clnt);
	if (result_2 == (bool_t *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_3 = cargardatos_1(&cargardatos_1_arg, clnt);
	if (result_3 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_4 = guardardatos_1(&guardardatos_1_arg, clnt);
	if (result_4 == (bool_t *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_5 = nuevolibro_1(&nuevolibro_1_arg, clnt);
	if (result_5 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_6 = comprar_1(&comprar_1_arg, clnt);
	if (result_6 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_7 = retirar_1(&retirar_1_arg, clnt);
	if (result_7 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_8 = ordenar_1(&ordenar_1_arg, clnt);
	if (result_8 == (bool_t *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_9 = nlibros_1(&nlibros_1_arg, clnt);
	if (result_9 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_10 = buscar_1(&buscar_1_arg, clnt);
	if (result_10 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_11 = descargar_1(&descargar_1_arg, clnt);
	if (result_11 == (TLibro *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_12 = prestar_1(&prestar_1_arg, clnt);
	if (result_12 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_13 = devolver_1(&devolver_1_arg, clnt);
	if (result_13 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

/**
 * @brief Función de prueba automatizada para ahorrar tiempo insertando datos.
 * Llama a los servicios de modificar y devolver/prestar de forma hardcodeada.
 */
static void test_automatico(CLIENT *clnt)
{
	printf("\n--- INICIANDO TEST AUTOMÁTICO ---\n");
	int *res;

	// 1. Identificarse como admin
	Cadena clave = "12";
	res = conexion_1(clave, clnt);
	if (!comprueba_llamada(res, clnt))
		return;
	if (*res < 0)
	{
		printf("Fallo en login de admin.\n");
		return;
	}
	int id_test = *res;
	printf("[OK] Admin conectado (ID: %d).\n", id_test);

	// 2. Añadir nuevo libro
	TNuevo nLibro;
	nLibro.Ida = id_test;
	strcpy(nLibro.Libro.Isbn, "111");
	strcpy(nLibro.Libro.Titulo, "Libro de Prueba");
	strcpy(nLibro.Libro.Autor, "Pepe Autor");
	nLibro.Libro.Anio = 2024;
	strcpy(nLibro.Libro.Pais, "España");
	strcpy(nLibro.Libro.Idioma, "Español");
	nLibro.Libro.NoLibros = 2; // lo añadimos con 2 libros
	nLibro.Libro.NoPrestados = 0;
	nLibro.Libro.NoListaEspera = 0;

	res = nuevolibro_1(&nLibro, clnt);
	if (comprueba_llamada(res, clnt) && *res == 1)
		printf("[OK] Libro añadido.\n");
	else
		printf("[FAIL] Error añadiendo libro (res: %d).\n", *res);

	// 3. Comprar libros
	TComRet cLibro;
	cLibro.Ida = id_test;
	strcpy(cLibro.Isbn, "111");
	cLibro.NoLibros = 5; // Compramos 5 más
	res = comprar_1(&cLibro, clnt);
	if (comprueba_llamada(res, clnt) && *res == 1)
		printf("[OK] 5 libros comprados.\n");
	else
		printf("[FAIL] Error comprando libros (res: %d).\n", *res);

	// 4. Retirar 3 libros
	cLibro.NoLibros = 3;
	res = retirar_1(&cLibro, clnt);
	if (comprueba_llamada(res, clnt) && *res == 1)
		printf("[OK] 3 libros retirados.\n");
	else
		printf("[FAIL] Error retirando libros (res: %d).\n", *res);

	// 5. Prestar (requiere Posicion, así que primero lo buscamos)
	TConsulta bLibro;
	bLibro.Ida = id_test;
	strcpy(bLibro.Isbn, "111");
	int *resPos = buscar_1(&bLibro, clnt);
	if (comprueba_llamada(resPos, clnt) && *resPos >= 0)
	{
		printf("[OK] Libro encontrado en pos: %d.\n", *resPos);

		TPosicion pLibro;
		pLibro.Ida = id_test;
		pLibro.Pos = *resPos;

		// Prestar 1
		res = prestar_1(&pLibro, clnt);
		if (comprueba_llamada(res, clnt) && (*res == 1 || *res == 0))
		{
			if (*res == 1)
				printf("[OK] Libro prestado con éxito (entregado).\n");
			else
				printf("[OK] Libro solicitado (puesto en lista de espera).\n");
		}
		else
			printf("[FAIL] Error prestando libro (res: %d).\n", *res);

		// Devolver 1
		res = devolver_1(&pLibro, clnt);
		if (comprueba_llamada(res, clnt) && (*res == 1 || *res == 0))
		{
			if (*res == 1)
				printf("[OK] Libro devuelto con éxito (puesto en estantería).\n");
			else
				printf("[OK] Libro devuelto con éxito (entregado a lista de espera).\n");
		}
		else
			printf("[FAIL] Error devolviendo libro (res: %d).\n", *res);
	}
	else
	{
		printf("[FAIL] Error buscando el libro (res: %d).\n", *resPos);
	}

	// Terminar admin
	bool_t *resDesc = desconexion_1(&id_test, clnt);
	if (comprueba_llamada(resDesc, clnt) && *resDesc)
	{
		printf("[OK] Desconexión exitosa tras finalizar tests.\n");
	}

	printf("--- FIN DEL TEST AUTOMÁTICO ---\n");
	Pause;
}

/**
 * @brief Punto de entrada del cliente RPC al gestor de la biblioteca.
 *
 * Crea la comunicación RPC a través de `clnt_create`. Inicia el
 * bucle del menú principal del usuario.
 *
 * @param[in] argc Cantidad de argumentos pasados por consola.
 * @param[in] argv Array de cadenas desde terminal. Debe incluir `host`.
 * @return         Retorna 0 si la ejecución finaliza con éxito.
 */
int main(int argc, char *argv[])
{

	// ------------------------ iniciar conexcion ------------------------

	// Comprobamos si ha pasado un argumento de ejecución
	if (argc < 2)
	{
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}

	// obtenemos host desde argumento de ejecución
	char *host = argv[1];

	CLIENT *clnt = clnt_create(host, GESTORBIBLIOTECA, GESTORBIBLIOTECA_VER, "tcp");

	// Controlar fallo al crear
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
	MostrarAviso("Conexión con el servidor exitosa\n");

	// ------------------------ Ejecucion menu ------------------------

	// variables
	int *resultado_rpc;
	int opcion;
	int idAdmin;

	do
	{

		opcion = MenuPrincipal();
		switch (opcion)
		{
		case 1:
			maneja_conexion_admin(clnt);
			break;
		case 2:
			maneja_buscar_libros(clnt);
			break;
		case 3:
			maneja_prestar_libro(clnt);
			break;
		case 4:
			maneja_devolver_libro(clnt);
			break;
		case 9: // Para probar servicios
			test_automatico(clnt);
			break;
		default:
			// MenuPrincipal() ya controla que opcion esté acotada a las opciones del switch
			break;
		}
		Pause;

	} while (opcion != 0);

	clnt_destroy(clnt);
	exit(0);
}
