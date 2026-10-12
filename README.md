# Dragon Castle — GBA homebrew en C

Un pequeño RPG de combate por turnos para Game Boy Advance. Un chico entra en un castillo antiguo y se enfrenta a un dragón. Si vence, aparecen los créditos con el texto solicitado:

> juego hecho con chatGPT  
> beta/test 0

## Controles

- **START o A**: avanzar en la historia / confirmar.
- **Arriba y abajo**: elegir acción en combate.
- **A**: atacar, usar una poción o defenderse; también avanza los mensajes.
- **B**: muestra una pequeña pista durante el menú de combate.

## Compilar la ROM

Hace falta instalar el entorno **devkitPro con devkitARM y libgba**. Después, desde esta carpeta, ejecuta:

```sh
make
```

Se generará `dragoncastle.gba`, que puedes abrir en un emulador de Game Boy Advance o usar con hardware compatible.

Para borrar archivos generados:

```sh
make clean
```

## Contenido

- `src/main.c`: juego completo, gráficos pixel-art dibujados por código, fuente 5×7, entrada de botones y combate.
- `Makefile`: reglas de compilación para devkitARM.

Este paquete contiene el código fuente; no incluye una ROM `.gba` ya compilada porque el compilador cruzado de devkitARM no está disponible en el entorno donde se preparó.
