/**
 * @file   GestorBiblioteca_server.c
 * @brief  Implementación de los procedimientos del servidor RPC para la Biblioteca.
 *
 * Contiene la lógica de negocio invocada por el dispatcher generado
 * por rpcgen. Cada función corresponde a un procedimiento declarado
 * en el fichero .x del protocolo.
 *
 * @author Javi
 */

/**
 * @mainpage SD Servicio RPC - Documentación
 *
 * - @ref GestorBiblioteca_server.c "Documentación del Servidor (ver funciones)"
 * - @ref GestorBiblioteca_client.c "Documentación del Cliente (ver funciones)"
 *
 */

#include "GestorBiblioteca.h"
#include <string.h>

// establece la codificación del sistema al programa
#include "autolocale.h"

// ************************************ Variables globales ************************************
TLibro *Biblioteca = NULL;				/**< Vector dinámico de libros */
int NumLibros = 0;						/**< Número de libros almacenados en el vector dinámico. */
static const int INCREMENTO_TAMA = 4;	/**< Indica de cuánto en cuánto debe incrementar la variable Tama */
int Tama = 0;							/**< Tamaño del vector dinámico. El incremento será por bloques de 4 libros. */
int IdAdmin = -1;						/**< Identificador de Administración enviado al usuario. */
const Cadena RUTA_FICHEROS = "./data/"; /**< Ruta relativa donde se alojan los ficheros de datos (ej. "./" para la raíz o "./data/" para una subcarpeta). */
Cadena NomFichero = "";					/**< Nombre del último fichero binario que se ha cargado en memoria. */
int CampoOrdenacion = 0;				/**< Campo de ordenación por el que se ordenarán los libros. */

static const char *clave_admin = "563498"; /**< Constraseña de asministrador solicitada por el servidor */
// *********************************** Funciones auxiliares ***********************************

/**
 * @brief Genera un identificador aleatorio para el administrador.
 *
 * Genera un número aleatorio y lo establece en la variable global IdAdmin.
 *
 * @return Identificador aleatorio generado.
 */
static int genera_id_admin()
{
	IdAdmin = 1 + rand() % RAND_MAX;

	return IdAdmin;
}

/**
 * @brief Comprueba si el identificador pasado coincide con el del administrador.
 *
 * @param[in] id  Identificador de sesión a comprobar.
 * @return        TRUE si coincide con IdAdmin y es mayor de 0, FALSE en caso contrario.
 */
static bool_t comprueda_id_admin(int id)
{
	return (id == IdAdmin && id > 0);
}

/**
 * @brief Ajusta el tamaño reservado para el vector dinámico de la biblioteca.
 *
 * Calcula el tamaño necesario para albergar todos los libros en bloques
 * de INCREMENTO_TAMA y actualiza la variable global Tama.
 *
 * @param[in] incremento TRUE si se va a añadir un libro (incrementar), FALSE si se va a eliminar (disminuir).
 *
 * @warning: La variable global NumLibros debe actualizarse (+1 o -1) DESPUÉS
 * de invocar a este método, una vez se haya modificado el vector.
 */
static void redimensiona_biblioteca(bool_t incremento)
{

	// +1 si se quiere aumentar el tamaño o -1 si se quiere disminuir
	int numeroIncremento = incremento ? 1 : -1;

	// misma lógica que al cargar archivo -> Nos da el múltiplo de INCREMENTO_TAMA superior al numero de libros

	// NumLibros + 1 o -1 dependiendo de si se incrementa o disminuye la biblioteca
	int tamanyoNuevo = ((NumLibros + numeroIncremento + INCREMENTO_TAMA - 1) / INCREMENTO_TAMA) * INCREMENTO_TAMA;

	// Si el tamaño nuevo no varía no hace falta redimensionar
	if (tamanyoNuevo != Tama)
	{
		Tama = tamanyoNuevo;

		TLibro *BibliotecaAuxiliar = (TLibro *)realloc(Biblioteca, sizeof(TLibro) * Tama);

		if (BibliotecaAuxiliar == NULL)
		{
			printf("Error. No ha sido posible reasignar memoria.\n");
			return;
		}

		Biblioteca = BibliotecaAuxiliar;
	}
}

