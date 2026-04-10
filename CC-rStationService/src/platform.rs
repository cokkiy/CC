use std::net::{SocketAddr, UdpSocket};
use std::process::Command;

use anyhow::{Context, Result, anyhow};

pub fn reboot(force: bool) -> Result<()> {
    #[cfg(windows)]
    {
        let mut args = vec!["/r", "/t", "0"];
        if force {
            args.insert(1, "/f");
        }
        run_command("shutdown", &args)
    }

    #[cfg(unix)]
    {
        let _ = force;
        run_command("shutdown", &["-r", "now"])
    }
}

pub fn shutdown() -> Result<()> {
    #[cfg(windows)]
    {
        run_command("shutdown", &["/s", "/t", "0"])
    }

    #[cfg(unix)]
    {
        run_command("shutdown", &["now"])
    }
}

pub fn send_udp_command(
    target: SocketAddr,
    command_code: i32,
    control_params: &[u8],
) -> Result<()> {
    let socket = UdpSocket::bind("0.0.0.0:0").context("bind UDP socket")?;
    let mut payload = [0u8; 512];
    payload[..4].copy_from_slice(&command_code.to_le_bytes());

    let data_len = control_params.len().min(payload.len() - 4);
    payload[4..4 + data_len].copy_from_slice(&control_params[..data_len]);

    socket
        .send_to(&payload, target)
        .with_context(|| format!("send UDP display command to {target}"))?;

    Ok(())
}

#[cfg(unix)]
pub fn daemonize() -> Result<()> {
    nix::unistd::daemon(true, false).map_err(|err| anyhow!("daemonize failed: {err}"))
}

#[cfg(not(unix))]
pub fn daemonize() -> Result<()> {
    Ok(())
}

fn run_command(command: &str, args: &[&str]) -> Result<()> {
    let status = Command::new(command)
        .args(args)
        .status()
        .with_context(|| format!("spawn {command}"))?;

    if status.success() {
        return Ok(());
    }

    Err(anyhow!("{command} exited with {status}"))
}
