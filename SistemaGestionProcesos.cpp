#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

struct NodoProceso {
    int id;
    string nombre;
    string prioridad;
    int memoria;
    string estado;
    NodoProceso* siguiente;
};
struct NodoCola {
    int id;
    string prioridad;
    NodoCola* siguiente;
};
struct NodoMemoria {
    int id;
    int tamano;
    NodoMemoria* siguiente;
};
const int CAPACIDAD_MEMORIA = 1024;

// ==================== VARIABLES GLOBALES ====================

NodoProceso* listaProcesos = NULL;
NodoCola* colaCPU = NULL;
NodoMemoria* pilaMemoria = NULL;

// Declaraciones necesarias para las operaciones entre estructuras.
NodoCola* buscarProcesoCola(NodoCola* actual, int id);
NodoMemoria* buscarBloque(NodoMemoria* actual, int id);
void ordenarCola(NodoCola*& frente);
void liberarEstructuras(NodoProceso*& procesos, NodoCola*& cola, NodoMemoria*& pila);

// ==================== VALIDACION Y ENTRADA DE DATOS ====================

int nivelPrioridad(string prioridad) {
    if (prioridad == "Alta") {
        return 1;
    }
    if (prioridad == "Media") {
        return 2;
    }
    return 3;
}

bool prioridadValida(string prioridad) {
    return prioridad == "Alta" || prioridad == "Media" || prioridad == "Baja";
}

bool estadoValido(string estadoActual) {
    return estadoActual == "Listo" || estadoActual == "En espera" ||
           estadoActual == "En ejecucion" || estadoActual == "Finalizado";
}

bool textoValido(string texto) {
    return texto.find_first_not_of(" \t\r") != string::npos;
}

bool convertirEntero(string texto, int& numero) {
    istringstream entrada(texto);
    char extra;
    return (entrada >> numero) && !(entrada >> extra);
}

int leerEntero(string mensaje) {
    string texto;
    int numero;
    while (true) {
        cout << mensaje;
        if (!getline(cin, texto)) {
            return -1;
        }
        if (convertirEntero(texto, numero)) {
            return numero;
        }
        cout << "Ingrese un numero entero valido.\n";
    }
}

// ==================== LISTA ENLAZADA: GESTOR DE PROCESOS ====================

NodoProceso* buscarProceso(NodoProceso* procesoActual, int id) {
    while (procesoActual && procesoActual->id != id) {
        procesoActual = procesoActual->siguiente;
    }
    return procesoActual;
}

bool validarProceso(NodoProceso* inicio, int id, string nombre, string prioridad, int memoria) {
    return id > 0 && textoValido(nombre) && prioridadValida(prioridad) && memoria > 0 &&
           !buscarProceso(inicio, id);
}

