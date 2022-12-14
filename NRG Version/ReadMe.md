panStamps sind Arduino Clones, die ein CC1101 Funkmodul beinhalten. Mit ihnen lassen sich Sensoren und Aktoren drahtlos an FHEM anbinden. Sie lassen sich genau wie Arduinos über die Arduino IDE oder mit dem ino Kommandozeilen Binary programmieren. Dieser bildet das Herzstück des Sensors.
Dieser Wiki Artikel bezieht auf den panstamp NRG 2 Weiterführende Information zu panStamps und den grundlegenden Modulen um sie mit FHEM zu integrieren finden sich im FHEM Wiki.

## Beschreibung

Dieser Sensor ist mit seiner Firmware spezialisert auf die Zustandserfassung von Fenster- und Türkontakten. Es können Gruppen gebildet werden mit Hilfe dessen 3 Zustände erfasst und an FHEM übermittelt werden können:

* Offen (open)
* Angeklappt (tilted)
* Geschlossen (closed)

Der auf dem panStamp NRG2 basierende Fensterkontaktsensor hat die folgenden Eigenschaften:

### Hardware:

8x ReedkontaktEingänge
3 LowCurrent LEDs zur Zustandsanzeige (Rot/Gelb/Grün)
Eingangsspannung von 4-36V, empfohlen wird eine Eingangsspannung von 5V aufgrund der Verlustleistung des Linearreglers
passt alles in eine 68mm Unterputzdose

### Software:

Konfiguration des Sendeintervalls
Konfiguration der Gruppen
Konfiguration der Blink- und Leuchtintervalls der LEDs
Übermittlung von allen 8 Eingängen
Übermittlung des Gruppenstatus

## LED Verhalten

Die LEDs zeigen folgende Zustände an, es kann immer nur eine LED gleichzeitig leuchten:

* Rot = mindestens eine definierte Gruppe haben den Status **Offen**
* Gelb = mindestens eine definierte Gruppe hat den Status **Angeklappt**, keine Gruppe ist Offen
* Green = alle definierten Gruppen haben den Status **Geschlossen**

Im Detail: Die Priorität ist die folgende: Offen -> Angeklappt -> Geschlossen Rot -> Gelb -> Grün


Über die Konfiguration kann folgendes eingestellt werden:

* Anzahl in Millisekunden, wie lange eine LED nach einem Event dauerhaft leuchten soll
* Anzahl in Millisekunden, wie lange nach 1) die LED ausgeschaltet ist
* Anzahl in Millisekunden, wie lange nach 1) und 2) die LED eingeschaltet ist


zb: 30000 , 15000, 50
Nach einem Event (Fenster wurde betätigt) leuchtet die passende LED 30sekunden dauerhaft, danach mit einem 50ms Blinken alle 15sek

## Schaltplan und Bauteilliste
[[Datei:Panstamp_Fensterkontaktsensor_v1.0_Schaltplan.jpg|200px|thumb|right|Schaltplan]]
[[Datei:Panstamp_Fensterkontaktsensor_v1.0_Platine_Oben.jpg|200px|thumb|right|Platinenlayout Oberseite]]
[[Datei:Panstamp_Fensterkontaktsensor_v1.0_Platine_Unten.jpg|200px|thumb|right|Platinenlayout Unterseite]]
[[Datei:Panstamp_Fensterkontaktsensor_v1.0_Fertig.jpg|200px|thumb|right|fertiger Aufbau]]


Alle benötigten Dateien sind im Repo zu finden:

* Sketch
* EnvironmentSensor.xml
* Target3001 Dateien

Unbestückte Platinen können beim Ersteller, falls vorrätig, angefragt und erworben werden.

WICHTIG! Schaltplan und Platinen sind für den Panstamp NRG 2.

### Bauteilliste:

