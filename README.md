# 🏤 Simulácia pošty

Tento program simuluje fungovanie pošty so zákazníkmi a úradníkmi pomocou zdieľanej pamäte a semaforov.

## 📌 Spustenie programu
Program sa spúšťa s nasledujúcimi argumentmi:

| Parameter | Popis |
|-----------|--------------------------------------|
| `NZ`      | Počet zákazníkov |
| `NU`      | Počet úradníkov |
| `TZ`      | Maximálna doba príchodu zákazníkov (ms) |
| `TU`      | Maximálna doba obsluhy zákazníka (ms) |
| `F`       | Maximálny čas, po ktorom sa pošta zatvorí (ms) |

## 🛠 Implementácia
- **Zdieľaná pamäť** pomocou `mmap`
- **Synchronizácia procesov** pomocou `sem_t` semaforov
- **Zákazníci** čakajú v radoch na služby
- **Úradníci** vybavujú zákazníkov a môžu si vziať prestávku

## 🏁 Ukončenie simulácie
Po zatvorení pošty a obslúžení všetkých zákazníkov program korektne ukončí procesy a uvoľní pamäť.

