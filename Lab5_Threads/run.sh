
# 1. Configuración de archivos
SMALL="access.log"
MEDIUM="access_medium.log"
LARGE="access_large.log"
SELECTED=""

echo "=========================================================="
echo "    LAB005: ANALIZADOR DE LOGS - SELECCIÓN DE CARGA"
echo "=========================================================="

# 2. Menú de selección de archivo
echo "Selecciona el tamaño del log para la prueba:"
echo "1) Pequeño (5,000 líneas)"
echo "2) Mediano (archivo medium)"
echo "3) Grande (archivo large)"
read -p "Opción [1-3]: " choice

case $choice in
    1) SELECTED=$SMALL ;;
    2) SELECTED=$MEDIUM ;;
    3) SELECTED=$LARGE ;;
    *) echo "Opción inválida. Usando log pequeño por defecto."; SELECTED=$SMALL ;;
esac

# 3. Verificar si el archivo existe
if [ ! -f "$SELECTED" ]; then
    echo "Error: No se encontró el archivo $SELECTED."
    exit 1
fi

# 4. Compilación
echo -e "\n[1/3] Compilando proyecto..."
make clean
make

# 5. Ejecución de pruebas de rendimiento

ln -sf "$SELECTED" access.log

echo -e "\n[2/3] Iniciando comparativa con: $SELECTED"
echo "----------------------------------------------------------"
echo "Prueba con 2 hilos:"
make run2
echo "----------------------------------------------------------"
echo "Prueba con 4 hilos:"
make run4
echo "----------------------------------------------------------"
echo "Prueba con 8 hilos:"
make run8
echo "----------------------------------------------------------"

# Limpiar enlace temporal
rm access.log

echo -e "\n[3/3] Pruebas completadas para $SELECTED."
echo "=========================================================="