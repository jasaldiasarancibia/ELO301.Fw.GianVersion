# Entrega: lectura del emulador y temperatura por COM

Esta variante implementa los dos puntos del aviso: leer el emulador de temperatura por I2C y mostrar el resultado por COM a 115200 8N1. No requiere interfaz HTML ni una segunda placa. El montaje de una sola NUCLEO-L452RE es una elección de implementación; el aviso no fija explícitamente la cantidad de placas.

## Contenido

`L452RE_ENTREGA_TEMPERATURA/Projects` contiene el proyecto CubeIDE. La carpeta vecina `Middlewares` contiene la biblioteca del curso; conservar ambas carpetas juntas. El emulador original se mantiene activo en I2C3 y el lector usa I2C1. No se lee directamente el ADC para resolver la tarea: la adquisición del alumno pasa por los registros I2C del emulador.

Cambios principales: `Core/Src/main.c` inicializa emulador y lector; `Core/Src/temperature_app.c` verifica identidad, habilita las mediciones, lee y convierte la temperatura y transmite texto. Elimina del bucle las demostraciones de botón/potenciómetro de la plantilla.

## Cableado en una misma placa, con alimentación desconectada

| Señal | Puente externo |
|---|---|
| SCL | PB8 (I2C1) a PC0 (I2C3) |
| SDA | PB9 (I2C1) a PC1 (I2C3) |

Añadir una resistencia de 4,7 kΩ entre SCL y 3,3 V y otra entre SDA y 3,3 V, si el bus no las tiene. Usar los contactos Morpho identificados en el plano de la Nucleo; no asumir A4/A5 sin revisar los puentes de soldadura. Al ser una misma placa, la tierra ya es común. No usar 5 V en el bus. Los puentes son indispensables: los periféricos no se conectan internamente por software.

## Abrir y ejecutar

1. Descomprimir la carpeta completa si se recibió el ZIP.
2. CubeIDE: File → Import → General → Existing Projects into Workspace.
3. Seleccionar `L452RE_ENTREGA_TEMPERATURA/Projects`. Desmarcar Copy projects into workspace para conservar la relación con Middlewares.
4. Compilar con Project → Build Project. No es necesario regenerar los fuentes con CubeMX.
5. Conectar una NUCLEO-L452RE por USB ST-LINK y programar este proyecto. Ejecutarlo; si se inicia una sesión Debug y queda detenido en main, pulsar Resume.
6. En un terminal serie abrir el COM de ST-LINK: 115200 baudios, 8 bits, sin paridad, 1 bit de parada y sin control de flujo.

Salida esperada, aproximadamente cada 500 ms:

```text
Temperatura: 25.00 C
Temperatura: 25.12 C
```

Los números anteriores son ejemplos, no mediciones realizadas. Se mide la temperatura interna del microcontrolador y puede diferir de ambiente. Dos decimales describen el formato, no la precisión garantizada.

## Protocolo y comprobación

Dirección 7 bits 0x48 (HAL: 0x48 << 1). Identidad: registro 0x02, respuesta 0x26. Habilitar lectura escribiendo 1 en registro 0x00; el emulador activo en el bus no significa que sus conversiones estén habilitadas. Leer dos bytes del registro 0x01, MSB primero; interpretar como entero con signo en centésimas. 0x8000 significa no disponible. Las interrupciones I2C3 del emulador se mantienen activas mientras el maestro I2C1 usa HAL bloqueante.

Sin cableado o pull-ups, es esperable un mensaje de error I2C. No mostrar 0 °C como sustituto de un fallo. Comprobar lecturas en el terminal y funcionamiento después de reset antes de entregar. El HTML anterior espera tramas TEMP y no es compatible con este formato de texto; no se requiere para esta entrega.

Fuente: plantilla ELO301 proporcionada por el usuario (https://github.com/linitrofe/ELO301.Fw.Template) y aviso del profesor. La biblioteca compilada del emulador y sus avisos de licencia se conservan.

## Resultado de compilación
18/11/2026: compilado en Debug con STM32CubeIDE 2.3.0, cero errores y una advertencia del enlazador por segmento LOAD RWX heredado del script de la plantilla. Ejecutable generado en Projects/Debug/L452RE_ENTREGA_TEMPERATURA.elf. Registro en compilacion.log. No se ha programado ni probado una placa física.

