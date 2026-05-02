## Works Need TODO
### The cc-rclient
impr means improve
1. impr: In the client's Device page, replace the "clear filters" button with a small Icon only button
2. bugfix: the device page Device panel can't scroll to bottom, some stationCard only can see a title
3. impr: in batch page, create new batch task should include "stop app" and "restart app" task types, or merge the "start app","stop app" and "restart app" into one type: "App Control" with a parameter indicate which subtype control
4. impr: the batch task also should include push/pull file or folder to/from device
5. dig: the batch task type "Command" really means what? and difference between "Script", script can only exec in the device, command means push some control msg to device or interact between client and the device such as trans file, or exec in client and detect the device by "ping", in such, which cmds should expose and support?
6. impr: based on 5, the Task Content  editor should support syntax highlights
7. bugfix: group and tag can't assigned to IoT simulator device
8. impr: in the device page's Runtime and Tools panel, move the endpoint block to a seprate row, remove "Batch Captures" and "Remote Files" block
9. bugfix: Device editor can't save "Startup Programs" and "Monitor Processes",