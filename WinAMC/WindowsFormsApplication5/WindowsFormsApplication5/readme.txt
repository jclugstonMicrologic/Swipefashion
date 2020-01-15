
/*|***************************************************************************/
/*|PROJECT:  WinFTC
/*|***************************************************************************/

version

1.00     beta

1.01     Current scaling changed to match Gain =13 (from Gain =40)
08/24/15

1.02     'PULL' colum changed to run time
08/29/15 Run time diplayed when applicable (ball trigger start to motor stop)
	     Ball trigger row highlighted in download 
       
1.03     Flag motor run time in log display table when (ball count == setup count)
09/05/15 

1.04     "FTC Setup Successfully" and type of setup (deploy/shop test) message added
10/10/15 Deploy/Shop Test setting changed to drop down list
	     Log table cleared upon entering Log window
		 PC timestamp sent to FTC
		 time column in download shows real time

1.05     Deploy delay time added to GUI
11/15/15 

1.06     Current scaling changed to match Gain =5 (from Gain =13)
01/10/16 Log timestamp adjustments
	     msec resolution added to time logs
		 Log start time offset field added

1.07     100th of second resolution added to inital PC timestamp
01/23/16 

1.08     Add ball state description text box
03/05/16 Add stuck switch indicator in IO controlbox

1.09     Download new larger log
06/05/16 

1.10     Add log type drop down list (standard/detailed)
06/18/16 

1.11     Change default log extenstion from *.csv to *.txt
09/04/16 Warn user if real time updates are running when download has been selected
		 Display memory type (extended or standard).  Requires FTC v0.19 or higher
		 Prevent 0 ball count as a trigger count

1.12     Single/dual switch option added
11/11/16 Mark log table (red) when no ball count detected due to too fast ball travel

1.13     Downloading 128 records when internal EEPROM used fixed, only download the 14 records
11/30/16 
