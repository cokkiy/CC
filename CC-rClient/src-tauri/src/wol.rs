use crate::models::Station;
use crate::storage::StorageError;
use std::net::{Ipv4Addr, SocketAddrV4, UdpSocket};

pub fn send_magic_packets(station: &Station) -> Result<(), StorageError> {
    let socket = UdpSocket::bind(SocketAddrV4::new(Ipv4Addr::UNSPECIFIED, 0))?;
    socket.set_broadcast(true)?;

    let mut sent = false;
    for interface in &station.network_interfaces {
        let mac = normalize_mac(&interface.mac);
        if mac.len() != 6 {
            continue;
        }

        let packet = build_magic_packet(&mac);
        for _ in 0..5 {
            socket.send_to(&packet, SocketAddrV4::new(Ipv4Addr::BROADCAST, 5555))?;
        }
        sent = true;
    }

    if sent {
        Ok(())
    } else {
        Err(StorageError::Io(std::io::Error::new(
            std::io::ErrorKind::InvalidInput,
            format!("Station {} has no valid MAC addresses.", station.name),
        )))
    }
}

fn normalize_mac(value: &str) -> Vec<u8> {
    let stripped = value
        .split(':')
        .next_back()
        .unwrap_or(value)
        .replace('-', "")
        .replace(':', "");

    stripped
        .as_bytes()
        .chunks(2)
        .filter_map(|chunk| {
            if chunk.len() != 2 {
                return None;
            }
            std::str::from_utf8(chunk)
                .ok()
                .and_then(|hex| u8::from_str_radix(hex, 16).ok())
        })
        .collect()
}

fn build_magic_packet(mac: &[u8]) -> Vec<u8> {
    let mut packet = vec![0xFF; 6];
    for _ in 0..16 {
        packet.extend_from_slice(mac);
    }
    packet
}
