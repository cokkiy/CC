use anyhow::Result;

#[derive(Debug, Clone, Default)]
pub struct InterfaceCounterSnapshot {
    pub if_name: String,
    pub bytes_received: u64,
    pub bytes_sented: u64,
    pub unicast_packet_received: u64,
    pub unicast_packet_sented: u64,
    pub multicast_packet_received: u64,
    pub multicast_packet_sented: u64,
}

#[derive(Debug, Clone, Default)]
pub struct NetworkCounterSnapshot {
    pub datagrams_received: u64,
    pub datagrams_sent: u64,
    pub datagrams_discarded: u64,
    pub datagrams_with_errors: u64,
    pub segments_received: u64,
    pub segments_sent: u64,
    pub errors_received: u64,
    pub current_connections: u64,
    pub reset_connections: u64,
    pub interface_counters: Vec<InterfaceCounterSnapshot>,
}

pub fn collect() -> Result<NetworkCounterSnapshot> {
    platform::collect()
}

#[cfg(target_os = "linux")]
mod platform {
    use super::{InterfaceCounterSnapshot, NetworkCounterSnapshot};
    use anyhow::{Context, Result};
    use std::collections::HashMap;

    pub fn collect() -> Result<NetworkCounterSnapshot> {
        let snmp = std::fs::read_to_string("/proc/net/snmp").context("read /proc/net/snmp")?;
        let dev = std::fs::read_to_string("/proc/net/dev").context("read /proc/net/dev")?;

        let udp = parse_protocol_section(&snmp, "Udp");
        let tcp = parse_protocol_section(&snmp, "Tcp");

        let interface_counters = parse_dev(&dev);
        let datagrams_discarded = udp.get("NoPorts").copied().unwrap_or_default()
            + udp.get("RcvbufErrors").copied().unwrap_or_default()
            + udp.get("SndbufErrors").copied().unwrap_or_default();

        Ok(NetworkCounterSnapshot {
            datagrams_received: udp.get("InDatagrams").copied().unwrap_or_default(),
            datagrams_sent: udp.get("OutDatagrams").copied().unwrap_or_default(),
            datagrams_discarded,
            datagrams_with_errors: udp.get("InErrors").copied().unwrap_or_default(),
            segments_received: tcp.get("InSegs").copied().unwrap_or_default(),
            segments_sent: tcp.get("OutSegs").copied().unwrap_or_default(),
            errors_received: tcp.get("InErrs").copied().unwrap_or_default(),
            current_connections: tcp.get("CurrEstab").copied().unwrap_or_default(),
            reset_connections: tcp.get("EstabResets").copied().unwrap_or_default(),
            interface_counters,
        })
    }

    fn parse_protocol_section(content: &str, section: &str) -> HashMap<String, u64> {
        let mut map = HashMap::new();
        let mut lines = content.lines().peekable();

        while let Some(header_line) = lines.next() {
            let Some(value_line) = lines.next() else {
                break;
            };

            if !header_line.starts_with(section) || !value_line.starts_with(section) {
                continue;
            }

            let headers = header_line.split_whitespace().skip(1);
            let values = value_line.split_whitespace().skip(1);

            for (key, value) in headers.zip(values) {
                if let Ok(number) = value.parse::<u64>() {
                    map.insert(key.to_string(), number);
                }
            }
        }

        map
    }

    fn parse_dev(content: &str) -> Vec<InterfaceCounterSnapshot> {
        content
            .lines()
            .skip(2)
            .filter_map(|line| {
                let (name, raw_values) = line.split_once(':')?;
                let values = raw_values
                    .split_whitespace()
                    .filter_map(|value| value.parse::<u64>().ok())
                    .collect::<Vec<_>>();

                if values.len() < 16 {
                    return None;
                }

                let multicast_received = values[7];
                let received_packets = values[1];
                let sent_packets = values[9];

                Some(InterfaceCounterSnapshot {
                    if_name: name.trim().to_string(),
                    bytes_received: values[0],
                    bytes_sented: values[8],
                    unicast_packet_received: received_packets.saturating_sub(multicast_received),
                    unicast_packet_sented: sent_packets,
                    multicast_packet_received: multicast_received,
                    multicast_packet_sented: 0,
                })
            })
            .collect()
    }
}

