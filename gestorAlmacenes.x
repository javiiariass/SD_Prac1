typedef char Cadena[90];

struct TFecha
{
    int Dia;
    int Mes;
    int Anyo;
};

struct TProducto
{
    Cadena CodProd;
    int Cantidad;
    Cadena NombreProd;
    float Precio;
    Cadena Descripcion;
    struct TFecha Caducidad;
};

struct TDatosAlmacen
{
    Cadena Nombre;
    Cadena Direccion;
    Cadena Fichero;
};

struct TBusProd
{
    int Almacen;
    Cadena CodProducto;
};

struct TObtProd
{
    int Almacen;
    int PosProducto;
};

struct TActProd
{
    int Almacen;
    struct TProducto Producto;
};

struct TOperacion
{
    int p1;
    Cadena p2;
    float p3;
    float p4;
};

program SUPERMERCADO
{
    version SUPERMERCADO_VER
    {
        // Dado una posición del almacén, devuelve sus datos
        TDatosAlmacen DatosAlmacen(int pAlmacen) = 1;

        // Dado una posición de almacén, devuelve el número de productos o -1 si no existe.
        int NProductos(int pAlmacen) = 2;

        // Crea un almacén vacío de productos y devuelve la posición donde se encuentra. Si previamente ya
        // estaba creado y cargado en memoria, devolverá la posición donde se encuentra en memoria.
        int CrearAlmacen(TDatosAlmacen pDatos) = 3;
        // Abre un fichero de almacén y lo carga en memoria y devuelve su posición. Si previamente ya estaba
        // cargado devuelve la posición donde se encuentra.
        int AbrirAlmacen(Cadena pNomFiche) = 4;
        // Dado la posición de un almacén vuelca sus datos al fichero. Devuelve true si ha podido guardarlo.
        bool GuardarAlmacen(int pAlmacen) = 5;
        // Dado la posición de un almacén, vuelca sus datos al fichero y elimina la memoria dinámica asociada.
        // Si el Almacén es compartido, solo el último cliente es el que eliminará la memoria dinámica.
        bool CerrarAlmacen(int pAlmacen) = 6;
        // Dado la posición del almacén devuelve true si el almacén está cargado en memoria.
        bool AlmacenAbierto(int pAlmacen) = 7;
        // Dado la posición del almacén y un código de producto, devuelve la posición dentro del vector de
        // dinámico donde se encuentra el producto. Si no lo encuentra devuelve -1.
        int BuscaProducto(TBusProd pBus) = 8;
        // Dado la posición del almacén y la del producto, lo devuelve.
        TProducto ObtenerProducto(TObtProd pPos) = 9;
        // Dado la posición de un almacén y un producto lo añadirá a la memoria dinámica del almacén y devuelve
        // true. No puede existir dos productos con el mismo código en el almacén.
        bool AnadirProducto(TActProd pProdNuevo) = 10;
        // Dado la posición de un almacén y un producto lo actualizará en la memoria dinámica del almacén.
        Devolverá true si el producto existe y ha sido actualizado.bool ActualizarProducto(TActProd pProd) = 11;
        // Dado la posición del almacén y un código de producto, lo elimina de la memoria dinámica y devuelve
        // true.
        bool EliminarProducto(TBusProd pProduc) = 12;
        // No hay que implementarlo por ahora
        TProducto operacion(TOperacion) = 13;
    }
    = 1;
}
= 0x30000001;
