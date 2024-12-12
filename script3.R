# Capturar el nombre del dataset desde los argumentos de la línea de comandos
args <- commandArgs(trailingOnly = TRUE)
if (length(args) < 2) {
   stop("Por favor, proporciona el nombre del dataset (sin extensión) como parámetro.")
}

dataset_name <- args[1]
seed <- as.numeric(args[2])
file_name <- paste0(dataset_name, ".csv")

# Cargar la librería
library(tree)

# Leer el dataset
data <- read.table(file_name, header = TRUE, sep = ',', quote = "\"")

# Preparar los datos
nombres_columnas <- names(data)[-1]  # Excluir la primera columna
cadena_columnas <- paste(nombres_columnas, collapse = " + ")

# Convertir la columna 'cat_0' a factor
data$cat_0 <- factor(data$cat_0)

set.seed(seed)

# Definir el % de datos para training
size <- 0.7
train_size <- floor(size * nrow(data))

# Calcular la cantidad de datos para entrenar
train_sample <- sample(seq_len(nrow(data)), size = train_size)

# Crear los conjuntos de training y testing
train <- data[train_sample, ]
test <- data[-train_sample, ]

# Crear la fórmula y ajustar el modelo
formula <- as.formula(paste("cat_0 ~", cadena_columnas))
arbol <- tree(formula, data = train)

# Predecir y calcular la matriz de confusión y la precisión
pred <- predict(arbol, test, type = "class")
conf_matrix <- with(test, table(pred, test$cat_0))
acc <- sum(diag(conf_matrix)) / sum(conf_matrix)

# Contar el número de variables utilizadas en el árbol y extraer sus números
variables_usadas <- unique(arbol$frame$var[arbol$frame$var != "<leaf>"])
num_variables <- length(variables_usadas)

# Extraer los números de las variables utilizadas
numeros_variables <- as.numeric(sub("cat_", "", variables_usadas))

# Mostrar precisión, número de variables y lista de números de variables
cat(paste(acc, num_variables, paste(numeros_variables, collapse = " ")))