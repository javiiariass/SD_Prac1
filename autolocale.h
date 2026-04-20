#include <stdio.h>
#include <locale.h>

#ifdef _WIN32
    #include <windows.h>
#endif

// Esta función se ejecuta automáticamente al cargar el programa
// antes de que empiece el main()
// Instrucción que indica al compilador que esta función debe ejecutarse
// antes de llamar al main
__attribute__((constructor))
void configurar_entorno() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        setlocale(LC_ALL, ".UTF8");
    #else
        setlocale(LC_ALL, "");
    #endif
}