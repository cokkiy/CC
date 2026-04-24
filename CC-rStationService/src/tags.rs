//! Tags Module
//!
//! This module provides tag management for stations, allowing stations
//! to be categorized and targeted by tag-based selectors.

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use uuid::Uuid;

/// A tag for categorizing stations.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Tag {
    /// Unique identifier
    pub id: Uuid,
    /// Tag name
    pub name: String,
    /// Tag description
    pub description: Option<String>,
    /// Tag color (hex format, e.g., "#FF5733")
    pub color: Option<String>,
    /// Creation timestamp
    pub created_at: DateTime<Utc>,
    /// Last update timestamp
    pub updated_at: DateTime<Utc>,
    /// Creator identifier
    pub created_by: Option<String>,
}

impl Tag {
    /// Create a new tag
    pub fn new(name: impl Into<String>) -> Self {
        let now = Utc::now();
        Self {
            id: Uuid::new_v4(),
            name: name.into(),
            description: None,
            color: None,
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

    /// Set the color
    pub fn with_color(mut self, color: impl Into<String>) -> Self {
        self.color = Some(color.into());
        self
    }

    /// Set the creator
    pub fn created_by(mut self, creator: impl Into<String>) -> Self {
        self.created_by = Some(creator.into());
        self
    }
}

/// A station's tag assignment.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StationTag {
    /// Station ID
    pub station_id: String,
    /// Tag ID
    pub tag_id: Uuid,
    /// When the tag was assigned
    pub assigned_at: DateTime<Utc>,
    /// Who assigned the tag
    pub assigned_by: Option<String>,
}

impl StationTag {
    /// Create a new station-tag assignment
    pub fn new(station_id: impl Into<String>, tag_id: Uuid) -> Self {
        Self {
            station_id: station_id.into(),
            tag_id,
            assigned_at: Utc::now(),
            assigned_by: None,
        }
    }

    /// Set who assigned the tag
    pub fn assigned_by(mut self, assigned_by: impl Into<String>) -> Self {
        self.assigned_by = Some(assigned_by.into());
        self
    }
}

/// Filter options for querying tags.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct TagFilter {
    /// Filter by name (partial match)
    pub name: Option<String>,
    /// Filter by creator
    pub created_by: Option<String>,
    /// Sort by field
    #[serde(default)]
    pub sort_by: TagSortField,
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

/// Fields that can be used for sorting tags.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum TagSortField {
    CreatedAt,
    UpdatedAt,
    Name,
}

impl Default for TagSortField {
    fn default() -> Self {
        Self::CreatedAt
    }
}

/// Filter options for querying station tags.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct StationTagFilter {
    /// Filter by station ID
    pub station_id: Option<String>,
    /// Filter by tag ID
    pub tag_id: Option<Uuid>,
    /// Sort by field
    #[serde(default)]
    pub sort_by: StationTagSortField,
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

/// Fields that can be used for sorting station tags.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum StationTagSortField {
    AssignedAt,
    StationId,
    TagId,
}

impl Default for StationTagSortField {
    fn default() -> Self {
        Self::AssignedAt
    }
}

/// Statistics about tags.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct TagStats {
    /// Total number of tags
    pub total_tags: u64,
    /// Total station-tag assignments
    pub total_assignments: u64,
    /// Average tags per station (if station_count provided)
    pub avg_tags_per_station: Option<f64>,
}