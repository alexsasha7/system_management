Proceso AsignarMemoria
    Definir cantidad, capacidad, ocupada, id, tamano Como Entero
    Definir ids, tamanos Como Entero
    Dimension ids[11]
    Dimension tamanos[11]
	
    capacidad <- 512
    cantidad <- 1
    ids[1] <- 101
    tamanos[1] <- 128
    ocupada <- 128
	
    Escribir "Procesos de ejemplo: 101 y 102"
    Escribir "Memoria libre: ", capacidad - ocupada, " MB"
    Escribir "Ingrese ID del proceso y tamano en MB:"
    Leer id, tamano
	
    Si (id = 101 O id = 102) Y tamano > 0 Y ocupada + tamano <= capacidad Y cantidad < 10 Entonces
        cantidad <- cantidad + 1
        ids[cantidad] <- id
        tamanos[cantidad] <- tamano
        ocupada <- ocupada + tamano
        Escribir "Bloque asignado al proceso ", id
        Escribir "Memoria libre: ", capacidad - ocupada, " MB"
    Sino
        Escribir "Proceso invalido o memoria insuficiente"
    FinSi
FinProceso