#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define CAN_ID_ENGINE_DATA 0x105
#define CAN_ID_BODY_STATUS 0x200

// Funcția DISPECER - procesează și rutează mesajele în funcție de ID
void process_can_message(uint32_t id, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
    
    switch (id)
      {
        
        // Cazul 1: Virtual Dyno (Date Motor)
        case CAN_ID_ENGINE_DATA:
	  {
            uint16_t raw_rpm = (b0 << 8) | b1;
            float rpm = raw_rpm * 0.25f;
            
            uint16_t raw_torque = (b2 << 8) | b3;
            float torque = raw_torque * 0.5f;
            
            float hp = (torque * rpm) / 7023.5f;
            
            // Am adaugat [0x%03X] pentru a afisa ID-ul in format hexazecimal
            printf("[0x%03X] [MOTOR]     | RPM: %4.0f | Cuplu: %5.1f Nm | Putere: %5.1f CP\n", 
                   id, rpm, torque, hp);
            break;
	  }
        
        // Cazul 2: Status Caroserie
        case CAN_ID_BODY_STATUS:
	  {
            uint8_t headlights_on = b0 & 0x01; 
            uint8_t door_open     = b0 & 0x02; 
            
            printf("[0x%03X] [CAROSERIE] | Faruri: %-7s | Usa sofer: %s\n", 
                   id, 
                   headlights_on ? "PORNITE" : "OPRITE",
                   door_open ? "DESCHISA" : "INCHISA");
            break;
	  }
        
        // Cazul 3: Error handling pentru mesaje irelevante
        default:
            printf("[0x%03X] [NECUNOSCUT]| Mesaj ignorat (Nu exista decodor pentru acest ID)\n", id);
            break;
    }
}

int main(int argc, char *argv[])
{
    // Validare argumente
    if (argc != 2)
      {
        printf("Usage: %s <nume_fisier_log.txt>\n", argv[0]);
        exit(EXIT_FAILURE);
      }

    // Deschidere fisier
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
      {
        printf("Eroare la deschiderea fisierului: %s\n", argv[1]);
        exit(EXIT_FAILURE);
      }

    printf("=== CAN Bus Analyzer & Virtual Dyno ===\n");
    printf("Procesare log din fisierul: %s\n", argv[1]);
    printf("------------------------------------------------------------------\n");

    uint32_t id, d0, d1, d2, d3;

    // Citire si procesare linie cu linie
    while (fscanf(file, "%x %x %x %x %x", &id, &d0, &d1, &d2, &d3) == 5)
      {
        process_can_message(id, (uint8_t)d0, (uint8_t)d1, (uint8_t)d2, (uint8_t)d3);
      }

    printf("------------------------------------------------------------------\n");
    
    fclose(file);
    return 0;
}