void agregarProceso(NodoProceso*& inicio, int id, string nombre, string prioridad, int memoria) {
    if (!validarProceso(inicio, id, nombre, prioridad, memoria)) {
        cout << "Datos invalidos o ID repetido.\n";
        return;
    }
    NodoProceso* nuevo = new NodoProceso;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->memoria = memoria;
    nuevo->estado = "Listo";
    nuevo->siguiente = NULL;
    if (!inicio) {
        inicio = nuevo;
    }
    else {
        NodoProceso* actual = inicio;
        while (actual->siguiente) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
    cout << "Proceso registrado.\n";
}

void imprimirProceso(NodoProceso* procesoActual) {
    cout << procesoActual->id << " | " << procesoActual->nombre << " | "
         << procesoActual->prioridad << " | " << procesoActual->memoria << " MB | "
         << procesoActual->estado << "\n";
}

void mostrarProcesos(NodoProceso* procesoActual) {
    if (!procesoActual) {
        cout << "Lista vacia.\n";
    }
    while (procesoActual) {
        imprimirProceso(procesoActual);
        procesoActual = procesoActual->siguiente;
    }
}

void consultarProceso(NodoProceso* procesoActual) {
    int opcion = leerEntero("Buscar: 1 ID, 2 nombre: ");
    bool encontrado = false;
    if (opcion == 1) {
        NodoProceso* nodo = buscarProceso(procesoActual, leerEntero("ID: "));
        if (nodo) {
            imprimirProceso(nodo);
            encontrado = true;
        }
    }
    else if (opcion == 2) {
        string nombre;
        cout << "Nombre exacto: ";
        getline(cin, nombre);
        while (procesoActual) {
            if (procesoActual->nombre == nombre) {
                imprimirProceso(procesoActual);
                encontrado = true;
            }
            procesoActual = procesoActual->siguiente;
        }
    }
    if (!encontrado) {
        cout << "No encontrado.\n";
    }
}

void eliminarProceso(NodoProceso*& inicio, NodoCola* frente, NodoMemoria* tope, int id) {
    if (buscarProcesoCola(frente, id) || buscarBloque(tope, id)) {
        cout << "Primero retire el proceso de la cola y libere su bloque.\n";
        return;
    }
    NodoProceso* actual = inicio;
    NodoProceso* anterior = NULL;
    while (actual && actual->id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (!actual) {
        cout << "No encontrado.\n";
        return;
    }
    if (anterior) {
        anterior->siguiente = actual->siguiente;
    }
    else {
        inicio = actual->siguiente;
    }
    delete actual;
    cout << "Proceso eliminado.\n";
}

void modificarPrioridad(NodoProceso* inicio, NodoCola*& frente, int id, string prioridad) {
    NodoProceso* procesoActual = buscarProceso(inicio, id);
    if (!procesoActual || !prioridadValida(prioridad)) {
        cout << "Proceso o prioridad invalida.\n";
        return;
    }
    procesoActual->prioridad = prioridad;
    NodoCola* colaActual = buscarProcesoCola(frente, id);
    if (colaActual) {
        colaActual->prioridad = prioridad;
        ordenarCola(frente);
    }
    cout << "Prioridad actualizada.\n";
}

void cambiarEstado(NodoProceso* inicio, NodoCola* frente, int id, string estado) {
    NodoProceso* procesoActual = buscarProceso(inicio, id);
    if (!procesoActual || !estadoValido(estado)) {
        cout << "Proceso o estado invalido.\n";
        return;
    }
    bool enCola = buscarProcesoCola(frente, id) != NULL;
    if ((enCola && estado != "En espera") || (!enCola && estado == "En espera")) {
        cout << "El estado En espera se administra mediante la cola.\n";
        return;
    }
    procesoActual->estado = estado;
    cout << "Estado actualizado.\n";
}

// ==================== COLA DE PRIORIDAD: PLANIFICADOR DE CPU ====================

NodoCola* buscarProcesoCola(NodoCola* actual, int id) {
    while (actual && actual->id != id) {
        actual = actual->siguiente;
    }
    return actual;
}

// Inserta despues de los nodos con igual prioridad para conservar su orden.
void insertarCola(NodoCola*& frente, NodoCola* nuevo) {
    NodoCola* actual = frente;
    NodoCola* anterior = NULL;
    while (actual && nivelPrioridad(actual->prioridad) <= nivelPrioridad(nuevo->prioridad)) {
        anterior = actual;
        actual = actual->siguiente;
    }
    nuevo->siguiente = actual;
    if (anterior) {
        anterior->siguiente = nuevo;
    }
    else {
        frente = nuevo;
    }
}

// Reutiliza los nodos existentes y reconstruye la cola ordenada.
void ordenarCola(NodoCola*& frente) {
    NodoCola* ordenada = NULL;
    while (frente) {
        NodoCola* nodo = frente;
        frente = frente->siguiente;
        insertarCola(ordenada, nodo);
    }
    frente = ordenada;
}

void encolarPorPrioridad(NodoProceso* inicio, NodoCola*& frente, int id) {
    NodoProceso* procesoActual = buscarProceso(inicio, id);
    if (!procesoActual) {
        cout << "Proceso no registrado.\n";
        return;
    }
    if (buscarProcesoCola(frente, id)) {
        cout << "Proceso ya encolado.\n";
        return;
    }
    NodoCola* nodo = new NodoCola;
    nodo->id = id;
    nodo->prioridad = procesoActual->prioridad;
    nodo->siguiente = NULL;
    insertarCola(frente, nodo);
    procesoActual->estado = "En espera";
    cout << "Proceso encolado.\n";
}

void desencolarProceso(NodoProceso* inicio, NodoCola*& frente) {
    if (!frente) {
        cout << "Cola vacia.\n";
        return;
    }
    NodoCola* nodo = frente;
    frente = frente->siguiente;
    NodoProceso* procesoActual = buscarProceso(inicio, nodo->id);
    if (procesoActual) {
        procesoActual->estado = "En ejecucion";
        cout << "Ejecutando proceso " << procesoActual->id << "\n";
        procesoActual->estado = "Finalizado";
    }
    delete nodo;
    cout << "Ejecucion simulada terminada. La memoria se libera por separado.\n";
}

void mostrarCola(NodoCola* actual) {
    if (!actual) {
        cout << "Cola vacia.\n";
    }
    while (actual) {
        cout << actual->id << " | " << actual->prioridad << "\n";
        actual = actual->siguiente;
    }
}

// ==================== PILA: GESTOR DE MEMORIA ====================

NodoMemoria* buscarBloque(NodoMemoria* actual, int id) {
    while (actual && actual->id != id) {
        actual = actual->siguiente;
    }
    return actual;
}

int memoriaOcupada(NodoMemoria* actual) {
    int total = 0;
    while (actual) {
        total += actual->tamano;
        actual = actual->siguiente;
    }
    return total;
}

// Valida el proceso y coloca su nuevo bloque en el tope (push).
void asignarMemoria(NodoProceso* inicio, NodoMemoria*& tope, int capacidad, int id) {
    NodoProceso* procesoActual = buscarProceso(inicio, id);
    if (!procesoActual) {
        cout << "Proceso no registrado.\n";
        return;
    }
    if (buscarBloque(tope, id)) {
        cout << "El proceso ya tiene memoria asignada.\n";
        return;
    }
    int ocupada = memoriaOcupada(tope);
    if (procesoActual->memoria <= 0 || procesoActual->memoria > capacidad - ocupada) {
        cout << "Tamano invalido o memoria insuficiente.\n";
        return;
    }
    NodoMemoria* nodo = new NodoMemoria;
    nodo->id = id;
    nodo->tamano = procesoActual->memoria;
    nodo->siguiente = tope;
    tope = nodo;
    cout << "Memoria asignada. Libre: " << capacidad - ocupada - nodo->tamano << " MB\n";
}

// Retira exclusivamente el bloque del tope (pop).
void liberarMemoria(NodoMemoria*& tope) {
    if (!tope) {
        cout << "Pila vacia.\n";
        return;
    }
    NodoMemoria* nodo = tope;
    tope = nodo->siguiente;
    cout << "Bloque liberado: " << nodo->id << " | " << nodo->tamano << " MB\n";
    delete nodo;
}

void mostrarMemoria(NodoMemoria* actual) {
    cout << "Ocupada: " << memoriaOcupada(actual) << " / " << CAPACIDAD_MEMORIA
         << " MB\n";
    while (actual) {
        cout << actual->id << " | " << actual->tamano << " MB\n";
        actual = actual->siguiente;
    }
}

// ==================== ARCHIVOS: GUARDAR Y RECUPERAR EL ESTADO ====================

// Guarda procesos, cola y pila; el orden de los bloques parte del tope.
bool guardarEstado(NodoProceso* procesoActual, NodoCola* colaActual, NodoMemoria* bloqueActual) {
    ofstream archivo("estado_procesos.txt");
    if (!archivo) {
        cout << "No se pudo guardar.\n";
        return false;
    }
    archivo << "SGP1\n";
    while (procesoActual) {
        archivo << "P\n"
                << procesoActual->id << "\n"
                << procesoActual->nombre << "\n"
                << procesoActual->prioridad << "\n"
                << procesoActual->memoria << "\n"
                << procesoActual->estado << "\n";
        procesoActual = procesoActual->siguiente;
    }
    while (colaActual) {
        archivo << "C\n" << colaActual->id << "\n";
        colaActual = colaActual->siguiente;
    }
    while (bloqueActual) {
        archivo << "M\n" << bloqueActual->id << "\n" << bloqueActual->tamano << "\n";
        bloqueActual = bloqueActual->siguiente;
    }
    archivo << "FIN\n";
    archivo.close();
    if (!archivo) {
        cout << "Error al escribir.\n";
        return false;
    }
    cout << "Estado completo guardado.\n";
    return true;
}

bool leerNumero(ifstream& archivo, int& numero) {
    string texto;
    return getline(archivo, texto) && convertirEntero(texto, numero);
}

// Carga en estructuras temporales. Solo reemplaza los datos si todo es valido.
bool cargarEstado(NodoProceso*& inicio, NodoCola*& frente, NodoMemoria*& tope) {
    ifstream archivo("estado_procesos.txt");
    if (!archivo) {
        cout << "No hay archivo de estado.\n";
        return false;
    }
    NodoProceso* procesoActual = NULL;
    NodoProceso* ultimoProceso = NULL;
    NodoCola* colaActual = NULL;
    NodoCola* ultimoCola = NULL;
    NodoMemoria* bloqueActual = NULL;
    NodoMemoria* ultimoBloque = NULL;
    string tipo, nombre, prioridad, estado;
    int id, memoria, fase = 0, ocupada = 0;
    bool valido = true, fin = false;
    if (!getline(archivo, tipo) || tipo != "SGP1") {
        valido = false;
    }
    while (valido && getline(archivo, tipo)) {
        if (tipo == "FIN") {
            fin = true;
            break;
        }
        if (tipo == "P" && fase == 0) {
            if (!leerNumero(archivo, id) || !getline(archivo, nombre) ||
                !getline(archivo, prioridad) || !leerNumero(archivo, memoria) ||
                !getline(archivo, estado)) {
                valido = false;
                break;
            }
            if (!validarProceso(procesoActual, id, nombre, prioridad, memoria) ||
                !estadoValido(estado)) {
                valido = false;
                break;
            }
            NodoProceso* nodo = new NodoProceso;
            nodo->id = id;
            nodo->nombre = nombre;
            nodo->prioridad = prioridad;
            nodo->memoria = memoria;
            nodo->estado = estado;
            nodo->siguiente = NULL;
            if (ultimoProceso) {
                ultimoProceso->siguiente = nodo;
            }
            else {
                procesoActual = nodo;
            }
            ultimoProceso = nodo;
        }
        else if (tipo == "C" && fase <= 1) {
            fase = 1;
            if (!leerNumero(archivo, id)) {
                valido = false;
                break;
            }
            NodoProceso* proceso = buscarProceso(procesoActual, id);
            if (!proceso || buscarProcesoCola(colaActual, id) ||
                proceso->estado != "En espera" ||
                (ultimoCola &&
                 nivelPrioridad(ultimoCola->prioridad) > nivelPrioridad(proceso->prioridad))) {
                valido = false;
                break;
            }
            NodoCola* nodo = new NodoCola;
            nodo->id = id;
            nodo->prioridad = proceso->prioridad;
            nodo->siguiente = NULL;
            if (ultimoCola) {
                ultimoCola->siguiente = nodo;
            }
            else {
                colaActual = nodo;
            }
            ultimoCola = nodo;
        }
        else if (tipo == "M") {
            fase = 2;
            if (!leerNumero(archivo, id) || !leerNumero(archivo, memoria)) {
                valido = false;
                break;
            }
            NodoProceso* proceso = buscarProceso(procesoActual, id);
            if (!proceso || buscarBloque(bloqueActual, id) || memoria <= 0 ||
                memoria != proceso->memoria || memoria > CAPACIDAD_MEMORIA - ocupada) {
                valido = false;
                break;
            }
            ocupada += memoria;
            NodoMemoria* nodo = new NodoMemoria;
            nodo->id = id;
            nodo->tamano = memoria;
            nodo->siguiente = NULL;
            // Anexar conserva el orden guardado desde el tope.
            if (ultimoBloque) {
                ultimoBloque->siguiente = nodo;
            }
            else {
                bloqueActual = nodo;
            }
            ultimoBloque = nodo;
        }
        else {
            valido = false;
        }
    }
    string extra;
    if (fin && getline(archivo, extra)) {
        valido = false;
    }
    for (NodoProceso* nodo = procesoActual; nodo; nodo = nodo->siguiente) {
        if (nodo->estado == "En espera" && !buscarProcesoCola(colaActual, nodo->id)) {
            valido = false;
        }
    }
    if (!valido || !fin) {
        liberarEstructuras(procesoActual, colaActual, bloqueActual);
        cout << "Archivo invalido. Se conserva el estado actual.\n";
        return false;
    }
    liberarEstructuras(inicio, frente, tope);
    inicio = procesoActual;
    frente = colaActual;
    tope = bloqueActual;
    cout << "Estado completo recuperado.\n";
    return true;
}

// ==================== LIBERACION DE MEMORIA DINAMICA ====================

// Libera todos los nodos con delete antes de finalizar o reemplazar el estado.
void liberarEstructuras(NodoProceso*& procesoActual, NodoCola*& colaActual,
                        NodoMemoria*& bloqueActual) {
    while (procesoActual) {
        NodoProceso* nodo = procesoActual;
        procesoActual = procesoActual->siguiente;
        delete nodo;
    }
    while (colaActual) {
        NodoCola* nodo = colaActual;
        colaActual = colaActual->siguiente;
        delete nodo;
    }
    while (bloqueActual) {
        NodoMemoria* nodo = bloqueActual;
        bloqueActual = bloqueActual->siguiente;
        delete nodo;
    }
}

// ==================== MENU PRINCIPAL ====================

void mostrarMenu() {
    cout << "\n============================================\n";
    cout << "       SISTEMA DE GESTION DE PROCESOS\n";
    cout << "============================================\n";
    cout << " 1. Registrar proceso\n";
    cout << " 2. Mostrar procesos\n";
    cout << " 3. Buscar proceso\n";
    cout << " 4. Eliminar proceso\n";
    cout << " 5. Modificar prioridad\n";
    cout << " 6. Cambiar estado\n";
    cout << " 7. Encolar proceso\n";
    cout << " 8. Ejecutar siguiente proceso\n";
    cout << " 9. Mostrar cola\n";
    cout << "10. Buscar en cola\n";
    cout << "11. Asignar memoria\n";
    cout << "12. Liberar memoria\n";
    cout << "13. Mostrar memoria\n";
    cout << "14. Buscar bloque\n";
    cout << "15. Guardar estado\n";
    cout << "16. Cargar estado (reemplaza los datos actuales)\n";
    cout << " 0. Guardar y salir\n";
}

// ==================== OPERACIONES DEL MENU ====================

void registrarProceso() {
    int id = leerEntero("ID del proceso: ");
    string nombre;
    string prioridad;

    cout << "Nombre: ";
    getline(cin, nombre);
    cout << "Prioridad (Alta/Media/Baja): ";
    getline(cin, prioridad);
    int memoria = leerEntero("Memoria requerida (MB): ");

    if (cin) {
        agregarProceso(listaProcesos, id, nombre, prioridad, memoria);
    }
}

void buscarProceso() {
    consultarProceso(listaProcesos);
}

void eliminarProceso() {
    int id = leerEntero("ID del proceso a eliminar: ");
    eliminarProceso(listaProcesos, colaCPU, pilaMemoria, id);
}

void modificarPrioridad() {
    int id = leerEntero("ID del proceso: ");
    string prioridad;
    cout << "Nueva prioridad (Alta/Media/Baja): ";
    getline(cin, prioridad);
    modificarPrioridad(listaProcesos, colaCPU, id, prioridad);
}

void cambiarEstado() {
    int id = leerEntero("ID del proceso: ");
    string estado;
    cout << "Estado (Listo/En espera/En ejecucion/Finalizado): ";
    getline(cin, estado);
    cambiarEstado(listaProcesos, colaCPU, id, estado);
}

void encolarProceso() {
    int id = leerEntero("ID del proceso a encolar: ");
    encolarPorPrioridad(listaProcesos, colaCPU, id);
}

void buscarEnCola() {
    int id = leerEntero("ID del proceso: ");
    NodoCola* actual = buscarProcesoCola(colaCPU, id);

    if (actual == NULL) {
        cout << "Proceso no encontrado en la cola.\n";
        return;
    }

    cout << "ID: " << actual->id << "\n";
    cout << "Prioridad: " << actual->prioridad << "\n";
}

void asignarMemoria() {
    int id = leerEntero("ID del proceso: ");
    asignarMemoria(listaProcesos, pilaMemoria, CAPACIDAD_MEMORIA, id);
}

void buscarMemoria() {
    int id = leerEntero("ID del proceso: ");
    NodoMemoria* actual = buscarBloque(pilaMemoria, id);

    if (actual == NULL) {
        cout << "El proceso no tiene memoria asignada.\n";
        return;
    }

    cout << "ID: " << actual->id << "\n";
    cout << "Memoria: " << actual->tamano << " MB\n";
}

// ==================== PROGRAMA PRINCIPAL ====================

int main() {
    int opcion;
    cargarEstado(listaProcesos, colaCPU, pilaMemoria);

    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione una opcion: ");

        if (!cin) {
            break;
        }

        cout << "\n";

        switch (opcion) {
            case 1:
                registrarProceso();
                break;
            case 2:
                mostrarProcesos(listaProcesos);
                break;
            case 3:
                buscarProceso();
                break;
            case 4:
                eliminarProceso();
                break;
            case 5:
                modificarPrioridad();
                break;
            case 6:
                cambiarEstado();
                break;
            case 7:
                encolarProceso();
                break;
            case 8:
                desencolarProceso(listaProcesos, colaCPU);
                break;
            case 9:
                mostrarCola(colaCPU);
                break;
            case 10:
                buscarEnCola();
                break;
            case 11:
                asignarMemoria();
                break;
            case 12:
                liberarMemoria(pilaMemoria);
                break;
            case 13:
                mostrarMemoria(pilaMemoria);
                break;
            case 14:
                buscarMemoria();
                break;
            case 15:
                guardarEstado(listaProcesos, colaCPU, pilaMemoria);
                break;
            case 16:
                cargarEstado(listaProcesos, colaCPU, pilaMemoria);
                break;
            case 0:
                if (!guardarEstado(listaProcesos, colaCPU, pilaMemoria)) {
                    opcion = -1;
                }
                break;
            default:
                cout << "Opcion invalida.\n";
        }
    } while (opcion != 0 && cin);

    liberarEstructuras(listaProcesos, colaCPU, pilaMemoria);
    return 0;
}
