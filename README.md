Tento program simuluje fungovanie pošty s viacerými zákazníkmi a úradníkmi pomocou zdieľanej pamäte a semaforov.

Hlavné časti programu:

Spracovanie argumentov – Program prijíma 5 argumentov určujúcich počet zákazníkov (NZ), úradníkov (NU), a časy čakania/služieb (TZ, TU, F).
Inicializácia zdieľanej pamäte a semaforov – Na synchronizáciu procesov sa používajú mmap na zdieľanie premenných a sem_t semafory.
Spracovanie zákazníkov – Zákazníci prichádzajú na poštu a zaradia sa do jednej z troch čakacích radov.
Obsluha úradníkmi – Úradníci náhodne vyberajú zákazníkov z radov a spracovávajú ich požiadavky. Ak nie sú zákazníci, môžu si vziať prestávku.
Ukončenie simulácie – Po zatvorení pošty a vybavení všetkých zákazníkov úradníci odchádzajú domov.