/**
 * @brief Variante de la función del profesor adaptada para usar qsort().
 *
 * Función comparadora que lee la variable global CampoOrdenacion y devuelve
 * negativo, cero o positivo según convenga, como requiere stdlib.
 *
 * @param[in] a  Puntero genérico al primer libro (const void*).
 * @param[in] b  Puntero genérico al segundo libro (const void*).
 * @return       <0 si 'a' va antes, 0 si son iguales, >0 si 'a' va después.
 */
static int comparar_libros(const void *a, const void *b)
{
	// 1. Convertir los punteros genéricos (void*) al tipo de tu estructura
	const TLibro *libroA = (const TLibro *)a;
	const TLibro *libroB = (const TLibro *)b;
	int salida;

	// Como strcmp ya hace eso (0 si iguales, >0 si libroA va despues y <0 si libroA va antes)
	// Para los enteros hago una resta y obtenemos lo mismo que con strcmp.
	switch (CampoOrdenacion)
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
	default: // por si CampoOrdenacion es nulo o algo no contemplado
		// dejamos todo como está.
		salida = 0;
		break;
	}

	return salida;
}

/**
 * @brief Ordena el vector dinámico de libros usando qsort.
 *
 * Invoca a la función qsort de la biblioteca estándar de C para ordenar
 * el vector Biblioteca utilizando la función comparar_libros. El criterio de
 * ordenación vendrá dado por la variable global CampoOrdenacion.
 */
static void ordena_biblioteca()
{
	// qsort(array, numero_de_elementos, tamaño_del_elemento, funcion_comparadora);
	qsort(Biblioteca, NumLibros, sizeof(TLibro), comparar_libros);
}

/**
 * @brief Guarda la biblioteca actual en el fichero binario asociado.
 *
 * Utiliza el nombre de fichero almacenado en NomFichero y reemplaza su
 * contenido con los elementos que actualmente están en el vector dinámico Biblioteca.
 *
 * @return TRUE si la escritura fue exitosa, FALSE en caso de error.
 */
static bool_t guardar_fichero_datos()
{

	if (strlen(NomFichero) == 0)
	{
		printf("No hay ningún fichero abierto. Abra primero una Biblioteca.\n");
		return FALSE;
	}

	Cadena rutaCompleta;
	strcpy(rutaCompleta, RUTA_FICHEROS);
	strcat(rutaCompleta, NomFichero);
	printf("Ruta del archivo a guardar: %s", rutaCompleta);

	FILE *archivo = fopen(rutaCompleta, "wb");

	// si falla devuelve NULL
	if (archivo == NULL)
	{
		printf("Error abriendo archivo %s", rutaCompleta);
		return FALSE;
	}

	printf("Archivo %s abierto con éxito. Escribiendo...", rutaCompleta);

	// Estructura de archivo
	// 	| N (num de Libros) | Libro 1 | Libro 2 | ··· | Libro N-1 | Libro N |
	//  | ----------------- | ------- | ------- | --- | --------- | ------- |

	int resultado_escritura = fwrite(&NumLibros, sizeof(int), 1, archivo);

	// fwrite devuelve el número de elementos escritos
	if (resultado_escritura != 1)
	{
		printf("Error escribiendo elementos de %s", rutaCompleta);
		fclose(archivo);
		return FALSE;
	}

	resultado_escritura = fwrite(Biblioteca, sizeof(TLibro), NumLibros, archivo);

	if (resultado_escritura != NumLibros)
	{
		printf("Error escribiendo elementos de %s", rutaCompleta);
		fclose(archivo);
		return FALSE;
	}

	fclose(archivo);
	return TRUE;
}

/**
 * @brief Carga en memoria dinámica el contenido de la biblioteca desde un fichero.
 *
 * Libera cualquier biblioteca previamente cargada. Después, lee el fichero
 * indicado en bloques mediante la constante INCREMENTO_TAMA, guarda sus datos en
 * el vector dinámico Biblioteca y actualiza las variables globales necesarias.
 *
 * @param[in] nombreArchivo  Nombre del fichero binario a cargar (con ruta relativa).
 * @return                   1 si la carga y asignación de memoria fueron exitosas, 0 en caso de error.
 */
