Proceso CambiarEstado
    Definir id, cantidad, i, posicion Como Entero
    Definir nuevoEstado Como Caracter
    Definir ids Como Entero
    Definir estados Como Caracter
    Dimension ids[11]
    Dimension estados[11]
	
    cantidad <- 2
    ids[1] <- 101
    ids[2] <- 102
    estados[1] <- "Listo"
    estados[2] <- "En espera"
	
    Escribir "Ingrese ID y nuevo estado:"
    Leer id, nuevoEstado
	
    posicion <- 0
    i <- 1
    Mientras i <= cantidad Hacer
        Si ids[i] = id Entonces
            posicion <- i
        FinSi
        i <- i + 1
    FinMientras
	
    Si posicion <> 0 Y (nuevoEstado = "Listo" O nuevoEstado = "En espera" O nuevoEstado = "En ejecucion" O nuevoEstado = "Finalizado") Entonces
        estados[posicion] <- nuevoEstado
        Escribir "Nuevo estado del proceso ", id, ": ", estados[posicion]
    Sino
        Escribir "Proceso inexistente o estado invalido"
    FinSi
FinProceso