|Bauteil|Bezeichnung|Shop|BauteilNr|
|:---|:---|:---|:---|
|  C1|  Keramikkondensator Typ:X7R 100nF|  Reichelt|  X7R-G0805 100N| 
|  X1, X2|  Keramikkondensator Typ:X7R 2,2uF|  Reichelt|  X7R-G0805 2,2/25|
|  IC2|  Linearregler LP2985|  Reichelt|  LP 2985 IM5-3,3|- 
|  K1-K9 (9x)|  Anreihklemme 3,5mm 2Pol |  IT-WNS|  AK-3.5-2-GY|- 
|  R2, R3, R4|  SMD Widerstand 1k |  Reichelt <br> IT-WNS|  SMD-0805 1K <br> R-1K-0805-1|- 
|  X3-X10 (8x)|  Keramikkondensator Typ:X7R/X5R 4,7uF|  Reichelt <br> IT-WNS|  X7R-G0805 4,7/25 <br> C-4U7-0805|- 
|  J1|  Stiftleiste 1x20 pol. gewinkelt, 2,54mm  |  IT-WNS|  SL-1x20-254-G|-
|  S1|  SMD Mikrotaster, Drucktaster, Print Miniatur 5x5x1,5mm|  Ebay||-
|  IC1|  panStamp NRG2|  panstamp.com|  panStamp NRG2|

Die Antenne muss 86mm lang sein.

# Firmware flashen
In der Arduino IDE unter "Einstellungen" folgende Board-Verwalter-URL einsetzen:<br>
<code>http://panstamp.org/arduino/package_panstamp_index.json</code><br>
Anschließend das "panstamp NRG" Board über den Boardmanager installieren und als Board "NRG2" auswählen.

Es muss die SWAP Library installiert werden:<br>
<code>https://github.com/panStamp/swap</code><br>
und anschließend den Sketch kompilieren.
Wie der Panstamp NRG2 mit dem Programmer verkabelt wird, ist hier beschrieben: https://github.com/panStamp/panstamp/wiki/panStamp-NRG-2.-Technical-details

