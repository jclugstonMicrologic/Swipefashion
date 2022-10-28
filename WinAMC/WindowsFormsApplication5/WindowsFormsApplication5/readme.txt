
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

1.05	  Add drive space check before capture to ensure there is enough space (2GB)
mm/dd/22  

1.06	  Add led dimmer slider bar
06/06/22  

1.07	  Capture photos one camera at a time
09/06/22 

1.08	  Start camera using MXID.  
09/09/22  Fix up mannequin rotating before 2nd, 3rd, etc camera has started

1.09	  Ensure Profile and Camera directories exist
10/28/22  Change lighting control from scroll bar to text box (up/down select)