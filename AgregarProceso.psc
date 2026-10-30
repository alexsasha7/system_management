Proceso AgregarProceso
    Definir id, cantidad, i, repetido Como Entero
    Definir nombre, prioridad Como Caracter
    Definir ids Como Entero
    Definir nombres, prioridades, estados Como Caracter
    Dimension ids[11]
    Dimension nombres[11]
    Dimension prioridades[11]
    Dimension estados[11]
	
    cantidad <- 2
    ids[1] <- 101
    ids[2] <- 102
	
    Escribir "Ingrese ID, nombre y prioridad (Alta, Media o Baja):"
    Leer id, nombre, prioridad
	
    repetido <- 0
    i <- 1
    Mientras i <= cantidad Hacer
        Si ids[i] = id Entonces
            repetido <- 1
        FinSi
        i <- i + 1
    FinMientras
	
    Si id > 0 Y nombre <> "" Y (prioridad = "Alta" O prioridad = "Media" O prioridad = "Baja") Y repetido = 0 Y cantidad < 10 Entonces
        cantidad <- cantidad + 1
        ids[cantidad] <- id
        nombres[cantidad] <- nombre
        prioridades[cantidad] <- prioridad
        estados[cantidad] <- "Listo"
        Escribir "Proceso registrado: ", ids[cantidad], " - ", nombres[cantidad]
    Sino
        Escribir "Datos invalidos, ID repetido o lista llena"
    FinSi
FinProceso