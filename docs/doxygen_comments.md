# Guía de comentarios Doxygen para proyectos Sun RPC en C (Linux)

Este documento describe el formato estándar de comentarios Doxygen que debe seguirse en todos los ficheros C de un proyecto Sun RPC. El objetivo es que la documentación pueda generarse correctamente con `doxygen`.

---

## Estructura típica de un proyecto Sun RPC

Un proyecto Sun RPC en Linux genera y organiza los siguientes tipos de ficheros:

| Fichero | Descripción |
|---|---|
| `servicio.x` | Definición del protocolo RPC (IDL). **No es C**, Doxygen no lo procesa. |
| `servicio.h` | Generado por `rpcgen`. Contiene tipos XDR y prototipos. **No modificar.** |
| `servicio_xdr.c` | Generado por `rpcgen`. Serialización XDR. **No modificar.** |
| `servicio_svc.c` | Generado por `rpcgen`. Esqueleto del servidor. **No modificar.** |
| `servicio_clnt.c` | Generado por `rpcgen`. Esqueleto del cliente. **No modificar.** |
| `server_impl.c` | **Escrito a mano.** Implementación de los procedimientos del servidor. |
| `client.c` | **Escrito a mano.** Lógica del cliente. |
| `common.h` / `common.c` | **Escrito a mano.** Utilidades compartidas. |

> **Regla general:** Solo se documentan con Doxygen los ficheros escritos a mano.
> Los ficheros generados por `rpcgen` no se tocan ni se documentan.

---

## Formato de comentario estándar

Se usa el estilo **Javadoc** con `/** ... */` y tags con `@`. Es el formato más compatible con IDEs y con la configuración por defecto de Doxygen.

- Bloque de documentación: `/** ... */`
- Comentario inline (tras el elemento): `/**< ... */`
- Tag de descripción corta: `@brief`
- Tags de parámetros: `@param`, `@param[in]`, `@param[out]`, `@param[in,out]`
- Tag de retorno: `@return`
- **No usar** la variante con `!` (`/*! */`) ni tags con barra invertida (`\brief`, `\param`).

---

## Cabecera de fichero

Todo fichero `.c` o `.h` escrito a mano debe empezar con este bloque:

```c
/**
 * @file   server_impl.c
 * @brief  Implementación de los procedimientos del servidor RPC.
 *
 * Contiene la lógica de negocio invocada por el dispatcher generado
 * por rpcgen. Cada función corresponde a un procedimiento declarado
 * en el fichero .x del protocolo.
 *
 * @author Nombre Apellido
 * @date   2026-04-20
 */
```

---

## Funciones de implementación del servidor

Los procedimientos RPC del servidor siguen la firma que impone `rpcgen`:
el nombre termina en `_N` (siendo N el número de versión) y tanto el
argumento como el retorno son punteros.

```c
/**
 * @brief Suma dos enteros en el servidor.
 *
 * Procedimiento RPC SUMAR de la versión 1. Recibe dos operandos
 * empaquetados en una estructura XDR y devuelve el resultado
 * al cliente mediante un puntero estático.
 *
 * @param[in] argp  Puntero a la estructura con los operandos.
 *                  Corresponde al tipo definido en servicio.x.
 * @return          Puntero al resultado (entero). NULL en caso de error.
 *
 * @warning El valor devuelto apunta a memoria estática. No liberar.
 */
int *sumar_1_svc(operandos *argp, struct svc_req *rqstp) {
    static int resultado;
    resultado = argp->a + argp->b;
    return &resultado;
}
```

> **Nota sobre `struct svc_req *rqstp`:** Este parámetro lo inyecta
> automáticamente `rpcgen` y normalmente no se usa. No es necesario
> documentarlo salvo que la implementación lo consulte (p.ej. para
> obtener la IP del cliente).

---

## Funciones del cliente