static int cargar_fichero_datos(const char *nombreArchivo)
{
	// Liberamos vector dinámico si ya hay uno
	if (Biblioteca != NULL)
	{
		free(Biblioteca);
		Biblioteca = NULL;
		NumLibros = 0;
		Tama = 0;
	}
	printf("Vector dinámico liberado\n");

	// Cargar nuevo archivo

	Cadena rutaCompleta;
	strcpy(rutaCompleta, RUTA_FICHEROS);
	strcat(rutaCompleta, nombreArchivo);
	printf("Ruta del archivo a abrir: %s\n", rutaCompleta);

	FILE *archivo = fopen(rutaCompleta, "rb");

	// si falla devuelve NULL
	if (archivo == NULL)
	{
		printf("Error abriendo archivo %s\n", rutaCompleta);
		return 0;
	}

	printf("Archivo %s abierto con éxito. Leyendo...\n", rutaCompleta);

	// Estructura de archivo
	// 	| N (num de Libros) | Libro 1 | Libro 2 | ··· | Libro N-1 | Libro N |
	//  | ----------------- | ------- | ------- | --- | --------- | ------- |

	// variables temporales por si hay fallo al crear vector dinámico
	int numero_libros, tamanyo_vector;
	TLibro *Biblioteca_auxiliar;

	int resultado_lectura = fread(&numero_libros, sizeof(int), 1, archivo);

	// fread devuelve el número de elementos leídos.
	if (resultado_lectura != 1)
	{
		printf("Error leyendo elementos de %s\n", rutaCompleta);
		fclose(archivo);
		return 0;
	}

	tamanyo_vector = ((numero_libros + INCREMENTO_TAMA - 1) / INCREMENTO_TAMA) * INCREMENTO_TAMA;
	Biblioteca_auxiliar = (TLibro *)malloc(sizeof(TLibro) * tamanyo_vector);

	if (Biblioteca_auxiliar == NULL)
	{
		printf("Error reservando memoria para vector de archivo %s\n", rutaCompleta);
		fclose(archivo);
		return 0;
	}

	resultado_lectura = fread(Biblioteca_auxiliar, sizeof(TLibro), numero_libros, archivo);

	if (resultado_lectura != numero_libros)
	{
		printf("Error leyendo elementos de %s\n", rutaCompleta);
		free(Biblioteca_auxiliar);
		fclose(archivo);
		return 0;
	}

	printf("Biblioteca %s cargada con exito\n", rutaCompleta);
	Biblioteca = Biblioteca_auxiliar;
	NumLibros = numero_libros;
	Tama = tamanyo_vector;
	strcpy(NomFichero, nombreArchivo);
	fclose(archivo);

	// al cargar un fichero puede haber libros disponibles y lista de espera. Los asignamos directamente
	for (int i = 0; i < NumLibros; i++)
	{
		TLibro *libro = &Biblioteca[i];
		if (libro->NoListaEspera > 0 && libro->NoLibros > 0)
		{
			if (libro->NoLibros >= libro->NoListaEspera)
			{
				libro->NoLibros -= libro->NoListaEspera;
				libro->NoPrestados += libro->NoListaEspera;
				libro->NoListaEspera = 0;
			}
			else
			{
				libro->NoListaEspera -= libro->NoLibros;
				libro->NoPrestados += libro->NoLibros;
				libro->NoLibros = 0;
			}
		}
	}

	ordena_biblioteca();

	return 1;
}

/**
 * @brief Busca un libro en la biblioteca por su ISBN.
 *
 * Itera sobre el vector dinámico de libros para encontrar una coincidencia
 * con el ISBN proporcionado.
 *
 * @param[in] isbn  ISBN del libro que se desea buscar.
 * @return          La posición del libro en el array si se encuentra, -1 en caso contrario.
 */
static int buscar_libro(const char *isbn)
{

	for (int i = 0; i < NumLibros; i++)
	{
		if (strcmp(isbn, Biblioteca[i].Isbn) == 0)
		{
			return i;
		}
	}

	return -1;
}

/**
 * @brief ORIGINAL DEL PROFESOR. Función para comparar dos libros según un campo.
 *
 * @warning NO UTILIZAR EN ESTA IMPLEMENTACIÓN. Se mantiene únicamente
 * por motivos de compatibilidad o por si el profesor la usa en el script de comprobación de servicios.
 * En su lugar, se utiliza comparar_libros() para llamarla con qsort.
 *
 * @param[in] P1     Posición del primer libro en el vector.
 * @param[in] P2     Posición del segundo libro en el vector.
 * @param[in] Campo  Campo de ordenación.
 * @return           TRUE si el libro en P1 es menor que el de P2, FALSE en caso contrario.
 * @see comparar_libros
 */
