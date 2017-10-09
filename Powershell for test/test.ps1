	  #set the current location
	  $x = $MyInvocation.MyCommand.Definition
      $x = Split-Path -Parent $MyInvocation.MyCommand.Definition
	  
	  Start-Process -FilePath IPC_SharedMemory.exe -ArgumentList "sub"
  	  Start-Process -FilePath IPC_SharedMemory.exe -ArgumentList "sub"  
	  Start-Process -FilePath IPC_SharedMemory.exe -ArgumentList "pub" 
