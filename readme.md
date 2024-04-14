This is a simple implementation to control a relay connected to GPIO 2 of a esp01s module. 
Current code create a web server assuming its connected to a light and allow controlling it through 
a web page. ESP01s is configured as an access point to work stand alone.

HOW TO USE 

Simply connect to the access point(Hotspot) created by the module using wifi, 
it will appear as LightAP and the password is 12345678. 
Then goto http://192.168.4.1 using a browser and use the links to control the relay. There are few useful settings in settings page.
It can be used to change what state the relay should be after a boot (default state).

Options are as follows.
1. Light ON ( Relay is always ON after a power toggle)
2. Light OFF ( Relay is always OFF after a power toggle)
3. Last state ( Relay is restored to the state before power toggle)
4. Power toggle controlled ( Used to turn the relay ON and off using power toggle, Always restore the oposite state before the power toggle)

Additionally the background of the interface become darker when light off. This is used to lower brightness for eye comfort when its been used control a 
light.

Programmed using uart and must use a 3v3 supported UART to usb adapter or a 3v3 dev board.

Please note the code provided is without gurantee. Use at your own risk. 

PCB folder
PDF file: Print it using a laser printer or a photocopier on a gloss paper without flipping and in actual sale for diy pcb etching.
JASON file : EasyEDA project file.
Images: Schematic and 3D board view
*Use a 4x4 female header(PH socket) as a base for the ESP01s of there will be no head room for components underneath the ESP board.

Caution !
Board contain risk of electric shock and fire hazard if adequate precautions arent followed. Use at your own risk.