bool_t EsMenor(int P1, int P2, int Campo)
{
	bool_t salida = FALSE;
	TLibro L1 = Biblioteca[P1];
	TLibro L2 = Biblioteca[P2];

	switch (Campo)
	{
	case 0:
		salida = strcmp(L1.Isbn, L2.Isbn) < 0 ? TRUE : FALSE;
		break;
	case 1:
		salida = strcmp(L1.Titulo, L2.Titulo) < 0 ? TRUE : FALSE;
		break;
	case 2:
		salida = strcmp(L1.Autor, L2.Autor) < 0 ? TRUE : FALSE;
		break;
	case 3:
		salida = L1.Anio < L2.Anio ? TRUE : FALSE;
		break;
	case 4:
		salida = strcmp(L1.Pais, L2.Pais) < 0 ? TRUE : FALSE;
		break;
	case 5:
		salida = strcmp(L1.Idioma, L2.Idioma) < 0 ? TRUE : FALSE;
		break;
	case 6:
		salida = L1.NoLibros < L2.NoLibros ? TRUE : FALSE;
		break;
	case 7:
		salida = L1.NoPrestados < L2.NoPrestados ? TRUE : FALSE;
		break;
	case 8:
		salida = L1.NoListaEspera < L2.NoListaEspera ? TRUE : FALSE;
		break;
	}
	return salida;
}

// *********************************** Servicios del servidor ***********************************

