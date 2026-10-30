Proceso LiberarMemoria
    Definir cantidad, ocupada, idLiberado, tamanoLiberado Como Entero
    Definir ids, tamanos Como Entero
    Dimension ids[11]
    Dimension tamanos[11]
	
    cantidad <- 2
    ids[1] <- 101
    tamanos[1] <- 64
    ids[2] <- 102
    tamanos[2] <- 128
    ocupada <- 192
	
    Si cantidad > 0 Entonces
        idLiberado <- ids[cantidad]
        tamanoLiberado <- tamanos[cantidad]
        ocupada <- ocupada - tamanoLiberado
        cantidad <- cantidad - 1
        Escribir "Bloque liberado del proceso ", idLiberado
        Escribir "Memoria ocupada: ", ocupada, " MB"
    Sino
        Escribir "La pila esta vacia"
    FinSi
FinProceso
