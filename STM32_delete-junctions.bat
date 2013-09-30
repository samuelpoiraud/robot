echo "Supression des jonctions"
junction.exe -d 2_Strategie\CurrentSoftwareDsPIC30F\QS
junction.exe -d 4_Balises\Software_Emetteur_US\QS
junction.exe -d 4_Balises\Software_Mere\QS
junction.exe -d 4_Balises\Software_Recepteur_IR\Project\QS
junction.exe -d 4_Balises\Software_Recepteur_US\QS
junction.exe -d 5_Supervision\Current_Software\QS
junction.exe -d 5_Supervision\SD_Project\QS
junction.exe -d 5_Supervision\Test_hard_SD\QS

junction.exe -d 1_Propulsion\CurrentSoftware\QS
junction.exe -d 1_Propulsion\CurrentSoftware\stm32f4xx
junction.exe -d 2_Strategie\CurrentSoftwareSTM32\QS
junction.exe -d 2_Strategie\CurrentSoftwareSTM32\stm32f4xx
junction.exe -d 3_Actionneur\CurrentSoftware\QS
junction.exe -d 3_Actionneur\CurrentSoftware\stm32f4xx
junction.exe -d R_D\HokuyoUTM30LX\Software\QS
junction.exe -d R_D\HokuyoUTM30LX\Software\stm32f4xx
pause