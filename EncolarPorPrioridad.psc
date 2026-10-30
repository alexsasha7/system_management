Proceso EncolarPorPrioridad
    Definir cantidad, id, nivel, i, posicion, j Como Entero
    Definir ids, niveles Como Entero
    Definir prioridad Como Caracter
    Definir prioridades Como Caracter
    Definir duplicado, encontrado Como Logico
    Dimension ids[11]
    Dimension niveles[11]
    Dimension prioridades[11]
	
    cantidad <- 2
    ids[1] <- 101
    prioridades[1] <- "Media"
    niveles[1] <- 2
    ids[2] <- 102
    prioridades[2] <- "Baja"
    niveles[2] <- 3
	
    Escribir "Ingrese ID y prioridad (Alta, Media o Baja):"
    Leer id, prioridad
	
    Segun prioridad Hacer
        "Alta": nivel <- 1
        "Media": nivel <- 2
        "Baja": nivel <- 3
        De Otro Modo:
            nivel <- 0
    FinSegun
	
    duplicado <- Falso
    i <- 1
    Mientras i <= cantidad Hacer
        Si ids[i] = id Entonces
            duplicado <- Verdadero
        FinSi
        i <- i + 1
    FinMientras
	
    Si id > 0 Y nivel > 0 Y NO duplicado Y cantidad < 10 Entonces
        posicion <- 1
        encontrado <- Falso
        Mientras posicion <= cantidad Y NO encontrado Hacer
            Si nivel < niveles[posicion] Entonces
                encontrado <- Verdadero
            Sino
                posicion <- posicion + 1
            FinSi
        FinMientras
		
        j <- cantidad
        Mientras j >= posicion Hacer
            ids[j + 1] <- ids[j]
            niveles[j + 1] <- niveles[j]
            prioridades[j + 1] <- prioridades[j]
            j <- j - 1
        FinMientras
		
        ids[posicion] <- id
        niveles[posicion] <- nivel
        prioridades[posicion] <- prioridad
        cantidad <- cantidad + 1
        Escribir "Cola ordenada por prioridad:"
        i <- 1
        Mientras i <= cantidad Hacer
            Escribir ids[i], " - ", prioridades[i]
            i <- i + 1
        FinMientras
    Sino
        Escribir "ID repetido, prioridad invalida o cola llena"
    FinSi
FinProceso