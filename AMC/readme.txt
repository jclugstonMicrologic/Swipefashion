
/*|***************************************************************************/
/*|PROJECT:  ApparelSize AMC
/*|			 
/*|***************************************************************************/

version

0.00        beta release
09/28/2020  compenstaed P and T readings sent to terminal (UART4)
            push button triggers solenoid on or off
            using HSI clock (no HSE available on eval board)
            
0.00        P, and T sampling incresased to 10/sec
10/dd/2020  debounce bush button
            Terminal messaging disabled
            PC UART comms enabled (comms to PC)
            BLE module updated to send press data periodically
            
            
0.05        Mods for real board, move to HSE (move from dev kit)
02/16/2021  BleMachine updates


0.06        Fix up bluetooth start up timing, 
03/25/2021  previous version could miss some ASCII starup messages


0.07        Add PWM channel for lighting (PB10/TIM2)
04/dd/2021  