# Hinweise zum Aufbau
## Hinweise zum Betrieb mit FHEM
### Configuration
Nach upload des Sketches auf den Panstamp ist zuerst die korrekte Kennung über das Register 09 sowie der gewünschte Sendezyklus über das Register 0A zu setzen (siehe  [Inbetriebnahme](https://wiki.fhem.de/wiki/PanStamp#Neue_panStamps_in_Betrieb_nehmen) zum den panStamps).
Anschließend ist die Konfiguration mittels Register 0B zu setzen:

|  Bezeichnung|  Länge|  Position|  Beschreibung|
|:---|:---|:---|:---|
|  Gruppe_Pin10|  1 Byte|  0|  Nummer der Gruppe für den ReedEingang D10, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin11|  1 Byte|  1|  Nummer der Gruppe für den ReedEingang D11, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin12|  1 Byte|  2|  Nummer der Gruppe für den ReedEingang D12, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin13|  1 Byte|  3|  Nummer der Gruppe für den ReedEingang D13, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin14|  1 Byte|  4|  Nummer der Gruppe für den ReedEingang D14, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin15|  1 Byte|  5|  Nummer der Gruppe für den ReedEingang D15, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin16|  1 Byte|  6|  Nummer der Gruppe für den ReedEingang D16, Erlaubt: 1-7, 0 ist deaktiviert|-
|  Gruppe_Pin17|  1 Byte|  7|  Nummer der Gruppe für den ReedEingang D17, Erlaubt: 1-7, 0 ist deaktiviert|-
|  BlinkOnEvtTime|  2 Byte |  8-9|  Dauer in ms, wie lange die LED nach einem Event dauerleuchten soll|-
|  BlinkOnIdleTime|  2 Byte |  10-11|  Dauer in ms, wie lange die LED im Idle-Modus dauerleuchten soll|-
|  BlinkOffTime|  2 Byte |  12-13|  Dauer in ms, wie lange die LED im Idle-Modus aus sein soll|

### Setzen der Konfiguration
Ein Register ist als Hex-Wert zu setzen. Folgende Tabelle zeigt eine exemplarische Konfiguration:

|  SubRegister|  Wert|  Binär Wert|  Hex Wert|  Comments|
|:---|:---|:---|:---|:---|
|  Gruppe_Pin10|  1|  1|  01|  Zuordnung D10 zur Gruppe 1|-
|  Gruppe_Pin11|  1|  1|  01|  Zuordnung D11 zur Gruppe 1|-
|  Gruppe_Pin12|  2|  2|  02|  Zuordnung D12 zur Gruppe 2|-
|  Gruppe_Pin13|  2|  2|  02|  Zuordnung D13 zur Gruppe 2|-
|  Gruppe_Pin14|  3|  3|  03|  Zuordnung D14 zur Gruppe 3|-
|  Gruppe_Pin15|  0|  0|  00|  Deaktivierung D15|-
|  Gruppe_Pin16|  0|  0|  00|  Deaktivierung D16|-
|  Gruppe_Pin17|  0|  0|  00|  Deaktivierung D17|-
|  BlinkOnEvtTime|  5000|  1001110001000|  1388|  Bei einem Event leuchtet die LED 5sek dauerhaft|-
|  BlinkOnIdleTime|  50|  110010|  0032|  Im Idle Mode leuchtet die LED 50ms|-
|  BlinkOffTime|  10000|  10011100010000|  2710|  Im  Idle Mode leuchtet die LED nur alle 10sek|

In obigem Fall ist das Konfigurationsregister wie folgt aufgebaut: <br>
<code>2710 0032 1388 00 00 00 03 02 02 01 01</code> <br>

Aufgrunddessen wird die Konfiguration folgendermaßen gesetzt:<br>
<code>set MySwapDevice regSet 0B 2710003213880000000302020101</code>

Es ist auch möglich die einzelnen Endpoints unabhängig voneinander zu setzen. z.B.:<br>
<code>set MySwapDevice regSet 0B.2 02</code>

Diese Konfiguration bedeutet folgendes:
* D10 und D11 gehören zur Gruppe 01
* D12 und D13 gehören zur Gruppe 02
* D14 gehört alleine zur Gruppe 03
* D15,D16,D17 gehören zur 0-Gruppe ist sind damit deaktiviert. Die Binarstati werden zwar regelmäßig mit übertragen, in der Gruppenauswertung spielen diese Pins aber keine Rolle.

Der Gruppenstatus würde dann wie folgt gesetzt:
* Wenn D10 und D11 offen ist hat die Gruppe ebenfalls den Status **Offen**
* Wenn D10 und D11 geschlossen ist hat die Gruppe den Status ebenfalls **Geschlossen**
* Wenn D10 und D11 unterschiedlichen Status haben ist ist die Gruppe **Angeklappt**
* Analog dazu D12/D13 mit der Gruppe 02
* Wenn D14 offen ist, hat die Gruppe 03 den Status **Offen**
* Wenn D14 geschlossen ist, hat die Gruppe 03 den Status **Geschlossen**
* Die Gruppe 03 kann NICHT den Status **Angeklappt** annehmen, da nur ein Pin dieser Gruppe zugeordnet ist

Die LEDs würden sich dann wie folgt verhalten:
* Nach einem Event leuchtet die entsprechende LED 5sek dauerhaft, danach blinkt sie für 50ms alle 10sek

## bereitgestellte Readings
Folgende Readings werden zusätzlich zu den Konfigurations-Readings in FHEM bereitgestellt:

|  Name des Reading|  Datentyp |  Beschreibung|
|:---|:---|:---|
|  0C.0-Binary_0 -> 0C.7-Binary_7|  [0-1]|  aktueller Status des ReedEingangs D10-D17 <br> 0 -> Reedkontakt geschlossen<br>1 -> Reedkontakt offen|-
|  0D.0-Gruppe_1 -> 0D.7-Gruppe_8|  [0-2]|  aktueller Status der Gruppe 1-7 <br> 0 -> Geschlossen<br>1 -> Angeklappt<br> 2 -> Offen|

== Beschreibung des Gruppenverhaltens ==
Mehrere Eingänge können zu einer Gruppe zugeordnet werden.
Der Status einer Gruppe wird nach folgendem Verhalten festgelegt:
* Sind alle Reed-Kontakte der zugeordneten Pins einer Gruppe geschlossen, so ist der Status der Gruppe: **Geschlossen**
* Sind alle Reed-Kontakte der zugeordneten Pins einer Gruppe offen, so ist der Status der Gruppe: **Offen**
* Sind die einzelnen Stati der Reed-Kontakte der zugeordneten Pins einer Gruppe sowohl geschlossen als auch offen, so ist der Status der Gruppe: **Angeklappt**
* Wurde nur ein Pin einer Gruppe zugeordnet, so nimmt die Gruppe nur den Status **Offen**/**Geschlossen** an

## Beschreibung des Sendeverhaltens
Der aktuelle Status der Pins als auch der Status der Gruppen werden bei folgenden Events volständig übertragen:
* gemäß TX-Intervall eingestelltes Sendeintervall
* sofort bei Auftreten eines Events (Änderung eines Status an einem Pin)

## Einbindung TabletUI mit lesbarem Status
[[Datei:Panstamp_Fensterkontaktsensor_v1.0_ftui_Fenster.jpg|200px|thumb||Tablet-UI Fenster]]
[[Datei:Panstamp_Fensterkontaktsensor_v1.0_ftui_FensterRolladen.jpg|200px|thumb||Tablet-UI Fenster&Rolladen]]

In der 99_myUtils.pm ist folgender Eintrag einzubinden

```perl
##############################################
# Übersetzt den Status der 
# SWAP Fensterkontakte in Klarnamen
# und Farbwerte für TabletUI
##############################################

sub
SWAP_WindowStateSensor($$$) {
  my ($device, $reading, $value) = @_;
  my $state = "??";
  my $color = "";
  my $group = 0;

  $group = $reading;  
  $group =~ s/.*-Gruppe_(\d)*.*/$1/;

  if ($value eq "00") {
    $state = "closed";
    $color = "#23a013";
  } elsif ($value eq "01") {
    $state = "tilted";
    $color = "#f4d742";
  } elsif ($value eq "02") {
    $state = "open";
    $color = "#F70A0A";
  }

  readingsBeginUpdate($defs{$device});
  readingsBulkUpdate($defs{$device}, "Gruppe".$group, $state);
  readingsBulkUpdate($defs{$device}, "Farbwert_Gruppe".$group, $color);
  readingsEndUpdate($defs{$device}, 1);

  return undef;
}
```

Anschließend folgenes Notify erstellt:<br>
<pre>
define NTFY_SWAP_Fensterstatus notify SWAP_.*:0D.*-Gruppe.* {SWAP_WindowStateSensor($NAME, $EVTPART0, $EVTPART1)}\
attr NTFY_SWAP_Fensterstatus DbLogExclude .*
attr NTFY_SWAP_Fensterstatus room SWAP
</pre>

Damit wird sowohl ein lesbarer Status als auch der in TabletUI anzuzeigende Farbwert gesetzt.

In TabletUI könnte dann zb. zusammen mit dem Rolladenstatus auch der Fensterstatus angeziegt werden. Im Platzhalter "par_SC_dev:par_SC_Reading" wird das Device als auch das Reading des Farbwertes gesetzt 
```html
            <div data-type="circlemenu" data-direction="bottom-half" data-circle-radius="120" data-item-diameter="67"
               style="font-size:130%;">
              <ul>
                <li>
                   <div data-type="symbol" data-device="par_RL_dev" data-get="level"
                       data-icons='["oa-fts_shutter_90","oa-fts_shutter_80","oa-fts_shutter_70","oa-fts_shutter_60","oa-fts_shutter_50",
                                    "oa-fts_shutter_50","oa-fts_shutter_40","oa-fts_shutter_30","oa-fts_shutter_20",
                                     "oa-fts_shutter_10","oa-fts_window_2w"]'
                       data-states='["0","10","20","30","40","50","60","70","80","90","100"]'
                       data-on-color="#2A2A2A"
                       data-on-background-color="par_SC_dev:par_SC_Reading"
                       data-off-background-color="par_SC_dev:par_SC_Reading"
                       data-background-icon="fa-circle">
                   </div>
                </li>
                <li> <div data-type="push" data-device="par_RL_dev" data-icon="oa-fts_shutter_up" data-set-on="on"></div></li>
                <li> <div data-type="push" data-device="par_RL_dev" data-icon="oa-fts_window_1w_tilt"  data-set-on="Lueftung"></div></li>
                <li> <div data-type="push" data-device="par_RL_auto" data-icon="oa-fts_shutter_automatic"  data-set-on="automatic"></div></li>
                <li> <div data-type="push" data-device="par_RL_auto" data-icon="oa-fts_shutter_manual"  data-set-on="manual"></div></li>
                <li> <div data-type="push" data-device="par_RL_dev" data-icon="oa-fts_sunblind" data-set-on="Beschatten"></div></li>
                <li> <div data-type="push" data-device="par_RL_dev" data-icon="oa-fts_shutter_down" data-set-on="off"></div></li>
              </ul>
            </div>
```

## Weitere Hinweise
Keine.

## Links ##

* [PanStamp Home](http://www.panstamp.com/home)