```c
/**
 * @brief Llama al procedimiento remoto SUMAR y muestra el resultado.
 *
 * Establece la conexión con el servidor, invoca el procedimiento
 * SUMAR_1 a través del handle RPC y gestiona los posibles errores
 * de comunicación.
 *
 * @param[in] host  Nombre o IP del servidor RPC.
 * @param[in] a     Primer operando.
 * @param[in] b     Segundo operando.
 * @return          El resultado de la suma, o -1 si hay error RPC.
 */
int llamar_sumar(const char *host, int a, int b);
```

---

## Estructuras y tipos propios

Si se definen estructuras auxiliares fuera del fichero `.x` (p.ej. para
estado interno del servidor), se documentan así:

```c
/**
 * @brief Contexto interno del servidor.
 *
 * Almacena el estado global necesario entre llamadas RPC.
 * Se inicializa en main() antes de registrar el servicio.
 */
typedef struct {
    int      num_llamadas;  /**< Contador total de llamadas recibidas. */
    int      activo;        /**< 1 si el servidor acepta peticiones, 0 si no. */
    char     log_path[256]; /**< Ruta al fichero de log. */
} ServerContext;
```

---

## Macros y constantes

```c
/** @brief Número de versión del protocolo RPC. Debe coincidir con el .x */
#define VERSION_RPC 1

/** @brief Timeout en segundos para las llamadas del cliente. */
#define TIMEOUT_SEG 10
```

---

## Variables globales

Si es imprescindible usar variables globales, se documentan antes de su declaración:

```c
/**
 * @brief Contexto global del servidor.
 *
 * Accedido por todos los procedimientos RPC. Inicializado en main().
 */
ServerContext ctx;
```

---

## Funciones auxiliares internas

Las funciones `static` (solo visibles en su unidad de compilación) también
se documentan, ya que Doxygen las incluye si se activa `EXTRACT_STATIC = YES`
en el `Doxyfile`.

```c
/**
 * @brief Registra una llamada RPC en el fichero de log.
 *
 * @param[in] proc_name  Nombre del procedimiento invocado.
 * @param[in] resultado  Valor devuelto al cliente.
 */
static void log_llamada(const char *proc_name, int resultado);
```

---

## Resumen de tags utilizados

| Tag | Cuándo usarlo |
|---|---|
| `@file` | Siempre, en la cabecera de cada fichero. |
| `@brief` | Siempre, en toda función, struct, macro y variable global. |
| `@param[in]` | Parámetro de solo entrada. |
| `@param[out]` | Parámetro de solo salida (se modifica via puntero). |
| `@param[in,out]` | Parámetro que se lee y se modifica. |
| `@return` | En toda función que no devuelva `void`. |
| `@note` | Aclaraciones importantes que no son advertencias. |
| `@warning` | Comportamientos peligrosos (memoria estática, etc.). |
| `@see` | Referencia a otra función o fichero relacionado. |
| `@date` / `@author` | Solo en la cabecera de fichero. |

---

## Configuración recomendada del Doxyfile

Para que Doxygen procese correctamente un proyecto Sun RPC, el `Doxyfile`
debe incluir al menos estas opciones:

```
PROJECT_NAME     = "Mi Servicio RPC"
INPUT            = .
FILE_PATTERNS    = *.c *.h
EXCLUDE_PATTERNS = *_xdr.c *_svc.c *_clnt.c servicio.h
EXTRACT_ALL      = NO
EXTRACT_STATIC   = YES
OPTIMIZE_OUTPUT_FOR_C = YES
GENERATE_HTML    = YES
GENERATE_LATEX   = NO
RECURSIVE        = YES
```

> `EXCLUDE_PATTERNS` excluye los ficheros generados por `rpcgen` para que
> no aparezcan en la documentación con comentarios vacíos.
> `OPTIMIZE_OUTPUT_FOR_C` adapta la terminología de Doxygen a C
> (usa "Functions" en lugar de "Member Functions", etc.).