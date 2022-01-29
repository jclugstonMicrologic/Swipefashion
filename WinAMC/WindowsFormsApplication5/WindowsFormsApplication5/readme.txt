
/*|***************************************************************************/
/*|PROJECT:  WinAMC
/*|***************************************************************************/

version

1.00	  beta
10/dd/20  BTFramework libriares added		 
		  auto connect and get services and characteristics for RN4871
		  auto subscribe and write CCCD subscribe

1.02	  Multiple camera support added (only two tested)
03/14/21  Multiple BLE peripherals functionality (Hi/LO/COMPR boards)

1.03	  Catch and fix rare serial message exception (rxBuffer sized incorrectly)
05/31/21  Modify GUI to fit smaller screeen resloutions
		  Fix camera capture only working once without GUI reset
		  Do not start motor until camera1 has taken a photo 
		  (may need to monitor all cameras before motor start)
		  
1.04	  Add pump on/off control in motor diagnostic screen
01/27/22  add focus option in motor diagnostic screen
		  depthai_demo.py and arg_manager.py modifeid to accept focus values
		  from WinAMC