##  MQTT payload enrichment for full runtime parity
- Disk/Volume total capicity and curent usage
- Total memory
- OS details
- Service path if capable
- Launcher path if capable
- Watched APP running state, include thread count, memory usage, cpu rate,etc
- Network traffic in interface grouped
- Splitting MQTT telemetry data into groups and group level based acquisition interbals and send intervals, such as low level telemetry gathered every 10 seconds but high level data gathered every 1 seconds, user can define the group and level and which telemetry includes in this group, maybe a UI needed and save this group and level and interval value in CC-rStationService.toml 