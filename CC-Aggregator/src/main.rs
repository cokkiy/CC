//! CC-Aggregator - Message broker for CC IoT Station Service
//! 
//! This is the central message aggregator that:
//! - Connects to MQTT broker (NATS)
//! - Collects telemetry from stations
//! - Forwards data to connected clients via WebSocket

mod config;
mod mqtt;
mod telemetry;
mod websocket;

use anyhow::Result;
use clap::Parser;
use config::AggregatorConfig;
use mqtt::MqttClient;
use std::sync::Arc;
use tokio::sync::mpsc;
use tokio::sync::broadcast;
use tracing::{error, info};
use tracing_subscriber::EnvFilter;
use websocket::WsServer;

/// CLI arguments for CC-Aggregator
#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Path to the configuration file
    #[arg(short, long, default_value = "CC-Aggregator.toml")]
    config: String,
}

/// Initialize tracing/logging
fn init_tracing(config: &config::LoggingConfig) {
    let env_filter = EnvFilter::try_from_default_env()
        .unwrap_or_else(|_| EnvFilter::new(&config.level));
    
    tracing_subscriber::fmt()
        .with_env_filter(env_filter)
        .with_target(true)
        .with_thread_ids(true)
        .with_file(true)
        .with_line_number(true)
        .try_init()
        .ok(); // Ignore if already initialized
}

#[tokio::main]
async fn main() -> Result<()> {
    // Parse CLI arguments
    let args = Args::parse();
    
    // Load configuration
    let config = AggregatorConfig::load(&args.config)
        .unwrap_or_else(|e| {
            eprintln!("[WARN] Failed to load {}: {}", args.config, e);
            eprintln!("[WARN] Using default configuration");
            AggregatorConfig::default()
        });
    
    // Initialize logging
    init_tracing(&config.logging);
    
    info!("=== CC-Aggregator Starting ===");
    info!("Server ID: {}", config.server_id);
    info!("MQTT Broker: {}:{}", config.mqtt.host, config.mqtt.port);
    info!("WebSocket Listen: {}", config.websocket.listen_addr);
    
    // Create MQTT client
    let mqtt_client = MqttClient::new(
        &config.mqtt.host,
        config.mqtt.port,
        &config.mqtt.client_id,
    )?;
    
    // Subscribe to all station topics
    mqtt_client.subscribe_all().await?;
    
    // Create broadcast channel for MQTT messages
    let (mqtt_broadcast_tx, _) = broadcast::channel::<mqtt::MqttMessage>(1000);
    let _mqtt_broadcast_tx_clone = mqtt_broadcast_tx.clone();
    
    // Create mpsc channel for the event loop
    let (tx, rx) = mpsc::channel::<mqtt::MqttMessage>(1000);
    
    // Start MQTT event poller - takes ownership of client
    let eventloop_handle = tokio::spawn(async move {
        let client = mqtt_client;
        let mut eventloop = client.into_eventloop();
        
        // Forward messages from mpsc to broadcast
        let tx_clone = tx.clone();
        tokio::spawn(async move {
            let mut receiver = rx;
            loop {
                match receiver.recv().await {
                    Some(msg) => {
                        if tx_clone.send(msg).await.is_err() {
                            break;
                        }
                    }
                    None => {
                        break;
                    }
                }
            }
        });
        
        if let Err(e) = mqtt::handle_mqtt_events(&mut eventloop, &tx).await {
            error!("MQTT event handler error: {:?}", e);
        }
    });
    
    // Create WebSocket server with cloned broadcast sender
    let ws_server = Arc::new(WsServer::new(mqtt_broadcast_tx.clone()));
    let ws_server_for_processor = ws_server.clone();
    
    // Start WebSocket server
    let listen_addr = config.websocket.listen_addr.clone();
    let ws_handle = {
        let ws_server = ws_server.clone();
        tokio::spawn(async move {
            if let Err(e) = ws_server.start(&listen_addr).await {
                error!("WebSocket server error: {:?}", e);
            }
        })
    };
    
    // Message processing task
    let processor_handle = tokio::spawn(async move {
        let mut receiver: broadcast::Receiver<mqtt::MqttMessage> = ws_server_for_processor.subscribe_to_messages();
        
        while let Ok(msg) = receiver.recv().await {
            // Process incoming MQTT messages
            if let Some((station_id, msg_type)) = msg.parse_topic() {
                match msg_type.as_str() {
                    "telemetry" => {
                        if let Some(telemetry) = msg.parse_json::<telemetry::TelemetryBundle>() {
                            ws_server_for_processor.broadcast_telemetry(&station_id, &telemetry).await;
                        }
                    }
                    "status" => {
                        if let Some(status) = msg.parse_json::<mqtt::StationStatus>() {
                            ws_server_for_processor.broadcast_status(&station_id, &status).await;
                        }
                    }
                    _ => {}
                }
            }
        }
    });
    
    info!("=== CC-Aggregator Started Successfully ===");
    
    // Wait for all tasks
    tokio::select! {
        result = eventloop_handle => {
            if let Err(e) = result {
                error!("Event loop task panicked: {:?}", e);
            }
        }
        result = ws_handle => {
            if let Err(e) = result {
                error!("WebSocket task panicked: {:?}", e);
            }
        }
        result = processor_handle => {
            if let Err(e) = result {
                error!("Processor task panicked: {:?}", e);
            }
        }
    }
    
    Ok(())
}
