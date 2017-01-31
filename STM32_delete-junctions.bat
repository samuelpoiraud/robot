
echo "Supression des jonctions DISPIC"
junction.exe -d 4_Balises\Software_Emetteur_US\QS
junction.exe -d 4_Balises\Software_Recepteur_IR\Project\QS

echo "Supression des jonctions STM32"
junction.exe -d 1_Propulsion\CurrentSoftware\QS
junction.exe -d 1_Propulsion\CurrentSoftware\stm32f4xx
junction.exe -d 2_Strategie\CurrentSoftwareSTM32\QS
junction.exe -d 2_Strategie\CurrentSoftwareSTM32\stm32f4xx
junction.exe -d 3_Actionneur\CurrentSoftwareSTM32\QS
junction.exe -d 3_Actionneur\CurrentSoftwareSTM32\stm32f4xx
junction.exe -d 4_Balises\Balise_Emettrice\CurrentSoftware\QS
junction.exe -d 4_Balises\Balise_Emettrice\CurrentSoftware\stm32f4xx
junction.exe -d 4_Balises\Balise_Receptrice\CurrentSoftware\QS
junction.exe -d 4_Balises\Balise_Receptrice\CurrentSoftware\stm32f4xx
junction.exe -d 6_IHM\CurrentSoftware\QS
junction.exe -d 6_IHM\CurrentSoftware\stm32f4xx
junction.exe -d R_D\HokuyoUTM30LX\Software\QS
junction.exe -d R_D\HokuyoUTM30LX\Software\stm32f4xx
junction.exe -d R_D\HokuyoUTM30LX\SoftwareURBCallbackInIT\QS
junction.exe -d R_D\HokuyoUTM30LX\SoftwareURBCallbackInIT\stm32f4xx
junction.exe -d R_D\projet_matrice_led\CurrentSoftware\QS
junction.exe -d R_D\projet_matrice_led\CurrentSoftware\stm32f4xx
junction.exe -d R_D\carteMosfet\CurrentSoftware\QS
junction.exe -d R_D\carteMosfet\CurrentSoftware\stm32f4xx
pause
