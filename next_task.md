## The next step should do
### Phase 10 Client UI Moderate - Completed
1. [x] Device page List mode supports user-selected 1-5 columns.
   - Implemented with the device browser column selector and persisted list column preference.
2. [x] Device page Grid mode no longer pushes Runtime & Tools below the device list on wide screens.
   - Implemented with the two-pane stations workspace and scrollable browser/detail panels.
3. [x] Group page add-device flow supports filtering and batch add.
   - Implemented name/IP filtering, multi-select, select-all filtered devices, and batch add.
4. [x] Tag page supports selecting multiple devices and setting tag values once.
   - Implemented name/IP filtering, multi-select, and batch save for selected devices.
5. [x] Client runtime no longer requires the Vite dev server for normal desktop launch.
   - `scripts/start-all.sh` uses bundled `CC-rClient/dist` assets, and template import supports local JSON/ZIP UI packages.
