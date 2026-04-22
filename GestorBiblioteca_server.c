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

// TODO actualizar clave a la dada por el profesor 563498
static const char *clave_admin = "12"; /**< Constraseña de asministrador solicitada por el servidor */
// *********************************** Funciones auxiliares ***********************************

/**
 * @brief Genera una id aleatoria para administrador y la establece
 * en la variable global IdAdmin
 *
 * @return id aleatoria que ha sido generada
 */
static int genera_id_admin()
{
	IdAdmin = 1 + rand() % RAND_MAX;

	return IdAdmin;
}

static bool_t comprueda_id_admin(int id)
{
	return (id == IdAdmin && id > 0);
}
static bool_t guardar_fichero_datos()
{

	if(strlen(NomFichero) == 0){
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
	printf("Ruta del archivo a abrir: %s", rutaCompleta);

	FILE *archivo = fopen(rutaCompleta, "rb");

	// si falla devuelve NULL
	if (archivo == NULL)
	{
		printf("Error abriendo archivo %s", rutaCompleta);
		return 0;
	}

	printf("Archivo %s abierto con éxito. Leyendo...", rutaCompleta);

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
		printf("Error leyendo elementos de %s", rutaCompleta);
		fclose(archivo);
		return 0;
	}

	tamanyo_vector = ((numero_libros + INCREMENTO_TAMA - 1) / INCREMENTO_TAMA) * INCREMENTO_TAMA;
	Biblioteca_auxiliar = (TLibro *)malloc(sizeof(TLibro) * tamanyo_vector);

	if (Biblioteca_auxiliar == NULL)
	{
		printf("Error reservando memoria para vector de archivo %s", rutaCompleta);
		fclose(archivo);
		return 0;
	}

	resultado_lectura = fread(Biblioteca_auxiliar, sizeof(TLibro), numero_libros, archivo);

	if (resultado_lectura != numero_libros)
	{
		printf("Error leyendo elementos de %s", rutaCompleta);
		free(Biblioteca_auxiliar);
		fclose(archivo);
		return 0;
	}

	printf("Biblioteca %s cargada con exito", rutaCompleta);
	Biblioteca = Biblioteca_auxiliar;
	NumLibros = numero_libros;
	Tama = tamanyo_vector;
	strcpy(NomFichero, nombreArchivo);
	fclose(archivo);
	strcpy(NomFichero, nombreArchivo);

	// TODO llamar a función para ordenar el vector por CampoOrdenacion
	// si fallo al ordenar devuelvo 0 pero, libero memoria?
	// uso de qsort?
	return 1;
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
bool_t *
desconexion_1_svc(int *argp, struct svc_req *rqstp)
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
bool_t *
guardardatos_1_svc(int *argp, struct svc_req *rqstp)
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

	/*
	 * insert server code here
	 */

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

	/*
	 * insert server code here
	 */

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

	/*
	 * insert server code here
	 */

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
bool_t *
ordenar_1_svc(TOrdenacion *argp, struct svc_req *rqstp)
{
	static bool_t result;

	/*
	 * insert server code here
	 */

	return &result;
}

/**
 * @brief Consulta el número total de libros en la biblioteca.
 *
 * Devuelve siempre el número de libros actualmente almacenados en el
 * vector dinámico (NumLibros). No verifica el Ida pasado por parámetro.
 *
 * @param[in] argp   Puntero al Identificador de Administración (Ida).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el número de libros actual.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *nlibros_1_svc(int *argp, struct svc_req *rqstp)
{
	static int result;

	/*
	 * insert server code here
	 */

	return &result;
}

/**
 * @brief Busca la posición de un libro en el vector a partir de su ISBN.
 *
 * @param[in] argp   Puntero a estructura TConsulta con Ida e ISBN buscado.
 * @param rqstp      Estructura de petición de rpcgen (no usada).
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

	/*
	 * insert server code here
	 */

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
TLibro *
descargar_1_svc(TPosicion *argp, struct svc_req *rqstp)
{
	static TLibro result;

	/*
	 * insert server code here
	 */

	return &result;
}

/**
 * @brief Presta un libro a un usuario.
 *
 * Presta el libro de la posición indicada. Si hay ejemplares disponibles,
 * reduce el disponible y aumenta prestados. Si no, añade al usuario a
 * lista de espera. Reordena el vector al finalizar. No pide verificación del Ida.
 *
 * @param[in] argp   Puntero a estructura TPosicion con Pos (y un Ida irrelevante/anónimo).
 * @param rqstp      Estructura de petición de rpcgen (no usada).
 * @return           Puntero a entero con el resultado:
 *                   - -1: La posición indicada no está dentro de los límites del vector.
 *                   -  0: No hay libros, usuario puesto en lista de espera.
 *                   -  1: Libro prestado correctamente.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *prestar_1_svc(TPosicion *argp, struct svc_req *rqstp)
{
	static int result;

	/*
	 * insert server code here
	 */

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
 *                   -  2: Error, no se puede devolver porque ni está prestado ni hay espera.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *devolver_1_svc(TPosicion *argp, struct svc_req *rqstp)
{
	static int result;

	/*
	 * insert server code here
	 */

	return &result;
}
