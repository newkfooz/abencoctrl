## **Smart Controller for Energy Consumption**

Sistema per controllare i consumi di corrente e potenza istantanea (*kWatt*) su un carico, in modo da poter risparmiare sulla bolletta di casa, o anche solo per avere una visione completa della potenza assorbita dagli elettrodomestici.
In campo industriale può essere applicato per monitorare i consumi dei macchinari, inoltre, con uno studio più approfondito è possibile osservare la risposta temporale della corrente per un dato macchinario, determinando curve e picchi di potenza caratteristici.


---

Il sistema si basa su:

1. una **pinza amperometrica** *YHDC_SCT013000* che monitora costantemente il carico di potenza;
2. un **NodeMCU** per la connessione a internet (*WiFi*);
3. un server **Node-Red** per la visualizzazione dei dati e il controllo a distanza del carico;
4. un **relè** che permette di disattivare il carico;
5. un **sito web** nella rete locale per modificare lo stato del relè;
6. un **broker MQTT**.

---
