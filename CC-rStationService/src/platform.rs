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