/**
 * @brief Conecta a un administrador verificando su contraseña.
 *
 * Verificará que la contraseña enviada coincida con la del administrador (563498)
 * y devolverá un número dependiendo de varias condiciones.
 *
 * @param[in] argp   Puntero a la contraseña (cadena) enviada por el cliente.
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: Ya hay un usuario identificado como administrador.
 *                   - -2: La contraseña es errónea.
 *                   - N (>0): Identificador aleatorio único de administrador.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *conexion_1_svc(char *argp, struct svc_req *rqstp)
{
	static int result;

	result = IdAdmin != -1 ? -1 : (strcmp(argp, clave_admin) != 0 ? -2 : genera_id_admin());

	return &result;
}

/**
 * @brief Desconecta al administrador actual.
 *
 * Comprueba que el Ida pasado coincide con el almacenado en el servidor.
 * Si coincide, borrará el Ida almacenado (lo pone a -1) y liberará el acceso.
 *
 * @param[in] argp   Puntero al Identificador de Administración (Ida).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a booleano: TRUE si se desconectó con éxito, FALSE en caso contrario.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
bool_t *desconexion_1_svc(int *argp, struct svc_req *rqstp)
{
	static bool_t result;

	if (*argp == IdAdmin)
	{
		IdAdmin = -1;
		result = TRUE;
	}
	else
		result = FALSE;

	return &result;
}

/**
 * @brief Carga los datos de los libros desde un fichero binario.
 *
 * Abrirá el fichero con el nombre indicado, leerá su contenido a memoria dinámica,
 * y finalmente lo cerrará. Verifica previamente que el Ida sea válido.
 * Una vez cargados los datos, los ordenará según CampoOrdenacion.
 *
 * @param[in] argp   Puntero a estructura TFichero con Ida y el nombre del fichero (NomFile).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: No hay administrador válido o Ida no coincide con el servidor.
 *                   -  0: Error abriendo fichero o error de memoria dinámica.
 *                   -  1: Datos cargados y ordenados correctamente.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *cargardatos_1_svc(TFichero *argp, struct svc_req *rqstp)
{
	static int result;

	if (!comprueda_id_admin(argp->Ida))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = -1;
	}
	else
	{
		result = cargar_fichero_datos(argp->NomFile);
	}

	return &result;
}

/**
 * @brief Guarda los datos del vector dinámico en un fichero.
 *
 * Guarda el estado actual de la biblioteca en el mismo fichero desde el que
 * se cargaron los datos (cuyo nombre está almacenado en el servidor).
 * Se verifica previamente que el Ida sea válido.
 *
 * @param[in] argp   Puntero al Identificador de Administración (Ida).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a booleano: TRUE si se ha guardado correctamente, FALSE en caso contrario.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
bool_t *guardardatos_1_svc(int *argp, struct svc_req *rqstp)
{
	static bool_t result;

	if (!comprueda_id_admin(*argp))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = FALSE;
	}
	else
	{
		result = guardar_fichero_datos();
	}

	return &result;
}

/**
 * @brief Añade un nuevo libro al vector dinámico.
 *
 * Verifica el Ida y que ningún libro previo tenga el mismo ISBN. Si es correcto,
 * inserta el nuevo libro y luego ordena el vector dinámico usando el
 * CampoOrdenacion almacenado en el servidor.
 *
 * @param[in] argp   Puntero a estructura TNuevo que contiene el Ida y los datos del Libro.
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: No hay administrador válido o Ida erróneo.
 *                   -  0: Ya existe un libro con ese ISBN en el vector.
 *                   -  1: Libro añadido y vector reordenado correctamente.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *nuevolibro_1_svc(TNuevo *argp, struct svc_req *rqstp)
{
	static int result;

	if (!comprueda_id_admin(argp->Ida))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = -1;
	}
	else if (buscar_libro(argp->Libro.Isbn) != -1)
	{
		printf("El isbn del libro que se está intentando añadir ya está registrado.\n");
		result = 0;
	}
	else
	{
		// redimensiona si es necesario para añadir un elemento más al vector
		redimensiona_biblioteca(TRUE);

		Biblioteca[NumLibros] = argp->Libro;
		++NumLibros;
		printf("Libro añadido por admin (id:%d) con exito a la biblioteca.\n", IdAdmin);
		result = 1;
	}

	ordena_biblioteca();

	return &result;
}

/**
 * @brief Compra (añade) ejemplares a un libro existente.
 *
 * Añade ejemplares al libro y, si había usuarios en lista de espera,
 * reduce la espera entregando los nuevos libros. Al finalizar, reordena
 * el vector dinámico. Verifica el Ida previamente.
 *
 * @param[in] argp   Puntero a estructura TComRet con Ida, ISBN y número de libros.
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: No hay administrador válido o Ida erróneo.
 *                   -  0: No se encuentra ningún libro con ese ISBN.
 *                   -  1: Ejemplares agregados, listas actualizadas y datos ordenados.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *comprar_1_svc(TComRet *argp, struct svc_req *rqstp)
{
	static int result;
	int posLibro;

	if (!comprueda_id_admin(argp->Ida))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = -1;
	}
	else if ((posLibro = buscar_libro(argp->Isbn)) == -1)
	{
		printf("El isbn del libro que se está intentando añadir ya está registrado.\n");
		result = 0;
	}
	else
	{
		// obtenermos el Libro para no tener que llamar al vector todo el rato
		TLibro *libroAuxiliar = &Biblioteca[posLibro];

		// incrementamos el numero de libros
		libroAuxiliar->NoLibros += argp->NoLibros;

		// Asignamos libros si hay gente en espera
		if (libroAuxiliar->NoListaEspera != 0)
		{
			// Si hay más libros que gente en espera
			if (argp->NoLibros >= libroAuxiliar->NoListaEspera)
			{
				libroAuxiliar->NoLibros -= libroAuxiliar->NoListaEspera;
				libroAuxiliar->NoPrestados += libroAuxiliar->NoListaEspera;
				libroAuxiliar->NoListaEspera = 0;

			} // Si hay más gente en espera que libros
			else
			{
				libroAuxiliar->NoListaEspera -= libroAuxiliar->NoLibros;
				libroAuxiliar->NoPrestados += libroAuxiliar->NoLibros;
				libroAuxiliar->NoLibros = 0;
			}
		}

		printf("stock añadido al libro(isbn: %s) por admin (id:%d) con exito a la biblioteca.\n", argp->Isbn, IdAdmin);
		result = 1;
	}

	ordena_biblioteca();

	return &result;
}

/**
 * @brief Retira ejemplares de un libro existente.
 *
 * Reduce el número de ejemplares disponibles de un libro y reordena el vector
 * dinámico. Verifica el Ida, busca el libro y comprueba que hay suficientes
 * ejemplares disponibles antes de retirarlos.
 *
 * @param[in] argp   Puntero a estructura TComRet con Ida, ISBN y número de libros a retirar.
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: No hay administrador válido o Ida erróneo.
 *                   -  0: No se encuentra ningún libro con ese ISBN.
 *                   -  1: Ejemplares retirados con éxito y datos ordenados.
 *                   -  2: No hay suficientes ejemplares disponibles para ser retirados.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *retirar_1_svc(TComRet *argp, struct svc_req *rqstp)
{
	static int result;
	int posLibro;

	if (!comprueda_id_admin(argp->Ida))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = -1;
	}
	else if ((posLibro = buscar_libro(argp->Isbn)) == -1)
	{
		printf("El isbn del libro que se está intentando añadir ya está registrado.\n");
		result = 0;
	}
	else
	{
		// obtenermos el Libro para no tener que llamar al vector todo el rato
		TLibro *libroAuxiliar = &Biblioteca[posLibro];

		if (libroAuxiliar->NoLibros < argp->NoLibros)
		{
			printf("Error. No hay suficientes libros para retirar. Stock actual: %d | libros a retirar: %d.\n", libroAuxiliar->NoLibros, argp->NoLibros);
			result = 2;
		}
		else
		{
			libroAuxiliar->NoLibros -= argp->NoLibros;
			printf("stock del libro(isbn: %s) actualizado por admin (id:%d) con exito.\n", argp->Isbn, IdAdmin);
			result = 1;
		}
	}

	ordena_biblioteca();

	return &result;
}

/**
 * @brief Ordena el vector dinámico de libros por un campo específico.
 *
 * Realiza una ordenación del vector y almacena el campo por el cual
 * se ordenó para futuras inserciones o modificaciones.
 *
 * @param[in] argp   Puntero a estructura TOrdenacion con Ida y el Campo a usar.
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a booleano:
 *                   - FALSE: No hay administrador válido o Ida erróneo.
 *                   - TRUE: Vector ordenado correctamente.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
bool_t *ordenar_1_svc(TOrdenacion *argp, struct svc_req *rqstp)
{
	static bool_t result;

	if (!comprueda_id_admin(argp->Ida))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = FALSE;
	}
	else
	{
		// Si no hay libros, no hay nada que ordenar
		if (NumLibros != 0)
		{
			// asignamos el nuevo campo de ordenación y llamamos al método ordena_biblioteca();
			CampoOrdenacion = argp->Campo;
			ordena_biblioteca();
		}
		result = TRUE;
	}

	return &result;
}

/**
 * @brief Consulta el número total de libros en la biblioteca.
 *
 * Devuelve siempre el número de libros actualmente almacenados en el
 * vector dinámico (NumLibros). No verifica el Ida pasado por parámetro.
 *
 * @param[in] argp   Puntero al Identificador de Administración (Ida).
 * @param rqstp      Estructura de petición de rpcgen.
 * @return           Puntero a entero con el número de libros actual.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *nlibros_1_svc(int *argp, struct svc_req *rqstp)
{
	return &NumLibros;
}

/**
 * @brief Busca la posición de un libro en el vector a partir de su ISBN.
 *
 * @param[in] argp   Puntero a estructura TConsulta con Ida e ISBN buscado.
 * @param rqstp      Estructura de petición de rpcgen.
 * @return           Puntero a entero con el resultado:
 *                   - -2: No hay administrador válido o Ida erróneo.
 *                   - -1: No se ha encontrado ningún libro con el ISBN indicado.
 *                   - >=0: Posición en el vector del libro buscado.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *buscar_1_svc(TConsulta *argp, struct svc_req *rqstp)
{
	static int result;

	if (!comprueda_id_admin(argp->Ida))
	{
		printf("Id admin incorrecta o No hay administrador válido.\n");
		result = -2;
	}
	else
	{
		result = buscar_libro(argp->Isbn);
	}

	return &result;
}

/**
 * @brief Descarga los datos de un libro en una posición concreta.
 *
 * Extrae y devuelve la estructura TLibro almacenada en la posición 'Pos'.
 * Si la posición es incorrecta, devuelve un libro con "????" en campos texto y 0 en numéricos.
 * Si la posición es correcta pero el Ida no lo es, devuelve el libro pero ocultando
 * (poniendo a 0) los campos NoPrestados y NoListaEspera.
 *
 * @param[in] argp   Puntero a estructura TPosicion con Ida y posición (Pos).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a la estructura de TLibro descargado (con posibles bloqueos de seguridad).
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
TLibro *descargar_1_svc(TPosicion *argp, struct svc_req *rqstp)
{
	static TLibro result;

	// Comprobar si la posición es correcta
	if (argp->Pos < 0 || argp->Pos >= NumLibros)
	{
		strcpy(result.Isbn, "????");
		strcpy(result.Titulo, "????");
		strcpy(result.Autor, "????");
		result.Anio = 0;
		strcpy(result.Pais, "????");
		strcpy(result.Idioma, "????");
		result.NoLibros = 0;
		result.NoPrestados = 0;
		result.NoListaEspera = 0;
	}
	else
	{
		result = Biblioteca[argp->Pos];

		if (!comprueda_id_admin(argp->Ida))
		{
			result.NoPrestados = 0;
			result.NoListaEspera = 0;
		}
	}

	return &result;
}

/**
 * @brief Presta un libro a un usuario.
 *
 * Presta el libro de la posición indicada. Si hay ejemplares disponibles,
 * reduce el disponible y aumenta prestados. Si no, añade usuario a
 * lista de espera. Reordena el vector al finalizar. No pide verificación del Ida.
 *
 * @param[in] argp   Puntero a estructura TPosicion con Pos (y un Ida irrelevante/anónimo).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: La posición indicada no está dentro de los límites del vector.
 *                   -  0: No hay libros, usuario puesto en lista de espera.
 *                   -  1: Libro prestado correctamente.
 *
 * @warning Asumo que la Pos pasada por parámetro va de [0 a NoLibros-1]
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *prestar_1_svc(TPosicion *argp, struct svc_req *rqstp)
{
	static int result;

	// Early return para posición incorrecta
	if (argp->Pos < 0 || argp->Pos >= NumLibros)
	{
		printf("Error. Se está pasando una posición incorrecta en el servicio `prestar`.\n");
		result = -1;
		return &result;
	}

	// obtenermos el Libro para no tener que llamar al vector todo el rato
	TLibro *libroAuxiliar = &Biblioteca[argp->Pos];

	if (libroAuxiliar->NoLibros == 0)
	{
		++libroAuxiliar->NoListaEspera;
		result = 0;
	}
	else
	{
		--libroAuxiliar->NoLibros;
		++libroAuxiliar->NoPrestados;
		result = 1;
	}

	ordena_biblioteca();

	return &result;
}

/**
 * @brief Devuelve un ejemplar de un libro de la biblioteca.
 *
 * Realiza la devolución de un libro en la posición indicada. Si hay usuarios
 * en lista de espera, se les entrega directamente (reduciendo la lista en uno).
 * Si no hay lista, aumenta el saldo de libros disponibles. Reordena al finalizar.
 *
 * @param[in] argp   Puntero a estructura TPosicion con Pos (y un Ida irrelevante/anónimo).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: La posición indicada no está dentro de los límites del vector.
 *                   -  0: Libro devuelto y entregado a un usuario de la lista de espera.
 *                   -  1: Libro devuelto, aumentando el número de ejemplares disponibles.
 *                   -  2: Error, no se puede devolver porque no está prestado.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *devolver_1_svc(TPosicion *argp, struct svc_req *rqstp)
{
	static int result;

	// Early return para posición incorrecta
	if (argp->Pos < 0 || argp->Pos >= NumLibros)
	{
		printf("Error. Se está pasando una posición incorrecta en el servicio `devolver`.\n");
		result = -1;
		return &result;
	}

	// obtenermos el Libro para no tener que llamar al vector todo el rato
	TLibro *libroAuxiliar = &Biblioteca[argp->Pos];

	// En el enunciado dice "El libro no se puede devolver, porque no hay ni usuarios en lista de espera ni libros
	//  prestados"
	// Si no hay libros prestados, debería dar igual si hay, o no, lista de espera, ¿no?
	// En caso de devolver 2, solo contemplo que no haya libros prestados
	if (libroAuxiliar->NoPrestados < 1)
	{
		result = 2;
	}
	else if (libroAuxiliar->NoListaEspera > 0)
	{
		--libroAuxiliar->NoListaEspera;
		result = 0;
	}
	else
	{
		++libroAuxiliar->NoLibros;
		--libroAuxiliar->NoPrestados;
		result = 1;
	}

	ordena_biblioteca();

	return &result;
}
