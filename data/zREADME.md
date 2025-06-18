# Explicación de los archivos CSV

Este proyecto utiliza varios archivos CSV para definir enemigos, objetos, estados y habilidades. Aquí se explica cada uno y algunos detalles que pueden no ser evidentes a simple vista.

---

## Enemies.csv

- **nombre**: Nombre del enemigo.
- **{vida;ataque;defensa}**: Estadísticas principales, separadas por punto y coma.
- **loot**: Por ahora siempre es NULL (puedes agregar objetos que suelten aquí).
- **esJefe(bool)**: Si es True, es un jefe.
- **habilidades**: Lista de habilidades separadas por punto y coma.

---

## Items.csv

- **nombre**: Nombre del objeto.
- **esConsumible**: 
  - `tipoPocion`: Se consume al usarlo (pociones, comida, etc).
  - `noConsumible`: No se consume (armas, armaduras, accesorios).
  - `libroDeHabilidad`: Al usarlo, aprendes una habilidad.
- **esEquipable**: 
  - `arma`: Daño extra al equipar.
  - `armadura`: Defensa o vida extra al equipar.
  - `noEquipable`: No se puede equipar (pociones, libros, etc).
- **{vida;daño;defensa}**: Bonos que otorga al equipar (si aplica).
- **vidaRecuperada**: Vida que recupera al usarlo (si es poción o comida).
- **habilidadAprendida**: Si es un libro, aquí va la habilidad que enseña.
- **descripción**: Descripción del objeto.

**IMPORTANTE:**  
Si un objeto NO es un libro (`libroDeHabilidad`), pero tiene una habilidad en la columna `habilidadAprendida`, significa que al usarlo activa esa habilidad (por ejemplo, una daga venenosa activa "Envenenar" al atacar, aplicando el efecto al enemigo).

---

## Status.csv

- **ID**: Identificador único del estado.
- **nombre**: Nombre del estado.
- **tipo**: Puede afectar vida, daño, defensa o hacer saltar turnos.
- **coste**: Solo para estados que hacen saltar turnos (cuántos turnos se salta).
- **operacion**: 
  - `suma`: Suma o resta la cantidad indicada.
  - `mult`: Multiplica el valor base por la cantidad indicada.
- **cantidad**: Valor a sumar/restar o multiplicar.

---

## Skills.csv

- **nombre**: Nombre de la habilidad.
- **cooldown**: Turnos que deben pasar antes de volver a usarla.
- **duracion**: Cuántos turnos dura el efecto.
- **tipo**: 
  - `Curacion`: Cura vida.
  - `Estado`: Aplica un estado (de Status.csv).
- **vidaCurada**: Cuánta vida cura (si aplica).
- **estados(ID)**: ID del estado que aplica (de Status.csv).
- **hacia**: 
  - `Propio`: Se aplica al usuario.
  - `Enemigo`: Se aplica al enemigo.

---

## Notas generales

- Si tienes dudas sobre qué hace una habilidad, revisa Skills.csv y Status.csv.
- Los objetos pueden tener efectos especiales si tienen una habilidad asociada.
- Los estados pueden afectar estadísticas de varias formas (sumar, multiplicar, saltar turnos, etc).