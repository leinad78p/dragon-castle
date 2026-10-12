# Dragon Castle — combat data

`P` = base power, `MP` = magic/action points consumed, `ACC` = chance to hit in percent. Power is not always final damage: damaging abilities also use the user's Attack stat and the dragon's defence. Defensive and healing skills use P as the shield/healing value. A listed stun chance is checked after a successful hit.

## Party base stats

| Character | Race | HP | MP | Attack | Defence |
|---|---|---:|---:|---:|---:|
| Edric | Human | 125 | 35 | 7 | 8 |
| Zaryndra | Dragon | 108 | 48 | 10 | 5 |
| Vaelis | Elf | 88 | 74 | 5 | 4 |
| Kendra | Dwarf | 140 | 38 | 9 | 9 |
| Dragon | Dragon | 350 | — | — | 8 |

## Edric — fighter and commander

| Skill | P | MP | ACC | Effect |
|---|---:|---:|---:|---|
| Corte Real | 24 | 0 | 96% | Single-target damage |
| Estocada Solar | 39 | 9 | 88% | High single-target damage |
| Guardia Firme | 30 | 5 | 100% | Gives Edric 30 shield |
| Grito de Valor | 5 | 10 | 100% | Gives living allies +5 Attack for the round |

## Zaryndra — dragon-blooded striker

| Skill | P | MP | ACC | Effect |
|---|---:|---:|---:|---|
| Garra Draconica | 28 | 0 | 96% | Single-target damage |
| Aliento Carmesi | 45 | 14 | 82% | Powerful damage, less accurate |
| Cola Sismica | 23 | 8 | 90% | Damage; 30% chance to stun the dragon |
| Escamas Ancestrales | 28 | 9 | 100% | Gives Zaryndra 28 shield |

## Vaelis — elven mage and healer

| Skill | P | MP | ACC | Effect |
|---|---:|---:|---:|---|
| Flecha Lunar | 22 | 0 | 99% | Accurate single-target damage |
| Lanza de Cristal | 38 | 12 | 91% | Magical damage |
| Raiz Vinculante | 18 | 17 | 85% | Damage; 45% chance to stun the dragon |
| Pulso Sanador | 40 | 15 | 100% | Heals the lowest-HP ally; revives a fallen ally with 22 HP |

## Kendra — dwarven defender

| Skill | P | MP | ACC | Effect |
|---|---:|---:|---:|---|
| Golpe de Runa | 30 | 0 | 94% | Single-target damage |
| Fisura de Granito | 44 | 12 | 82% | Heavy, less accurate damage |
| Martillo Pesado | 22 | 8 | 89% | Damage; 20% chance to stun the dragon |
| Forja Protectora | 14 | 10 | 100% | Gives each living ally 14 shield |

## Turn order

Edric → Zaryndra → Vaelis → Kendra → Dragon. A defeated hero's turn is skipped. The dragon loses its action when stunned, otherwise it attacks one hero; every third enemy action is an area attack with Black Flame.
