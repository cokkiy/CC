//! Station Groups Module
//!
//! This module provides station group management for organizing stations
//! into logical groups for batch operations and targeting.

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use uuid::Uuid;

/// A station group for organizing stations.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StationGroup {
    /// Unique identifier
    pub id: Uuid,
    /// Group name
    pub name: String,
    /// Group description
    pub description: Option<String>,
    /// Station IDs belonging to this group
    #[serde(default)]
    pub station_ids: Vec<String>,
    /// Creation timestamp
    pub created_at: DateTime<Utc>,
    /// Last update timestamp
    pub updated_at: DateTime<Utc>,
    /// Creator identifier
    pub created_by: Option<String>,
}

impl StationGroup {
    /// Create a new station group
    pub fn new(name: impl Into<String>) -> Self {
        let now = Utc::now();
        Self {
            id: Uuid::new_v4(),
            name: name.into(),
            description: None,
            station_ids: Vec::new(),
            created_at: now,
            updated_at: now,
            created_by: None,
        }
    }

    /// Set the description
    pub fn with_description(mut self, description: impl Into<String>) -> Self {
        self.description = Some(description.into());
        self
    }

    /// Set the creator
    pub fn created_by(mut self, creator: impl Into<String>) -> Self {
        self.created_by = Some(creator.into());
        self
    }

    /// Add a station to the group
    pub fn add_station(mut self, station_id: impl Into<String>) -> Self {
        let station_id = station_id.into();
        if !self.station_ids.contains(&station_id) {
            self.station_ids.push(station_id);
            self.updated_at = Utc::now();
        }
        self
    }

    /// Add multiple stations to the group
    pub fn add_stations(mut self, station_ids: impl IntoIterator<Item = impl Into<String>>) -> Self {
        for station_id in station_ids {
            let station_id = station_id.into();
            if !self.station_ids.contains(&station_id) {
                self.station_ids.push(station_id);
            }
        }
        self.updated_at = Utc::now();
        self
    }

    /// Remove a station from the group
    pub fn remove_station(mut self, station_id: &str) -> Self {
        self.station_ids.retain(|id| id != station_id);
        self.updated_at = Utc::now();
        self
    }

    /// Check if the group contains a station
    pub fn contains_station(&self, station_id: &str) -> bool {
        self.station_ids.iter().any(|id| id == station_id)
    }

    /// Get the number of stations in the group
    pub fn station_count(&self) -> usize {
        self.station_ids.len()
    }
}

/// Filter options for querying station groups.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct StationGroupFilter {
    /// Filter by name (partial match)
    pub name: Option<String>,
    /// Filter by creator
    pub created_by: Option<String>,
    /// Sort by field
    #[serde(default)]
    pub sort_by: StationGroupSortField,
    /// Sort ascending or descending
    #[serde(default = "default_sort_desc")]
    pub sort_desc: bool,
    /// Pagination offset
    #[serde(default)]
    pub offset: u64,
    /// Pagination limit
    #[serde(default = "default_limit")]
    pub limit: u64,
}

fn default_sort_desc() -> bool {
    true
}

fn default_limit() -> u64 {
    50
}

/// Fields that can be used for sorting station groups.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum StationGroupSortField {
    CreatedAt,
    UpdatedAt,
    Name,
    StationCount,
}

impl Default for StationGroupSortField {
    fn default() -> Self {
        Self::CreatedAt
    }
}

/// Statistics about station groups.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct StationGroupStats {
    /// Total number of groups
    pub total_groups: u64,
    /// Total stations across all groups
    pub total_stations: u64,
    /// Average stations per group
    pub avg_stations_per_group: f64,
}