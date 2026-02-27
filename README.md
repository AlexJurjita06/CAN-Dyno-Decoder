# 🏎️ CAN Bus Telemetry Decoder & Virtual Dyno

Acest proiect este un decodor în linie de comandă (C) creat pentru a procesa loguri de rețea CAN (Controller Area Network) auto. Programul simulează funcționalitatea unui sistem de telemetrie, extrăgând date brute (raw data) de pe magistrală și transformându-le în valori fizice utilizabile (RPM, Nm, CP, status uși/faruri).



## 🔧 Utilitatea în domeniul Automotive
În industria auto (Testare, Diagnoză, Dezvoltare ECU), inginerii lucrează constant cu date brute înregistrate de pe mașini (datalogging). Acest tool demonstrează conceptele de bază folosite în software-urile de analiză (precum Vector CANalyzer sau software-uri de chiptuning/dyno):

1. **Routing-ul Mesajelor:** Identificarea pachetelor pe baza CAN ID-ului (ex: `0x105` pentru motor, `0x200` pentru caroserie).
2. **Signal Extraction & Scaling:** Recompunerea semnalelor pe mai mulți bytes (bit shifting) și aplicarea factorilor de scalare pentru a obține unități de măsură reale (ex: `factor 0.25` pentru RPM).
3. **Optimizarea Lățimii de Bandă (Bitmasking):** Extragerea statusului mai multor componente dintr-un singur Byte, metodă folosită de BCM (Body Control Module) pentru a nu aglomera rețeaua.
4. **Calcul de Performanță:** Aplicarea formulelor fizice în timp real pentru a deriva date noi (calculul puterii pe baza cuplului și turației).

## 🧠 Cum am gândit arhitectura codului

Programul funcționează ca un **Dispecer (Parser)** care citește un fișier log linie cu linie și aplică o logică de decodare specifică în funcție de ID-ul mesajului:

* **Modulul Motor (ID `0x105`):** * Are prioritate mare pe bus (ID mic). 
  * Primește turația (RPM) pe primii 2 bytes și cuplul (Nm) pe următorii 2 bytes. 
  * **Logica de calcul:** Deoarece senzorii trimit date brute, codul concatenează bytes-ii (`(b0 << 8) | b1`), aplică scalarea din documentația modulului (`* 0.25f`) și calculează Puterea (Cai Putere Metrici) folosind formula standard a industriei: `(Cuplu * RPM) / 7023.5`.
* **Modulul Caroserie (ID `0x200`):** * Conține date despre elemente non-critice. 
  * Folosește **măști de biți (Bitwise AND)** pe un singur Byte pentru a verifica simultan statusul farurilor (`bitul 0`) și al ușii șoferului (`bitul 1`).
* **Sistemul de Tracking:** * Folosește pointeri transmiși prin referință (`&max_hp`, `&max_rpm`) pentru a menține starea performanțelor de vârf (Peak Power) pe toată durata analizei logului, generând un raport tip "Virtual Dyno" la finalul execuției.

## 🚀 Cum se rulează

1. Compilează codul sursă:
   `gcc can_decoder.c -o can_decoder`
2. Rulează programul oferindu-i un fișier de log cu mesaje CAN hexazecimale:
   `./can_decoder log.txt`
