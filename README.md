# WindowStatusSensor <br />

Der WindowStatusSensor ermöglicht den Anschluss und Überwachung von 8 Reedkontakten. Damit lassen sich schlussendlich 4 Fenster komplett überwachen. Dazu wird an jedem Fenster ein Reedkontakt oben und einer unten verbaut. 
Die Logik ist folgende:

|Reed oben|Reed unten| Status Fenster|
|:--:|:--:|:--:|
|zu|zu|geschlossen|
|offen|zu|angeklappt|
|offen|offen|offen|

Dazu werden immer 2 Reedkontakte als eine Gruppe definiert. Man kann natürlich auch nur einen Reedkontakt pro Gruppe benutzen, hat dann dementsprechend auch nur einen offen/zu Status.

Anstatt Fenster lassen sich natürlich auch Türen o.ä. überwachen. Ist in der Tür neben dem Reedkontakt an der der Türoberseite (Tür ist **offen/zu**) zusätzlich ein Schließerkontakt am Schloss eingebaut, kann dieser Reedkontakt ebenfalls in derselben Gruppe genutzt werden. Damit hätte die Tür den Status **Offen/Zu/Abgeschlossen**

Den WindowStatusSensor gibt hier im Git es als 2 Ausführungen:
* auf Basis des Panstamp NRG2 (der PanStamp wird leider nicht mehr hergestellt)
* auf Basis eines Arduino Mini 8Mhz mit CC1101 und AsksinPP Firmware als HM-SEC-RHS Homematic Gerät

Der Aufbau und Parameterisierung beider Systeme sind im jeweiligen Readme zu finden.

Für beide Aufführungen sind Platinen im Git mit entsprechenden Gerberfiles und Target3001 Datei für eine Weiterentwicklung vorhanden. Beide Platinen sind auf eine 5-12V DC Stromversorgung ausgelegt. Natürlich können auch eigene Platinen mit Batterieversorgung designed werden. Ich selbst habe in jeder Fensterleibung in der Bauphase ein 2x2x0.8 KNX Kabel verlegt auf dem 5VDC anliegen. Damit hat man auch das Batteriewechselproblem gelöst.

Leider werden keine Panstamps NRG2 oder AVR2 mehr hergestellt, diese sind weitaus besser als eine Arduino Mini/CC1101 oder ESP8266 Version. Daher der Schwenk auf den Arduino Mini.

## Vergleich Panstamp NRG vs. Arduino Version

[[Images/both_top_1.jpeg|200px|thumb|right|Schaltplan]]

[[Images/both_top_1.jpeg]]
