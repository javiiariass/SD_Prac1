# Guía de Conventional Commits

La convención dicta que los mensajes de commit deben seguir la siguiente estructura base:

```text
<tipo>[ámbito opcional]: <descripción corta>

[cuerpo opcional]

[pie opcional]
```

## 1. El Ámbito (Lo que va entre paréntesis)

Cuando ves un commit como `feat(user): añadir foto de perfil`, la palabra **`user`** representa el **ámbito** o **scope**.

* **¿Para qué sirve?** Sirve para contextualizar e indicar exactamente **qué parte del código, módulo o componente** se ha visto afectado por el cambio. 
* **¿Es obligatorio?** No, es completamente opcional, pero se recomienda encarecidamente en proyectos medianos y grandes.
* **Ejemplos comunes:** `(auth)`, `(ui)`, `(api)`, `(db)`, `(router)`, `(shopping-cart)`.
* **Ejemplo práctico:** Si ves `fix(auth): corregir caída al iniciar sesión`, inmediatamente sabes que el bug ocurrió en el sistema de autenticación, sin ni siquiera mirar los archivos modificados.

---

## 2. Tipos de Commits y Cuándo Aplicarlos

A continuación, la lista completa de los tipos permitidos y el escenario exacto en el que debes utilizarlos.

| Tipo | Significado | ¿Cuándo aplicar este tipo? |
|---|---|---|
| **`feat`** | Feature | Cuando añades una **nueva funcionalidad** al producto o aplicación. Es algo que el usuario final o el cliente de tu API va a poder notar o utilizar. |
| **`fix`** | Bug Fix | Cuando **solucionas un error** o un comportamiento inesperado en el código de producción. |
| **`refactor`**| Refactorización | Cuando reescribes o reestructuras código existente **sin cambiar su comportamiento externo**. No añade funcionalidades ni arregla bugs, pero mejora la legibilidad, mantenibilidad o arquitectura del código. |
| **`perf`** | Performance | Cuando realizas un cambio en el código específicamente diseñado para **mejorar el rendimiento** (hacerlo más rápido, consumir menos memoria, optimizar una consulta, etc.). |
| **`style`** | Estilo | Cuando los cambios **solo afectan el formato** del código y no su lógica. Ejemplos: ajustar indentación, cambiar comillas, quitar espacios en blanco, añadir puntos y comas, o pasar un linter (como Prettier). |
| **`test`** | Pruebas | Cuando **añades pruebas nuevas o corriges pruebas existentes** (unitarias, e2e, integración). No hay modificaciones en el código que va a producción. |
| **`docs`** | Documentación | Cuando tu cambio **solo afecta a archivos de texto o documentación**. Ejemplos: actualizar el `README.md`, modificar comentarios JSDoc, o ajustar la especificación de Swagger. |
| **`chore`** | Mantenimiento | Cuando realizas tareas rutinarias que **no afectan el código de producción ni los tests**. Ejemplos: actualizar dependencias en el `package.json`, modificar el `.gitignore`, o configurar herramientas del entorno de desarrollo. |
| **`build`** | Construcción | Cuando modificas archivos que afectan el **sistema de empaquetado o compilación** del proyecto. Ejemplos: cambios en la configuración de Webpack, Vite, Maven, Gradle o Dockerfiles base. |
| **`ci`** | Integración Continua| Cuando haces cambios en la **configuración de los pipelines** de automatización y despliegue. Ejemplos: modificar los archivos de GitHub Actions, GitLab CI, Travis o Jenkins. |
| **`revert`** | Reversión | Cuando el único propósito de tu commit es **deshacer los cambios de un commit anterior**. Suele incluir el hash del commit que se está revirtiendo en la descripción. |

---

## 💡 Regla especial: "Breaking Changes" (Cambios Rupturistas)

Si cualquiera de los commits anteriores introduce un cambio que **rompe la compatibilidad** con versiones anteriores (por ejemplo, cambiar el nombre de un endpoint en tu API que otros equipos ya están usando), debes añadir un signo de exclamación `!` antes de los dos puntos:

```bash
feat(api)!: cambiar la estructura de la respuesta de usuarios
```