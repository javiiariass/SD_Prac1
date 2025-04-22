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

        TDatosAlmacen DatosAlmacen(int pAlmacen) = 1;


        int NProductos(int pAlmacen) = 2;



        int CrearAlmacen(TDatosAlmacen pDatos) = 3;


        int AbrirAlmacen(Cadena pNomFiche) = 4;

        bool GuardarAlmacen(int pAlmacen) = 5;


        bool CerrarAlmacen(int pAlmacen) = 6;

        bool AlmacenAbierto(int pAlmacen) = 7;


        int BuscaProducto(TBusProd pBus) = 8;

        TProducto ObtenerProducto(TObtProd pPos) = 9;


        bool AnadirProducto(TActProd pProdNuevo) = 10;



        bool EliminarProducto(TBusProd pProduc) = 12;

        TProducto operacion(TOperacion) = 13;
    }
    = 1;
}
= 0x30000001;