#[cfg(windows)]
mod platform {
    use super::{InterfaceCounterSnapshot, NetworkCounterSnapshot};
    use anyhow::{Result, anyhow};
    use windows::Win32::Foundation::NO_ERROR;
    use windows::Win32::NetworkManagement::IpHelper::{
        FreeMibTable, GetIfTable2, GetTcpStatisticsEx, GetUdpStatisticsEx, MIB_IF_TABLE2,
        MIB_TCPSTATS_LH, MIB_UDPSTATS,
    };
    use windows::Win32::Networking::WinSock::AF_INET;

    pub fn collect() -> Result<NetworkCounterSnapshot> {
        unsafe {
            let mut udp_stats = MIB_UDPSTATS::default();
            let udp_status = GetUdpStatisticsEx(&mut udp_stats, AF_INET.0 as u32);
            if udp_status != NO_ERROR.0 {
                return Err(anyhow!("GetUdpStatisticsEx failed with {}", udp_status));
            }

            let mut tcp_stats = MIB_TCPSTATS_LH::default();
            let tcp_status = GetTcpStatisticsEx(&mut tcp_stats, AF_INET.0 as u32);
            if tcp_status != NO_ERROR.0 {
                return Err(anyhow!("GetTcpStatisticsEx failed with {}", tcp_status));
            }

            let mut table: *mut MIB_IF_TABLE2 = std::ptr::null_mut();
            let table_status = GetIfTable2(&mut table);
            if table_status != NO_ERROR.0 {
                return Err(anyhow!("GetIfTable2 failed with {}", table_status));
            }

            let interface_counters = if table.is_null() {
                Vec::new()
            } else {
                let table_ref = &*table;
                let count = table_ref.NumEntries as usize;
                let rows_ptr = table_ref.Table.as_ptr();
                let rows = std::slice::from_raw_parts(rows_ptr, count);

                let counters = rows
                    .iter()
                    .map(|row| {
                        let name = String::from_utf16_lossy(&row.Alias)
                            .trim_end_matches('\0')
                            .to_string();

                        InterfaceCounterSnapshot {
                            if_name: name,
                            bytes_received: row.InOctets,
                            bytes_sented: row.OutOctets,
                            unicast_packet_received: row.InUcastPkts,
                            unicast_packet_sented: row.OutUcastPkts,
                            multicast_packet_received: row.InNUcastPkts,
                            multicast_packet_sented: row.OutNUcastPkts,
                        }
                    })
                    .collect::<Vec<_>>();

                FreeMibTable(table as _);
                counters
            };

            Ok(NetworkCounterSnapshot {
                datagrams_received: udp_stats.dwInDatagrams as u64,
                datagrams_sent: udp_stats.dwOutDatagrams as u64,
                datagrams_discarded: udp_stats.dwNoPorts as u64,
                datagrams_with_errors: udp_stats.dwInErrors as u64,
                segments_received: tcp_stats.dwInSegs as u64,
                segments_sent: tcp_stats.dwOutSegs as u64,
                errors_received: tcp_stats.dwInErrs as u64,
                current_connections: tcp_stats.dwCurrEstab as u64,
                reset_connections: tcp_stats.dwEstabResets as u64,
                interface_counters,
            })
        }
    }
}

#[cfg(not(any(target_os = "linux", windows)))]
mod platform {
    use super::NetworkCounterSnapshot;
    use anyhow::Result;

    pub fn collect() -> Result<NetworkCounterSnapshot> {
        Ok(NetworkCounterSnapshot::default())
    }
}
