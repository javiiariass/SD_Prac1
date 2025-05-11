**IMPORTANTE**
En el case 3, tal y como está, puede dar lugar a fallo. cerrarunalmacen permite cerrar cualquier almacen que esté abierto pasándole la id (si es válida).
He puedo para que pueda cerrar cualquiera que el usuario introduzca. 
Esto en "la vida real" no podríamos permitirlo porque un cliente podría cerrar el almacén que tiene abierto otro cliente.
Lo he dejado así para poder probar más facilmente y poder cerrar todos los almacenes desde un único cliente.
Si se quiere tener el funcionamiento "adecuado", en el main, en el case 3 del switch, descomenta la línea que se indica y comenta la línea anterior(también se indica).