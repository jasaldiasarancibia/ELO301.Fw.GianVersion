# Cableado y puesta en marcha: una NUCLEO-L452RE

## Por qué se conectan esos pines

El microcontrolador contiene varios controladores I2C independientes. El profesor puso el emulador en I2C3, que recibe reloj por PC0 y datos por PC1. Nuestro lector utiliza I2C1, cuyo reloj sale por PB8 y sus datos por PB9. Para que se comuniquen se unen las señales con la misma función: reloj con reloj y datos con datos. No es una conexión TX/RX cruzada.

Los números 8, 9, 0 y 1 son nombres de GPIO dentro de los puertos B y C, no números de posición en el conector. No se conectan entre sí automáticamente por compartir el mismo chip. El firmware preparado necesita esos puentes externos.

Esquema eléctrico simplificado (no representa la posición física de los conectores):

```text
                 UNA MISMA PLACA

                      3,3 V
                        |
                      4,7 kΩ
                        |
PB8 / I2C1_SCL ---------+--------- PC0 / I2C3_SCL
  Lector: reloj                    Emulador: reloj

                      3,3 V
                        |
                      4,7 kΩ
                        |
PB9 / I2C1_SDA ---------+--------- PC1 / I2C3_SDA
  Lector: datos                    Emulador: datos

USB del computador → conector USB ST-LINK de la Nucleo
```

Son DOS resistencias, una por señal. No van en serie entre los dos pines: cada una une la señal a 3,3 V. I2C utiliza salidas de drenador abierto: las resistencias llevan las señales a nivel alto cuando los dispositivos las liberan. No usar 5 V. La tierra ya es común al utilizar una sola placa.

## Contactos físicos

Según UM1724, tabla 31, para NUCLEO-L452RE MB1136:

| Señal | Contacto Morpho |
|---|---|
| PB8 | CN10, pin 3 |
| PB9 | CN10, pin 5 |
| PC0 | CN7, pin 38, si los puentes seleccionan PC0 |
| PC1 | CN7, pin 36, si los puentes seleccionan PC1 |
| 3,3 V | CN7, pin 16 |

ST indica que CN7-38 y CN7-36 pueden conectarse alternativamente a PB8/PB9 según los puentes de soldadura. Verificar que realmente exponen PC0/PC1 antes de cablear; unir PB8 consigo mismo no conecta el emulador. Identificar CN7, CN10 y el pin 1 por la serigrafía y el plano, sin contar desde un extremo supuesto. Si hay dudas, enviar una foto nítida de ambas caras para comprobar la revisión y los puentes. No modificar puentes de soldadura sin verificar su estado.

Manual: https://www.st.com/resource/en/user_manual/um1724-.pdf (página 61 y tabla 10 sobre puentes).

Realizar las conexiones con el USB desconectado. Puede usarse una protoboard para las resistencias.

## Qué está incluido

- Proyecto STM32CubeIDE: fuentes C, cabeceras, HAL/CMSIS, arranque y scripts de enlace.
- Archivos `.project`, `.cproject` y `.ioc`.
- Emulador del curso: biblioteca compilada, cabecera y manejadores de interrupciones.
- Ejecutable ELF compilado en Debug y registro de compilación.
- Guías y terminal opcional para Windows (`VER_TEMPERATURA.ps1`).

No necesitas crear un proyecto nuevo ni descargar otra vez el GitHub. Los drivers ST-LINK y STM32CubeIDE se instalan en Windows por separado; no están redistribuidos en el ZIP.

## Abrir y compilar sin placa

1. Extraer TODO el ZIP; no abrir los archivos directamente dentro del comprimido. Preferir una ruta corta, por ejemplo C:\STM32\EntregaTemperatura.
2. En CubeIDE: File → Import → General → Existing Projects into Workspace.
3. Select root directory: seleccionar la carpeta `L452RE_ENTREGA_TEMPERATURA/Projects` del ZIP extraído.
4. Seleccionar el proyecto detectado y desmarcar Copy projects into workspace. Middlewares debe permanecer junto a Projects, porque el proyecto la referencia con una ruta relativa.
5. Finish. Project → Build Project. No es necesario regenerar los fuentes desde el `.ioc` para compilar esta entrega.

## Programar y ver temperaturas con placa

1. Cablear como se indica arriba y conectar USB ST-LINK con cable de datos.
2. En CubeIDE seleccionar el proyecto y usar Run → Debug As → STM32 C/C++ Application. Seleccionar ST-LINK si pregunta. Esto programa la memoria de la placa; verificar que es la placa destinada a esta entrega.
3. Si se detiene en main, pulsar Resume (F8) para ejecutar. Mientras está detenido no se realizan mediciones.
4. Localizar el COM de ST-LINK en Administrador de dispositivos → Puertos (COM y LPT). Si no aparece, comprobar cable de datos y driver ST-LINK.
5. Abrir ese COM en un terminal: 115200 baudios, 8 bits, sin paridad, 1 stop, sin control de flujo. Como alternativa, abrir PowerShell en esta carpeta y ejecutar `powershell -NoProfile -ExecutionPolicy Bypass -File .\VER_TEMPERATURA.ps1`. La opción se aplica solo a ese proceso y no cambia la política permanente del equipo.
6. Escribir el nombre COM cuando se solicite. No mantener otro programa usando el mismo puerto. Ctrl+C cierra el terminal.

Salida de ejemplo: `Temperatura: 25.12 C`. Sin puentes o pull-ups puede aparecer un error I2C. Sin placa, puedes compilar pero no ejecutar la medición real. El emulador corre dentro del STM32 físico, no dentro de CubeIDE.

## Estado comprobado

Compilado en STM32CubeIDE 2.3.0, Debug, cero errores y una advertencia LOAD RWX del enlazador. Falta la prueba física: programación de la Nucleo, comunicación I2C y recepción real por COM. La versión de una placa es la implementación elegida; el aviso del profesor no especifica expresamente la cantidad de placas.
